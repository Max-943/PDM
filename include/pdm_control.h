#ifndef PDM_CONTROL_H
#define PDM_CONTROL_H

#include <Arduino.h>

enum class PDMState : uint8_t
{
    Startup,
    Standby,
    Operational,
    CANTimeout,
    Undervoltage,
    Overvoltage,
    Overtemperature,
    EmergencyOff,
    CriticalFault
};

struct PDMControlStatus
{
    PDMState state;
    bool canCommandValid;
    bool emergencyOffActive;
    bool voltagePermission;
    bool temperaturePermission;
    bool systemPermission;
    uint8_t aliveCounter;
    uint32_t runtimeSeconds;
};

void PDMControl_Init();
void PDMControl_Task10ms();
void PDMControl_Task100ms();
void PDMControl_Task1s();
void PDMControl_EmergencyShutdown();
bool PDMControl_RequestFaultReset();
PDMState PDMControl_GetState();
bool PDMControl_AllowsOutputs();
bool PDMControl_IsEmergencyOffActive();
uint8_t PDMControl_GetAliveCounter();
uint32_t PDMControl_GetRuntimeSeconds();
PDMControlStatus PDMControl_GetStatus();

#endif
