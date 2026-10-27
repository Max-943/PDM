#ifndef OUTPUTS_H
#define OUTPUTS_H

#include <Arduino.h>

enum class OutputId : uint8_t
{
    OutputPC0 = 0,
    OutputPC1 = 1,
    Count = 2
};

enum class OutputInhibitReason : uint16_t
{
    None             = 0x0000,
    EmergencyOff     = 0x0001,
    CANTimeout       = 0x0002,
    SystemFault      = 0x0004,
    Overtemperature  = 0x0008,
    Undervoltage     = 0x0010,
    Overvoltage      = 0x0020
};

struct OutputStatus
{
    bool requestedOn;
    bool enabled;
    bool permitted;
    uint16_t inhibitReasons;
};

void Outputs_InitSafe();
void Outputs_Task10ms();
void Outputs_Request(OutputId id, bool enabled);
void Outputs_SetSystemPermission(bool permitted, OutputInhibitReason reason);
void Outputs_ForceOff(OutputId id, OutputInhibitReason reason);
void Outputs_ForceAllOff(OutputInhibitReason reason);
void Outputs_ClearInhibit(OutputInhibitReason reason);
bool Outputs_IsRequested(OutputId id);
bool Outputs_GetState(OutputId id);
uint16_t Outputs_GetInhibitReasons(OutputId id);
OutputStatus Outputs_GetStatus(OutputId id);

inline OutputInhibitReason operator|(OutputInhibitReason a, OutputInhibitReason b)
{
    return static_cast<OutputInhibitReason>(
        static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

#endif
