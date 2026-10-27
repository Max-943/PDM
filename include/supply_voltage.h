#ifndef SUPPLY_VOLTAGE_H
#define SUPPLY_VOLTAGE_H

#include <Arduino.h>

enum class SupplyVoltageState : uint8_t
{
    Unknown,
    Normal,
    Undervoltage,
    Overvoltage,
    SensorFault
};

void SupplyVoltage_Init();
void SupplyVoltage_Task10ms();
void SupplyVoltage_Task100ms();
uint16_t SupplyVoltage_GetRawADC();
uint16_t SupplyVoltage_GetMillivolts();
uint16_t SupplyVoltage_GetFilteredMillivolts();
SupplyVoltageState SupplyVoltage_GetState();
bool SupplyVoltage_IsValid();
bool SupplyVoltage_IsUndervoltage();
bool SupplyVoltage_IsOvervoltage();
bool SupplyVoltage_AllowsOutputs();

#endif
