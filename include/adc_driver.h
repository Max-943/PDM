#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <Arduino.h>

/* The only module permitted to access AVR ADC registers. */
void ADC_Init();
uint16_t ADC_ReadRaw(uint8_t channel);
uint16_t ADC_ReadAverage(uint8_t channel, uint8_t sampleCount);
uint16_t ADC_RawToMillivolts(uint16_t rawValue);
bool ADC_IsRawPlausible(uint16_t rawValue);

#endif
