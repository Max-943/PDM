#ifndef EFUSE_H
#define EFUSE_H

#include <Arduino.h>

enum class EFuseId : uint8_t
{
    Channel1 = 0,
    Channel2 = 1,
    Count = 2
};

enum class EFuseState : uint8_t
{
    Off,
    Starting,
    Running,
    FaultLatched,
    RetryWait,
    TemperatureShutdown,
    CANTimeoutShutdown,
    VoltageShutdown,
    EmergencyShutdown
};

enum class EFuseFault : uint16_t
{
    None                 = 0x0000,
    HardwareFLT          = 0x0001,
    SoftwareOvercurrent  = 0x0002,
    RetryLimitReached    = 0x0004,
    Overtemperature      = 0x0008,
    CANTimeout           = 0x0010,
    SupplyUndervoltage   = 0x0020,
    SupplyOvervoltage    = 0x0040,
    CurrentSensorInvalid = 0x0080,
    EmergencyOff         = 0x0100
};

struct EFuseStatus
{
    bool requestedOn;
    bool outputEnabled;
    bool hardwareFaultActive;
    bool currentValid;
    EFuseState state;
    uint16_t activeFaults;
    uint16_t latchedFaults;
    uint16_t currentMilliamp;
    uint16_t filteredCurrentMilliamp;
    uint16_t peakCurrentMilliamp;
    uint16_t activeCurrentLimitMilliamp;
    uint8_t retryCount;
};

void EFuse_Init();
void EFuse_Task1ms();
void EFuse_Task10ms();
void EFuse_Task100ms();
void EFuse_RequestState(EFuseId id, bool enabled);
void EFuse_SetSystemPermission(EFuseId id, bool permitted);
void EFuse_ForceShutdown(EFuseId id, EFuseFault reason);
void EFuse_ForceAllOff(EFuseFault reason);
bool EFuse_ClearFault(EFuseId id);
void EFuse_ClearPeakCurrent(EFuseId id);
bool EFuse_IsEnabled(EFuseId id);
bool EFuse_IsRequested(EFuseId id);
bool EFuse_HardwareFaultActive(EFuseId id);
bool EFuse_CurrentIsValid(EFuseId id);
uint16_t EFuse_GetCurrentMilliamp(EFuseId id);
uint16_t EFuse_GetFilteredCurrentMilliamp(EFuseId id);
uint16_t EFuse_GetPeakCurrentMilliamp(EFuseId id);
uint16_t EFuse_GetActiveFaults(EFuseId id);
uint16_t EFuse_GetLatchedFaults(EFuseId id);
EFuseState EFuse_GetState(EFuseId id);
EFuseStatus EFuse_GetStatus(EFuseId id);

inline EFuseFault operator|(EFuseFault a, EFuseFault b)
{
    return static_cast<EFuseFault>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

#endif
