#include "hardware_test.h"
#include "test_config.h"
#include "adc_driver.h"
#include "board_config.h"
#include "buttons.h"
#include "can_driver.h"
#include "can_messages.h"
#include "efuse.h"
#include "fault_manager.h"
#include "leds.h"
#include "outputs.h"
#include "pdm_control.h"
#include "scheduler.h"
#include "supply_voltage.h"
#include "temperature.h"
#include <avr/io.h>

namespace
{
    bool active = TEST_MODE_ENABLED != 0;
    #if TEST_MODE_ENABLED

    bool continuous = false;
    uint32_t lastReport = 0;

    #endif

    void printBool(bool value) { Serial.print(value ? "YES" : "NO"); }
    void printFeature(const __FlashStringHelper *name, bool enabled)
    {
        Serial.print(name); Serial.println(enabled ? F(" ENABLED") : F(" DISABLED"));
    }

    void printEFuse(EFuseId id, const __FlashStringHelper *name)
    {
        EFuseStatus s = EFuse_GetStatus(id);
        Serial.print(name); Serial.print(F(": state=")); Serial.print(static_cast<uint8_t>(s.state));
        Serial.print(F(" req=")); Serial.print(s.requestedOn);
        Serial.print(F(" enabled=")); Serial.print(s.outputEnabled);
        Serial.print(F(" current_mA=")); Serial.print(s.filteredCurrentMilliamp);
        Serial.print(F(" peak_mA=")); Serial.print(s.peakCurrentMilliamp);
        Serial.print(F(" FLT=")); Serial.print(s.hardwareFaultActive);
        Serial.print(F(" active=0x")); Serial.print(s.activeFaults, HEX);
        Serial.print(F(" latched=0x")); Serial.println(s.latchedFaults, HEX);
    }
}

void HardwareTest_Init()
{
#if TEST_MODE_ENABLED
    Serial.println(F("*** BENCH TEST MODE ACTIVE - USE CURRENT LIMITED SUPPLY ***"));
    HardwareTest_PrintHelp();
#endif
}

bool HardwareTest_IsActive() { return active; }

void HardwareTest_PrintHelp()
{
    Serial.println(F("Commands:"));
    Serial.println(F("  ?/help        help"));
    Serial.println(F("  report        full live report"));
    Serial.println(F("  stream on/off periodic report"));
    Serial.println(F("  ef1 on/off    eFuse1 request"));
    Serial.println(F("  ef2 on/off    eFuse2 request"));
    Serial.println(F("  out0 on/off   PC0 output"));
    Serial.println(F("  out1 on/off   PC1 output"));
    Serial.println(F("  all off       immediate safe shutdown"));
    Serial.println(F("  clear         clear noncritical/eFuse faults"));
    Serial.println(F("  leds          cycle all LEDs"));
    Serial.println(F("  can status    send all telemetry now"));
    Serial.println(F("  adc           print all ADC raw values"));
}

void HardwareTest_PrintFullReport()
{
    Serial.println(F("\n========== PDM BENCH REPORT =========="));
    Serial.print(F("test_mode=")); printBool(active); Serial.println();
    printFeature(F("CAN"), FEATURE_CAN_ENABLED);
    printFeature(F("EFUSE"), FEATURE_EFUSE_ENABLED);
    printFeature(F("TEMPERATURE"), FEATURE_TEMPERATURE_ENABLED);
    printFeature(F("SUPPLY"), FEATURE_SUPPLY_VOLTAGE_ENABLED);
    printFeature(F("BUTTONS"), FEATURE_BUTTONS_ENABLED);
    printFeature(F("OUTPUTS"), FEATURE_OUTPUTS_ENABLED);
    printFeature(F("LEDS"), FEATURE_LEDS_ENABLED);
    printFeature(F("WATCHDOG"), FEATURE_WATCHDOG_ENABLED);

    Serial.print(F("ADC IMON1/IMON2/NTC1/NTC2/NTC3/VIN: "));
    Serial.print(ADC_ReadRaw(Board::ADC_IMON1)); Serial.print('/');
    Serial.print(ADC_ReadRaw(Board::ADC_IMON2)); Serial.print('/');
    Serial.print(ADC_ReadRaw(Board::ADC_NTC1)); Serial.print('/');
    Serial.print(ADC_ReadRaw(Board::ADC_NTC2)); Serial.print('/');
    Serial.print(ADC_ReadRaw(Board::ADC_NTC3)); Serial.print('/');
    Serial.println(ADC_ReadRaw(Board::ADC_VIN));

    printEFuse(EFuseId::Channel1, F("EFUSE1"));
    printEFuse(EFuseId::Channel2, F("EFUSE2"));
    for (uint8_t i = 0; i < 3; ++i)
    {
        TemperatureSensorId id = static_cast<TemperatureSensorId>(i);
        Serial.print(F("NTC")); Serial.print(i + 1); Serial.print(F(": raw=")); Serial.print(Temperature_GetRawADC(id));
        Serial.print(F(" R_ohm=")); Serial.print(Temperature_GetResistanceOhm(id));
        Serial.print(F(" temp_dC=")); Serial.print(Temperature_GetDeciCelsius(id));
        Serial.print(F(" valid=")); Serial.println(Temperature_IsValid(id));
    }
    Serial.print(F("VIN raw=")); Serial.print(SupplyVoltage_GetRawADC());
    Serial.print(F(" measured_mV=")); Serial.print(SupplyVoltage_GetMillivolts());
    Serial.print(F(" filtered_mV=")); Serial.print(SupplyVoltage_GetFilteredMillivolts());
    Serial.print(F(" state=")); Serial.println(static_cast<uint8_t>(SupplyVoltage_GetState()));
    Serial.print(F("Buttons B1/B2=")); Serial.print(Buttons_IsPressed(ButtonId::Button1)); Serial.print('/'); Serial.println(Buttons_IsPressed(ButtonId::Button2));
    Serial.print(F("Outputs PC0/PC1=")); Serial.print(Outputs_GetState(OutputId::OutputPC0)); Serial.print('/'); Serial.println(Outputs_GetState(OutputId::OutputPC1));
    Serial.print(F("CAN state=")); Serial.print(static_cast<uint8_t>(CAN_GetState()));
    Serial.print(F(" RX=")); Serial.print(CAN_GetReceiveCount()); Serial.print(F(" TX=")); Serial.print(CAN_GetTransmitCount());
    Serial.print(F(" drop=")); Serial.print(CAN_GetDroppedFrameCount()); Serial.print(F(" queue=")); Serial.println(CAN_GetQueueCount());
    Serial.print(F("PDM state=")); Serial.print(static_cast<uint8_t>(PDMControl_GetState()));
    Serial.print(F(" systemFaults=0x")); Serial.println(FaultManager_GetActiveFaults() | FaultManager_GetLatchedFaults(), HEX);
    Serial.print(F("Scheduler overruns=")); Serial.println(Scheduler_GetOverrunCount());
    Serial.println(F("======================================"));
}

void HardwareTest_ProcessSerial()
{
#if TEST_MODE_ENABLED
    if (!Serial.available()) return;
    String line = Serial.readStringUntil('\n'); line.trim(); line.toLowerCase();
    if (line == "?" || line == "help") HardwareTest_PrintHelp();
    else if (line == "report") HardwareTest_PrintFullReport();
    else if (line == "stream on") { continuous = true; Serial.println(F("stream ON")); }
    else if (line == "stream off") { continuous = false; Serial.println(F("stream OFF")); }
    else if (line == "ef1 on") { EFuse_RequestState(EFuseId::Channel1, true); EFuse_SetSystemPermission(EFuseId::Channel1, true); Serial.println(F("eFuse1 test request ON")); }
    else if (line == "ef1 off") { EFuse_RequestState(EFuseId::Channel1, false); Serial.println(F("eFuse1 OFF")); }
    else if (line == "ef2 on") { EFuse_RequestState(EFuseId::Channel2, true); EFuse_SetSystemPermission(EFuseId::Channel2, true); Serial.println(F("eFuse2 test request ON")); }
    else if (line == "ef2 off") { EFuse_RequestState(EFuseId::Channel2, false); Serial.println(F("eFuse2 OFF")); }
    else if (line == "out0 on") { Outputs_ClearInhibit(OutputInhibitReason::SystemFault); Outputs_Request(OutputId::OutputPC0, true); Serial.println(F("PC0 request ON")); }
    else if (line == "out0 off") { Outputs_Request(OutputId::OutputPC0, false); Serial.println(F("PC0 OFF")); }
    else if (line == "out1 on") { Outputs_ClearInhibit(OutputInhibitReason::SystemFault); Outputs_Request(OutputId::OutputPC1, true); Serial.println(F("PC1 request ON")); }
    else if (line == "out1 off") { Outputs_Request(OutputId::OutputPC1, false); Serial.println(F("PC1 OFF")); }
    else if (line == "all off") { EFuse_RequestState(EFuseId::Channel1, false); EFuse_RequestState(EFuseId::Channel2, false); Outputs_ForceAllOff(OutputInhibitReason::EmergencyOff); Serial.println(F("ALL OUTPUTS OFF")); }
    else if (line == "clear") { EFuse_ClearFault(EFuseId::Channel1); EFuse_ClearFault(EFuseId::Channel2); FaultManager_ClearAllNonCritical(); Outputs_ClearInhibit(OutputInhibitReason::EmergencyOff); Serial.println(F("fault clear requested")); }
    else if (line == "leds") { for (uint8_t i=0;i<5;++i) LEDs_Pulse(static_cast<LEDId>(i), 1000); Serial.println(F("LED pulse test")); }
    else if (line == "can status") { CANMessages_SendStatus(); CANMessages_SendCurrents(); CANMessages_SendSensors(); CANMessages_SendFaults(); CANMessages_SendDiagnostics(); Serial.println(F("CAN telemetry sent")); }
    else if (line == "adc") HardwareTest_PrintFullReport();
    else Serial.println(F("unknown command; type help"));
#endif
}

void HardwareTest_Task100ms()
{
#if TEST_MODE_ENABLED
    if (continuous && millis() - lastReport >= 1000U) { lastReport = millis(); HardwareTest_PrintFullReport(); }
#endif
}
