#ifndef HARDWARE_TEST_H
#define HARDWARE_TEST_H

#include <Arduino.h>

void HardwareTest_Init();
void HardwareTest_ProcessSerial();
void HardwareTest_Task100ms();
void HardwareTest_PrintHelp();
void HardwareTest_PrintFullReport();
bool HardwareTest_IsActive();

#endif
