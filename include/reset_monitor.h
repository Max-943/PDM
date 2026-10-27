#ifndef RESET_MONITOR_H
#define RESET_MONITOR_H

#include <Arduino.h>

enum class ResetCause : uint8_t
{
    None        = 0x00,
    PowerOn     = 0x01,
    External    = 0x02,
    BrownOut    = 0x04,
    Watchdog    = 0x08,
    JTAG        = 0x10,
    Unknown     = 0x80
};

void ResetMonitor_CaptureEarly();
void ResetMonitor_Init();
void ResetMonitor_Task1s();
ResetCause ResetMonitor_GetCause();
uint8_t ResetMonitor_GetRawMCUSR();
bool ResetMonitor_WasWatchdogReset();
bool ResetMonitor_WasBrownOutReset();
const char *ResetMonitor_GetCauseText();

#endif
