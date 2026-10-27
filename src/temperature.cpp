#include "temperature.h"
#include "adc_driver.h"
#include "board_config.h"
#include "pdm_settings.h"
#include <math.h>

namespace
{
    TemperatureSensorStatus sensors[3] = {};
    uint8_t channel(uint8_t i){const uint8_t c[3]={Board::ADC_NTC1,Board::ADC_NTC2,Board::ADC_NTC3};return c[i];}
    int16_t offset(uint8_t i){const int16_t o[3]={Settings::NTC1_OFFSET_DC,Settings::NTC2_OFFSET_DC,Settings::NTC3_OFFSET_DC};return o[i];}
    int16_t convert(uint32_t r)
    {
        const float t0=298.15f;
        float inv=1.0f/t0 + logf(static_cast<float>(r)/Settings::NTC_R25_OHM)/Settings::NTC_BETA_K;
        return static_cast<int16_t>((1.0f/inv-273.15f)*10.0f);
    }
}
void Temperature_Init(){for(auto &s:sensors){s={0,0,0,false,TemperatureSensorFault::Implausible};}}
void Temperature_Task100ms()
{
    for(uint8_t i=0;i<3;++i)
    {
        auto &s=sensors[i]; s.rawADC=ADC_ReadAverage(channel(i),Settings::SLOW_ADC_SAMPLES);
        if(s.rawADC<=Settings::ADC_SHORT_THRESHOLD){s.valid=false;s.fault=TemperatureSensorFault::ShortToGround;continue;}
        if(s.rawADC>=Settings::ADC_OPEN_THRESHOLD){s.valid=false;s.fault=TemperatureSensorFault::OpenCircuit;continue;}
        s.resistanceOhm=static_cast<uint32_t>(Settings::NTC_PULLUP_OHM)*s.rawADC/(Settings::ADC_MAX_COUNT-s.rawADC);
        int16_t v=convert(s.resistanceOhm)+offset(i);
        if(!s.valid) s.temperatureDeciC=v; else s.temperatureDeciC=static_cast<int16_t>(s.temperatureDeciC+(v-s.temperatureDeciC)/(1<<Settings::TEMPERATURE_FILTER_SHIFT));
        s.valid=true;s.fault=TemperatureSensorFault::None;
    }
}
static TemperatureSensorStatus& ts(TemperatureSensorId id){return sensors[static_cast<uint8_t>(id)];}
int16_t Temperature_GetDeciCelsius(TemperatureSensorId id){return ts(id).temperatureDeciC;}
int16_t Temperature_GetMaximumDeciCelsius(){int16_t m=-32768;for(auto&s:sensors)if(s.valid&&s.temperatureDeciC>m)m=s.temperatureDeciC;return m;}
uint32_t Temperature_GetResistanceOhm(TemperatureSensorId id){return ts(id).resistanceOhm;}
uint16_t Temperature_GetRawADC(TemperatureSensorId id){return ts(id).rawADC;}
bool Temperature_IsValid(TemperatureSensorId id){return ts(id).valid;}
bool Temperature_AreAllSensorsValid(){return sensors[0].valid&&sensors[1].valid&&sensors[2].valid;}
bool Temperature_HasWarning(){return Temperature_AreAllSensorsValid()&&Temperature_GetMaximumDeciCelsius()>=Settings::TEMP_WARNING_DC;}
bool Temperature_RequiresShutdown(){return !Temperature_AreAllSensorsValid()||Temperature_GetMaximumDeciCelsius()>=Settings::TEMP_SHUTDOWN_DC;}
uint8_t Temperature_GetValidityMask(){uint8_t m=0;for(uint8_t i=0;i<3;++i)if(sensors[i].valid)m|=_BV(i);return m;}
TemperatureSensorFault Temperature_GetFault(TemperatureSensorId id){return ts(id).fault;}
TemperatureSensorStatus Temperature_GetStatus(TemperatureSensorId id){return ts(id);}
