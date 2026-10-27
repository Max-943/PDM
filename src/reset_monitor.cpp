#include "reset_monitor.h"
#include <avr/io.h>
#include <avr/wdt.h>

namespace { uint8_t raw=0; ResetCause cause=ResetCause::None; }
void ResetMonitor_CaptureEarly(){raw=MCUSR;MCUSR=0;wdt_disable();}
void ResetMonitor_Init(){if(raw&_BV(WDRF))cause=ResetCause::Watchdog;else if(raw&_BV(BORF))cause=ResetCause::BrownOut;else if(raw&_BV(EXTRF))cause=ResetCause::External;else if(raw&_BV(PORF))cause=ResetCause::PowerOn;else if(raw&_BV(JTRF))cause=ResetCause::JTAG;else cause=raw?ResetCause::Unknown:ResetCause::None;}
void ResetMonitor_Task1s(){}
ResetCause ResetMonitor_GetCause(){return cause;}
uint8_t ResetMonitor_GetRawMCUSR(){return raw;}
bool ResetMonitor_WasWatchdogReset(){return cause==ResetCause::Watchdog;}
bool ResetMonitor_WasBrownOutReset(){return cause==ResetCause::BrownOut;}
const char*ResetMonitor_GetCauseText(){switch(cause){case ResetCause::PowerOn:return"Power-on";case ResetCause::External:return"External";case ResetCause::BrownOut:return"Brown-out";case ResetCause::Watchdog:return"Watchdog";case ResetCause::JTAG:return"JTAG";case ResetCause::None:return"None";default:return"Unknown";}}
