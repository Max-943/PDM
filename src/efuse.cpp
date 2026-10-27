#include "efuse.h"
#include "adc_driver.h"
#include "board_config.h"
#include "pdm_settings.h"
#include <avr/io.h>

namespace
{
    struct Channel
    {
        EFuseStatus s;
        bool permission;
        uint16_t overMs, retryMs, offMs, startMs;
        uint8_t fltActive, fltClear;
    } ch[2];
    uint8_t ix(EFuseId id){return static_cast<uint8_t>(id);}
    void pin(EFuseId id,bool on){uint8_t b=id==EFuseId::Channel1?PB3:PB4;if(on)PORTB|=_BV(b);else PORTB&=static_cast<uint8_t>(~_BV(b));}
    bool flt(EFuseId id){uint8_t b=id==EFuseId::Channel1?PC4:PC7;return (PINC&_BV(b))==0;}
    uint8_t adc(EFuseId id){return id==EFuseId::Channel1?Board::ADC_IMON1:Board::ADC_IMON2;}
    uint16_t runLimit(EFuseId id){return id==EFuseId::Channel1?Settings::EFUSE1_RUN_LIMIT_MA:Settings::EFUSE2_RUN_LIMIT_MA;}
    uint16_t startLimit(EFuseId id){return id==EFuseId::Channel1?Settings::EFUSE1_START_LIMIT_MA:Settings::EFUSE2_START_LIMIT_MA;}
    bool autoRetry(EFuseId id){return id==EFuseId::Channel1?Settings::EFUSE1_AUTO_RETRY:Settings::EFUSE2_AUTO_RETRY;}
    uint16_t convert(EFuseId id,uint16_t raw)
    {
        uint32_t mv=ADC_RawToMillivolts(raw);
        uint32_t ma=mv*1000000000ULL/(Settings::IMON_RESISTOR_OHM*Settings::IMON_GAIN_NA_PER_A);
        uint16_t num=id==EFuseId::Channel1?Settings::EFUSE1_GAIN_NUM:Settings::EFUSE2_GAIN_NUM;
        uint16_t den=id==EFuseId::Channel1?Settings::EFUSE1_GAIN_DEN:Settings::EFUSE2_GAIN_DEN;
        int16_t off=id==EFuseId::Channel1?Settings::EFUSE1_OFFSET_MA:Settings::EFUSE2_OFFSET_MA;
        int32_t corrected=static_cast<int32_t>(ma*num/den)+off;
        if (corrected < 0)
        {
            corrected = 0;
        }

        if (corrected > 65535)
        {
            corrected = 65535;
        }

return static_cast<uint16_t>(corrected);
    }
    void shutdown(EFuseId id,EFuseFault reason)
    {
        Channel&c=ch[ix(id)]; pin(id,false);c.s.outputEnabled=false;c.s.activeFaults|=static_cast<uint16_t>(reason);c.s.latchedFaults|=static_cast<uint16_t>(reason);c.offMs=0;
        if(autoRetry(id)&&c.s.retryCount<Settings::EFUSE_MAX_RETRIES){c.s.state=EFuseState::RetryWait;c.retryMs=0;++c.s.retryCount;}else{c.s.state=EFuseState::FaultLatched;if(c.s.retryCount>=Settings::EFUSE_MAX_RETRIES)c.s.latchedFaults|=static_cast<uint16_t>(EFuseFault::RetryLimitReached);}
    }
    void start(EFuseId id){Channel&c=ch[ix(id)];pin(id,true);c.s.outputEnabled=true;c.s.state=EFuseState::Starting;c.startMs=0;c.overMs=0;}
}
void EFuse_Init()
{
    PORTB&=static_cast<uint8_t>(~(_BV(PB3)|_BV(PB4)));DDRB|=_BV(PB3)|_BV(PB4);
    DDRC&=static_cast<uint8_t>(~(_BV(PC4)|_BV(PC7)));PORTC&=static_cast<uint8_t>(~(_BV(PC4)|_BV(PC7)));
    for(uint8_t i=0;i<2;++i){ch[i]={};ch[i].s.state=EFuseState::Off;ch[i].s.currentValid=true;}
}
void EFuse_Task1ms()
{
    static uint8_t next=0;EFuseId id=static_cast<EFuseId>(next);next^=1;Channel&c=ch[ix(id)];
    uint16_t raw=ADC_ReadAverage(adc(id),Settings::CURRENT_ADC_SAMPLES);uint16_t ma=convert(id,raw);c.s.currentMilliamp=ma;
    if(c.s.filteredCurrentMilliamp==0)c.s.filteredCurrentMilliamp=ma;else c.s.filteredCurrentMilliamp=static_cast<uint16_t>(c.s.filteredCurrentMilliamp+(static_cast<int32_t>(ma)-c.s.filteredCurrentMilliamp)/(1<<Settings::CURRENT_FILTER_SHIFT));
    c.s.currentValid=c.s.filteredCurrentMilliamp<=Settings::CURRENT_PLAUSIBILITY_MAX_MA;
    if(c.s.filteredCurrentMilliamp>c.s.peakCurrentMilliamp)c.s.peakCurrentMilliamp=c.s.filteredCurrentMilliamp;
    if(c.s.state==EFuseState::Starting){c.startMs+=2;if(c.startMs>=Settings::EFUSE_START_WINDOW_MS)c.s.state=EFuseState::Running;}
    uint16_t limit=c.s.state==EFuseState::Starting?startLimit(id):runLimit(id);c.s.activeCurrentLimitMilliamp=limit;
    if(c.s.outputEnabled&&c.s.filteredCurrentMilliamp>limit){if(c.overMs<=65533)c.overMs+=2;}else c.overMs=0;
    if(c.overMs>=Settings::EFUSE_OVERCURRENT_DELAY_MS)shutdown(id,EFuseFault::SoftwareOvercurrent);
}
void EFuse_Task10ms()
{
    for(uint8_t i=0;i<2;++i)
    {
        EFuseId id=static_cast<EFuseId>(i);Channel&c=ch[i];bool f=flt(id);
        if(f){c.fltClear=0;if(c.fltActive<255)++c.fltActive;}else{c.fltActive=0;if(c.fltClear<255)++c.fltClear;}
        if(c.fltActive>=Settings::FLT_ACTIVE_SAMPLES&&!c.s.hardwareFaultActive){c.s.hardwareFaultActive=true;shutdown(id,EFuseFault::HardwareFLT);}
        if(c.fltClear>=Settings::FLT_CLEAR_SAMPLES)c.s.hardwareFaultActive=false;
        if(!c.s.outputEnabled&&c.offMs<65535)c.offMs+=10;
        if(c.s.state==EFuseState::RetryWait){c.retryMs+=10;if(c.retryMs>=Settings::EFUSE_RETRY_DELAY_MS&&c.offMs>=Settings::EFUSE_MINIMUM_OFF_TIME_MS&&!c.s.hardwareFaultActive&&c.s.requestedOn&&c.permission)start(id);}
        else if((c.s.state==EFuseState::Off)&&c.s.requestedOn&&c.permission&&c.offMs>=Settings::EFUSE_MINIMUM_OFF_TIME_MS)start(id);
        else if(c.s.outputEnabled&&(!c.s.requestedOn||!c.permission)){pin(id,false);c.s.outputEnabled=false;c.s.state=EFuseState::Off;c.offMs=0;}
    }
}
void EFuse_Task100ms(){}
void EFuse_RequestState(EFuseId id,bool e){ch[ix(id)].s.requestedOn=e;if(!e){pin(id,false);ch[ix(id)].s.outputEnabled=false;ch[ix(id)].s.state=EFuseState::Off;ch[ix(id)].offMs=0;}}
void EFuse_SetSystemPermission(EFuseId id,bool p){ch[ix(id)].permission=p;}
void EFuse_ForceShutdown(EFuseId id,EFuseFault reason){shutdown(id,reason);}
void EFuse_ForceAllOff(EFuseFault reason){shutdown(EFuseId::Channel1,reason);shutdown(EFuseId::Channel2,reason);}
bool EFuse_ClearFault(EFuseId id){Channel&c=ch[ix(id)];if(c.s.hardwareFaultActive)return false;c.s.activeFaults=0;c.s.latchedFaults=0;c.s.retryCount=0;c.s.state=EFuseState::Off;c.offMs=0;return true;}
void EFuse_ClearPeakCurrent(EFuseId id){ch[ix(id)].s.peakCurrentMilliamp=0;}
bool EFuse_IsEnabled(EFuseId id){return ch[ix(id)].s.outputEnabled;}
bool EFuse_IsRequested(EFuseId id){return ch[ix(id)].s.requestedOn;}
bool EFuse_HardwareFaultActive(EFuseId id){return ch[ix(id)].s.hardwareFaultActive;}
bool EFuse_CurrentIsValid(EFuseId id){return ch[ix(id)].s.currentValid;}
uint16_t EFuse_GetCurrentMilliamp(EFuseId id){return ch[ix(id)].s.currentMilliamp;}
uint16_t EFuse_GetFilteredCurrentMilliamp(EFuseId id){return ch[ix(id)].s.filteredCurrentMilliamp;}
uint16_t EFuse_GetPeakCurrentMilliamp(EFuseId id){return ch[ix(id)].s.peakCurrentMilliamp;}
uint16_t EFuse_GetActiveFaults(EFuseId id){return ch[ix(id)].s.activeFaults;}
uint16_t EFuse_GetLatchedFaults(EFuseId id){return ch[ix(id)].s.latchedFaults;}
EFuseState EFuse_GetState(EFuseId id){return ch[ix(id)].s.state;}
EFuseStatus EFuse_GetStatus(EFuseId id){return ch[ix(id)].s;}
