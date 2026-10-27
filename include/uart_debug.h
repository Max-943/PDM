#ifndef UART_DEBUG_H
#define UART_DEBUG_H

#include <Arduino.h>

void UARTDebug_Init();
void UARTDebug_Task1s();
void UARTDebug_PrintStartupBanner();
void UARTDebug_PrintSummary();
void UARTDebug_ProcessInput();
void UARTDebug_Print(const char *text);
void UARTDebug_Println(const char *text);
void UARTDebug_PrintHex16(const char *label, uint16_t value);
void UARTDebug_PrintValue(const char *label, int32_t value, const char *unit);
bool UARTDebug_IsEnabled();

#if !defined(PDM_DEBUG_ENABLED)
#define PDM_DEBUG_ENABLED 0
#endif

#endif
