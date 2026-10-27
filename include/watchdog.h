#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <Arduino.h>

void Watchdog_Init();
void Watchdog_Enable();
void Watchdog_Disable();
void Watchdog_MarkMainLoopAlive();
void Watchdog_MarkTask1msAlive();
void Watchdog_MarkTask10msAlive();
void Watchdog_MarkTask100msAlive();
void Watchdog_ServiceIfHealthy();
bool Watchdog_IsSoftwareHealthy();
uint32_t Watchdog_GetServiceCount();

#endif
