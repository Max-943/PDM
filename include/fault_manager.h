#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include <Arduino.h>

enum class SystemFault : uint16_t
{
    None                 = 0x0000,
    CANInitialization    = 0x0001,
    CANTimeout           = 0x0002,
    CANBusOff            = 0x0004,
    SupplyUndervoltage   = 0x0008,
    SupplyOvervoltage    = 0x0010,
    TemperatureSensor    = 0x0020,
    Overtemperature      = 0x0040,
    ADCFault             = 0x0080,
    SchedulerOverrun     = 0x0100,
    WatchdogReset        = 0x0200,
    ConfigurationInvalid = 0x0400,
    InternalError        = 0x0800,
    EmergencyOff         = 0x1000
};

struct FaultManagerStatus
{
    uint16_t activeSystemFaults;
    uint16_t latchedSystemFaults;
    uint16_t historicalSystemFaults;
    uint8_t faultSequenceCounter;
    uint16_t totalFaultCount;
};

void FaultManager_Init();
void FaultManager_Task10ms();
void FaultManager_Task100ms();
void FaultManager_Task1s();
void FaultManager_SetActive(SystemFault fault);
void FaultManager_ClearActive(SystemFault fault);
void FaultManager_Latch(SystemFault fault);
bool FaultManager_ClearLatched(SystemFault fault);
void FaultManager_ClearAllNonCritical();
bool FaultManager_IsActive(SystemFault fault);
bool FaultManager_IsLatched(SystemFault fault);
bool FaultManager_HasAnyFault();
bool FaultManager_HasCriticalFault();
uint16_t FaultManager_GetActiveFaults();
uint16_t FaultManager_GetLatchedFaults();
uint16_t FaultManager_GetHistoricalFaults();
uint8_t FaultManager_GetSequenceCounter();
uint16_t FaultManager_GetTotalFaultCount();
FaultManagerStatus FaultManager_GetStatus();

inline SystemFault operator|(SystemFault a, SystemFault b)
{
    return static_cast<SystemFault>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

#endif
