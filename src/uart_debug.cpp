#include "uart_debug.h"
#include "pdm_settings.h"
#include "firmware_version.h"
#include "reset_monitor.h"
#include "pdm_control.h"
#include "supply_voltage.h"
#include "efuse.h"
#include "temperature.h"
#include "can_driver.h"
#include "fault_manager.h"

void UARTDebug_Init()
{
#if PDM_DEBUG_ENABLED
Serial.begin(Settings::DEBUG_BAUDRATE);
#endif
}
void UARTDebug_Task1s(){UARTDebug_ProcessInput();UARTDebug_PrintSummary();}
void UARTDebug_PrintStartupBanner()
{
#if PDM_DEBUG_ENABLED
Serial.println();Serial.print(FirmwareVersion::NAME);Serial.print(" v");Serial.print(FirmwareVersion::MAJOR);Serial.print('.');Serial.print(FirmwareVersion::MINOR);Serial.print('.');Serial.println(FirmwareVersion::PATCH);Serial.print("Reset: ");Serial.println(ResetMonitor_GetCauseText());
#endif
}
void UARTDebug_PrintSummary()
{
#if PDM_DEBUG_ENABLED
Serial.print("STATE=");Serial.print(static_cast<uint8_t>(PDMControl_GetState()));Serial.print(" VIN_mV=");Serial.print(SupplyVoltage_GetFilteredMillivolts());Serial.print(" I1_mA=");Serial.print(EFuse_GetFilteredCurrentMilliamp(EFuseId::Channel1));Serial.print(" I2_mA=");Serial.print(EFuse_GetFilteredCurrentMilliamp(EFuseId::Channel2));Serial.print(" TMAX_dC=");Serial.print(Temperature_GetMaximumDeciCelsius());Serial.print(" CAN_RX=");Serial.print(CAN_GetReceiveCount());Serial.print(" DROP=");Serial.print(CAN_GetDroppedFrameCount());Serial.print(" FAULT=0x");Serial.println(FaultManager_GetActiveFaults()|FaultManager_GetLatchedFaults(),HEX);
#endif
}
void UARTDebug_ProcessInput()
{
#if PDM_DEBUG_ENABLED
while(Serial.available()){char c=Serial.read();if(c=='s')UARTDebug_PrintSummary();}
#endif
}
void UARTDebug_Print(const char*t)
{
#if PDM_DEBUG_ENABLED
Serial.print(t);
#else
(void)t;
#endif
}
void UARTDebug_Println(const char*t)
{
#if PDM_DEBUG_ENABLED
Serial.println(t);
#else
(void)t;
#endif
}
void UARTDebug_PrintHex16(const char*l,uint16_t v)
{
#if PDM_DEBUG_ENABLED
Serial.print(l);Serial.println(v,HEX);
#else
(void)l;(void)v;
#endif
}
void UARTDebug_PrintValue(const char*l,int32_t v,const char*u)
{
#if PDM_DEBUG_ENABLED
Serial.print(l);Serial.print(v);Serial.println(u);
#else
(void)l;(void)v;(void)u;
#endif
}
bool UARTDebug_IsEnabled(){return PDM_DEBUG_ENABLED!=0;}
