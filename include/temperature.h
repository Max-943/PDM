#ifndef TEMPERATURE_H
#define TEMPERATURE_H

#include <Arduino.h>

enum class TemperatureSensorId : uint8_t
{
    Sensor1 = 0,
    Sensor2 = 1,
    Sensor3 = 2,
    Count = 3
};

enum class TemperatureSensorFault : uint8_t
{
    None = 0x00,
    ShortToGround = 0x01,
    OpenCircuit = 0x02,
    Implausible = 0x04
};

struct TemperatureSensorStatus
{
    uint16_t rawADC;
    uint32_t resistanceOhm;
    int16_t temperatureDeciC;
    bool valid;
    TemperatureSensorFault fault;
};

void Temperature_Init();
void Temperature_Task100ms();
int16_t Temperature_GetDeciCelsius(TemperatureSensorId id);
int16_t Temperature_GetMaximumDeciCelsius();
uint32_t Temperature_GetResistanceOhm(TemperatureSensorId id);
uint16_t Temperature_GetRawADC(TemperatureSensorId id);
bool Temperature_IsValid(TemperatureSensorId id);
bool Temperature_AreAllSensorsValid();
bool Temperature_HasWarning();
bool Temperature_RequiresShutdown();
uint8_t Temperature_GetValidityMask();
TemperatureSensorFault Temperature_GetFault(TemperatureSensorId id);
TemperatureSensorStatus Temperature_GetStatus(TemperatureSensorId id);

#endif
