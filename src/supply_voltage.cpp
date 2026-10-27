#include "supply_voltage.h"
#include "adc_driver.h"
#include "board_config.h"
#include "pdm_settings.h"

namespace { uint16_t rawADC=0, measured=0, filtered=0; SupplyVoltageState state=SupplyVoltageState::Unknown; }
void SupplyVoltage_Init(){rawADC=measured=filtered=0;state=SupplyVoltageState::Unknown;}
void SupplyVoltage_Task10ms(){}
void SupplyVoltage_Task100ms()
{
    rawADC=ADC_ReadAverage(Board::ADC_VIN,Settings::SLOW_ADC_SAMPLES);
    uint32_t adcMv=ADC_RawToMillivolts(rawADC);
    uint32_t rail=adcMv*(Settings::VIN_TOP_RESISTOR_OHM+Settings::VIN_BOTTOM_RESISTOR_OHM)/Settings::VIN_BOTTOM_RESISTOR_OHM;
    rail=rail*Settings::VIN_CAL_NUM/Settings::VIN_CAL_DEN;
    if (rail > 65535UL)
    {
        rail = 65535UL;
    }

    measured = static_cast<uint16_t>(rail);
    if(filtered==0) filtered=measured; else filtered=static_cast<uint16_t>(filtered + (static_cast<int32_t>(measured)-filtered)/(1<<Settings::VIN_FILTER_SHIFT));
    if(!ADC_IsRawPlausible(rawADC)) state=SupplyVoltageState::SensorFault;
    else if(filtered>=Settings::VIN_OVERVOLTAGE_MV) state=SupplyVoltageState::Overvoltage;
    else if(filtered<=Settings::VIN_UNDERVOLTAGE_MV) state=SupplyVoltageState::Undervoltage;
    else if(state==SupplyVoltageState::Undervoltage && filtered<Settings::VIN_RECOVERY_MV) {}
    else state=SupplyVoltageState::Normal;
}
uint16_t SupplyVoltage_GetRawADC(){return rawADC;}
uint16_t SupplyVoltage_GetMillivolts(){return measured;}
uint16_t SupplyVoltage_GetFilteredMillivolts(){return filtered;}
SupplyVoltageState SupplyVoltage_GetState(){return state;}
bool SupplyVoltage_IsValid(){return state!=SupplyVoltageState::Unknown&&state!=SupplyVoltageState::SensorFault;}
bool SupplyVoltage_IsUndervoltage(){return state==SupplyVoltageState::Undervoltage;}
bool SupplyVoltage_IsOvervoltage(){return state==SupplyVoltageState::Overvoltage;}
bool SupplyVoltage_AllowsOutputs(){return state==SupplyVoltageState::Normal;}
