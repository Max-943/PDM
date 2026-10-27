#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

enum class LEDId : uint8_t
{
    SystemFault = 0,
    EFuse1Fault,
    EFuse2Fault,
    CANStatus,
    ButtonStatus,
    Count
};

enum class LEDPattern : uint8_t
{
    Off,
    Solid,
    SlowFlash,
    FastFlash,
    Pulse
};

void LEDs_Init();
void LEDs_Task10ms();
void LEDs_Task100ms();
void LEDs_SetPattern(LEDId id, LEDPattern pattern);
void LEDs_SetSolid(LEDId id, bool on);
void LEDs_Pulse(LEDId id, uint16_t durationMs);
void LEDs_NotifyCANActivity();
void LEDs_NotifyButtonActivity();
LEDPattern LEDs_GetPattern(LEDId id);

#endif
