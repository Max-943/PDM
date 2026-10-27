#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

enum class ButtonId : uint8_t
{
    Button1 = 0,
    Button2 = 1,
    Count = 2
};

struct ButtonStatus
{
    bool rawPressed;
    bool debouncedPressed;
    bool pressedEvent;
    bool releasedEvent;
    bool longPressEvent;
    uint16_t heldTimeMs;
};

void Buttons_Init();
void Buttons_Task10ms();
bool Buttons_IsPressed(ButtonId id);
bool Buttons_GetPressedEvent(ButtonId id);
bool Buttons_GetReleasedEvent(ButtonId id);
bool Buttons_GetLongPressEvent(ButtonId id);
uint16_t Buttons_GetHeldTimeMs(ButtonId id);
ButtonStatus Buttons_GetStatus(ButtonId id);
void Buttons_ClearEvents(ButtonId id);
bool Buttons_AnyPressed();

#endif
