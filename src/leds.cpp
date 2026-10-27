#include "leds.h"
#include "board_config.h"
#include "pdm_settings.h"
#include <avr/io.h>

namespace
{
    struct Led { LEDPattern pattern; uint16_t pulse; } l[5];
    uint32_t elapsed=0;
    void write(LEDId id,bool on)
    {
        if(!Board::LEDS_ACTIVE_HIGH) on=!on;
        if(id==LEDId::ButtonStatus){if(on)PORTB|=_BV(PB1);else PORTB&=static_cast<uint8_t>(~_BV(PB1));return;}
        uint8_t bit=PD5;
        if(id==LEDId::EFuse1Fault)bit=PD6; else if(id==LEDId::EFuse2Fault)bit=PD7; else if(id==LEDId::CANStatus)bit=PD4;
        if(on)PORTD|=_BV(bit);else PORTD&=static_cast<uint8_t>(~_BV(bit));
    }
}
void LEDs_Init(){DDRB|=_BV(PB1);DDRD|=_BV(PD4)|_BV(PD5)|_BV(PD6)|_BV(PD7);for(uint8_t i=0;i<5;++i){l[i]={LEDPattern::Off,0};write(static_cast<LEDId>(i),false);}}
void LEDs_Task10ms()
{
    elapsed+=10;
    for(uint8_t i=0;i<5;++i)
    {
        LEDId id=static_cast<LEDId>(i);auto& x=l[i];bool on=false;
        switch(x.pattern){case LEDPattern::Solid:on=true;break;case LEDPattern::SlowFlash:on=(elapsed/Settings::LED_SLOW_FLASH_MS)%2==0;break;case LEDPattern::FastFlash:on=(elapsed/Settings::LED_FAST_FLASH_MS)%2==0;break;case LEDPattern::Pulse:on=x.pulse>0;if(x.pulse>=10)x.pulse-=10;else{x.pulse=0;x.pattern=LEDPattern::Off;}break;default:break;}
        write(id,on);
    }
}
void LEDs_Task100ms(){}
void LEDs_SetPattern(LEDId id,LEDPattern p){l[static_cast<uint8_t>(id)].pattern=p;}
void LEDs_SetSolid(LEDId id,bool on){LEDs_SetPattern(id,on?LEDPattern::Solid:LEDPattern::Off);}
void LEDs_Pulse(LEDId id,uint16_t ms){auto&x=l[static_cast<uint8_t>(id)];x.pattern=LEDPattern::Pulse;x.pulse=ms;}
void LEDs_NotifyCANActivity(){LEDs_Pulse(LEDId::CANStatus,Settings::LED_CAN_PULSE_MS);}
void LEDs_NotifyButtonActivity(){LEDs_Pulse(LEDId::ButtonStatus,Settings::LED_CAN_PULSE_MS);}
LEDPattern LEDs_GetPattern(LEDId id){return l[static_cast<uint8_t>(id)].pattern;}
