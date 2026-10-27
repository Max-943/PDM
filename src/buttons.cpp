#include "buttons.h"
#include "board_config.h"
#include "pdm_settings.h"
#include "leds.h"
#include <avr/io.h>

namespace
{
    struct Internal { ButtonStatus s; uint8_t stable; bool longSent; };
    Internal b[2] = {};
    bool raw(uint8_t i)
    {
        uint8_t bit = i==0 ? Board::BUTTON1_PC2_BIT : Board::BUTTON2_PC3_BIT;
        bool high = (PINC & _BV(bit)) != 0;
        return Board::BUTTONS_ACTIVE_LOW ? !high : high;
    }
}
void Buttons_Init()
{
    DDRC &= static_cast<uint8_t>(~(_BV(Board::BUTTON1_PC2_BIT)|_BV(Board::BUTTON2_PC3_BIT)));
    PORTC &= static_cast<uint8_t>(~(_BV(Board::BUTTON1_PC2_BIT)|_BV(Board::BUTTON2_PC3_BIT))); // external pull-ups
    b[0]={}; b[1]={};
}
void Buttons_Task10ms()
{
    for(uint8_t i=0;i<2;++i)
    {
        Internal &x=b[i]; bool r=raw(i); x.s.rawPressed=r;
        if(r==x.s.debouncedPressed) x.stable=0;
        else if(++x.stable>=Settings::BUTTON_DEBOUNCE_COUNT)
        {
            x.stable=0; x.s.debouncedPressed=r;
            if(r){x.s.pressedEvent=true; x.s.heldTimeMs=0; x.longSent=false; LEDs_NotifyButtonActivity();}
            else{x.s.releasedEvent=true; x.s.heldTimeMs=0; x.longSent=false;}
        }
        if(x.s.debouncedPressed)
        {
            if(x.s.heldTimeMs <= 60000-Settings::BUTTON_SAMPLE_PERIOD_MS) x.s.heldTimeMs += Settings::BUTTON_SAMPLE_PERIOD_MS;
            if(!x.longSent && x.s.heldTimeMs>=Settings::BUTTON_LONG_PRESS_MS){x.s.longPressEvent=true;x.longSent=true;}
        }
    }
}
static Internal& get(ButtonId id){return b[static_cast<uint8_t>(id)];}
bool Buttons_IsPressed(ButtonId id){return get(id).s.debouncedPressed;}
bool Buttons_GetPressedEvent(ButtonId id){bool v=get(id).s.pressedEvent;get(id).s.pressedEvent=false;return v;}
bool Buttons_GetReleasedEvent(ButtonId id){bool v=get(id).s.releasedEvent;get(id).s.releasedEvent=false;return v;}
bool Buttons_GetLongPressEvent(ButtonId id){bool v=get(id).s.longPressEvent;get(id).s.longPressEvent=false;return v;}
uint16_t Buttons_GetHeldTimeMs(ButtonId id){return get(id).s.heldTimeMs;}
ButtonStatus Buttons_GetStatus(ButtonId id){return get(id).s;}
void Buttons_ClearEvents(ButtonId id){get(id).s.pressedEvent=get(id).s.releasedEvent=get(id).s.longPressEvent=false;}
bool Buttons_AnyPressed(){return b[0].s.debouncedPressed||b[1].s.debouncedPressed;}
