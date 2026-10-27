#include "can_messages.h"
#include "can_driver.h"
#include "pdm_settings.h"
#include "efuse.h"
#include "outputs.h"
#include "buttons.h"
#include "temperature.h"
#include "supply_voltage.h"
#include "fault_manager.h"
#include "pdm_control.h"
#include "firmware_version.h"
#include "reset_monitor.h"
#include "scheduler.h"
#include "test_config.h"

extern "C" {
#include "generated/pdm_dbc.h"
}

namespace
{
    PDMCommand command = {};
    uint8_t aliveCounter = 0;

    bool validCounter(uint8_t value)
    {
        return !command.valid || static_cast<uint8_t>(command.rollingCounter + 1U) == value;
    }

    bool sendPacked(uint16_t id, uint8_t length, const uint8_t *data)
    {
#if FEATURE_CAN_ENABLED
        return CAN_SendStandard(id, length, data);
#else
        (void)id; (void)length; (void)data;
        return false;
#endif
    }
}

uint8_t CANMessages_CalculateCRC8(const uint8_t *data, uint8_t length)
{
    uint8_t crc = Settings::CAN_CRC8_INITIAL;
    for (uint8_t i = 0; i < length; ++i)
    {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; ++bit)
            crc = (crc & 0x80U) ? static_cast<uint8_t>((crc << 1U) ^ Settings::CAN_CRC8_POLYNOMIAL) : static_cast<uint8_t>(crc << 1U);
    }
    return crc;
}

void CANMessages_Init() { command = {}; }

void CANMessages_ProcessReceived()
{
#if FEATURE_CAN_ENABLED
    CANFrame frame;
    while (CAN_GetFrame(frame))
    {
        if (frame.extended || frame.remote) continue;

        if (frame.id == PDM_DBC_PDM_COMMAND_FRAME_ID && frame.length == PDM_DBC_PDM_COMMAND_LENGTH)
        {
            struct pdm_dbc_pdm_command_t decoded;
            pdm_dbc_pdm_command_init(&decoded);
            if (pdm_dbc_pdm_command_unpack(&decoded, frame.data, frame.length) != 0) continue;

            const bool crcOk = CANMessages_CalculateCRC8(frame.data, frame.length - 1U) == decoded.command_crc;
            const bool counterOk = validCounter(decoded.command_counter);
            command.efuse1Requested = decoded.e_fuse1_request != 0U;
            command.efuse2Requested = decoded.e_fuse2_request != 0U;
            command.outputPC0Requested = decoded.pc0_request != 0U;
            command.outputPC1Requested = decoded.pc1_request != 0U;
            command.emergencyOffRequested = decoded.emergency_off != 0U;
            command.rollingCounter = decoded.command_counter;
            command.receivedCRC = decoded.command_crc;
            command.counterValid = counterOk;
            command.crcValid = crcOk;
            command.valid = crcOk && counterOk;
            if (command.valid) command.lastValidMessageMs = millis();
        }
        else if (frame.id == PDM_DBC_PDM_FAULT_RESET_FRAME_ID && frame.length == PDM_DBC_PDM_FAULT_RESET_LENGTH)
        {
            struct pdm_dbc_pdm_fault_reset_t decoded;
            pdm_dbc_pdm_fault_reset_init(&decoded);
            if (pdm_dbc_pdm_fault_reset_unpack(&decoded, frame.data, frame.length) != 0) continue;
            if (decoded.reset_key == 0xA55AU)
            {
                if (decoded.reset_e_fuse1) EFuse_ClearFault(EFuseId::Channel1);
                if (decoded.reset_e_fuse2) EFuse_ClearFault(EFuseId::Channel2);
                if (decoded.reset_system) FaultManager_ClearAllNonCritical();
            }
        }
    }
#endif
}

void CANMessages_Task10ms() {}

void CANMessages_SendStatus()
{
    struct pdm_dbc_pdm_status_t m;
    pdm_dbc_pdm_status_init(&m);
    m.e_fuse1_enabled = EFuse_IsEnabled(EFuseId::Channel1);
    m.e_fuse2_enabled = EFuse_IsEnabled(EFuseId::Channel2);
    m.pc0_enabled = Outputs_GetState(OutputId::OutputPC0);
    m.pc1_enabled = Outputs_GetState(OutputId::OutputPC1);
    m.button1_pressed = Buttons_IsPressed(ButtonId::Button1);
    m.button2_pressed = Buttons_IsPressed(ButtonId::Button2);
    m.can_command_valid = CANMessages_HasValidCommand();
    m.system_fault = FaultManager_HasAnyFault();
    m.e_fuse1_state = static_cast<uint8_t>(EFuse_GetState(EFuseId::Channel1));
    m.e_fuse2_state = static_cast<uint8_t>(EFuse_GetState(EFuseId::Channel2));
    m.pdm_state = static_cast<uint8_t>(PDMControl_GetState());
    m.last_command_counter = command.rollingCounter;
    m.alive_counter = aliveCounter++;
    m.firmware_major = FirmwareVersion::MAJOR;
    m.firmware_minor = FirmwareVersion::MINOR;
    uint8_t data[PDM_DBC_PDM_STATUS_LENGTH] = {};
    if (pdm_dbc_pdm_status_pack(data, &m, sizeof(data)) == 0)
        sendPacked(PDM_DBC_PDM_STATUS_FRAME_ID, PDM_DBC_PDM_STATUS_LENGTH, data);
}

void CANMessages_SendCurrents()
{
    struct pdm_dbc_pdm_currents_t m;
    pdm_dbc_pdm_currents_init(&m);
    m.e_fuse1_current = EFuse_GetFilteredCurrentMilliamp(EFuseId::Channel1);
    m.e_fuse2_current = EFuse_GetFilteredCurrentMilliamp(EFuseId::Channel2);
    m.e_fuse1_peak_current = EFuse_GetPeakCurrentMilliamp(EFuseId::Channel1);
    m.e_fuse2_peak_current = EFuse_GetPeakCurrentMilliamp(EFuseId::Channel2);
    uint8_t data[PDM_DBC_PDM_CURRENTS_LENGTH] = {};
    if (pdm_dbc_pdm_currents_pack(data, &m, sizeof(data)) == 0)
        sendPacked(PDM_DBC_PDM_CURRENTS_FRAME_ID, PDM_DBC_PDM_CURRENTS_LENGTH, data);
}

void CANMessages_SendSensors()
{
    struct pdm_dbc_pdm_sensors_t m;
    pdm_dbc_pdm_sensors_init(&m);
    m.temperature1 = Temperature_GetDeciCelsius(TemperatureSensorId::Sensor1);
    m.temperature2 = Temperature_GetDeciCelsius(TemperatureSensorId::Sensor2);
    m.temperature3 = Temperature_GetDeciCelsius(TemperatureSensorId::Sensor3);
    m.supply_voltage = SupplyVoltage_GetFilteredMillivolts();
    uint8_t data[PDM_DBC_PDM_SENSORS_LENGTH] = {};
    if (pdm_dbc_pdm_sensors_pack(data, &m, sizeof(data)) == 0)
        sendPacked(PDM_DBC_PDM_SENSORS_FRAME_ID, PDM_DBC_PDM_SENSORS_LENGTH, data);
}

void CANMessages_SendFaults()
{
    struct pdm_dbc_pdm_faults_t m;
    pdm_dbc_pdm_faults_init(&m);
    m.e_fuse1_faults = EFuse_GetLatchedFaults(EFuseId::Channel1) | EFuse_GetActiveFaults(EFuseId::Channel1);
    m.e_fuse2_faults = EFuse_GetLatchedFaults(EFuseId::Channel2) | EFuse_GetActiveFaults(EFuseId::Channel2);
    m.system_faults = FaultManager_GetActiveFaults() | FaultManager_GetLatchedFaults();
    m.ntc_validity = Temperature_GetValidityMask();
    m.fault_counter = FaultManager_GetSequenceCounter();
    uint8_t data[PDM_DBC_PDM_FAULTS_LENGTH] = {};
    if (pdm_dbc_pdm_faults_pack(data, &m, sizeof(data)) == 0)
        sendPacked(PDM_DBC_PDM_FAULTS_FRAME_ID, PDM_DBC_PDM_FAULTS_LENGTH, data);
}

void CANMessages_SendDiagnostics()
{
    struct pdm_dbc_pdm_diagnostics_t m;
    pdm_dbc_pdm_diagnostics_init(&m);
    m.can_rx_count = static_cast<uint16_t>(CAN_GetReceiveCount());
    m.can_tx_count = static_cast<uint16_t>(CAN_GetTransmitCount());
    m.can_dropped = CAN_GetDroppedFrameCount();
    m.reset_cause = ResetMonitor_GetRawMCUSR();
    m.scheduler_overruns = static_cast<uint8_t>(Scheduler_GetOverrunCount());
    uint8_t data[PDM_DBC_PDM_DIAGNOSTICS_LENGTH] = {};
    if (pdm_dbc_pdm_diagnostics_pack(data, &m, sizeof(data)) == 0)
        sendPacked(PDM_DBC_PDM_DIAGNOSTICS_FRAME_ID, PDM_DBC_PDM_DIAGNOSTICS_LENGTH, data);
}

const PDMCommand &CANMessages_GetCommand() { return command; }
bool CANMessages_HasValidCommand() { return command.valid && !CANMessages_HasTimedOut(); }
bool CANMessages_HasTimedOut() { return command.lastValidMessageMs == 0U || millis() - command.lastValidMessageMs > Settings::CAN_COMMAND_TIMEOUT_MS; }
uint32_t CANMessages_GetLastValidCommandTimeMs() { return command.lastValidMessageMs; }
uint8_t CANMessages_GetLastCommandCounter() { return command.rollingCounter; }
void CANMessages_InvalidateCommand() { command.valid = false; }
