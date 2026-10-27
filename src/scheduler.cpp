#include "scheduler.h"
#include "pdm_settings.h"
#include "efuse.h"
#include "buttons.h"
#include "supply_voltage.h"
#include "fault_manager.h"
#include "pdm_control.h"
#include "outputs.h"
#include "leds.h"
#include "temperature.h"
#include "can_messages.h"
#include "uart_debug.h"
#include "reset_monitor.h"
#include "watchdog.h"
#include "hardware_test.h"
#include "test_config.h"

namespace { SchedulerStatistics st={}; uint32_t p1,p10,p100,p1000,last1,last10,last100; bool healthy=true; void update(uint16_t&maxv,uint32_t start,uint16_t limit){uint32_t d=micros()-start;if(d>maxv)maxv=d>65535?65535:d;if(d>limit){++st.overrunCount;healthy=false;}} }
void Scheduler_Init(){uint32_t n=millis();p1=p10=p100=p1000=n;last1=last10=last100=n;healthy=true;st={};}
void Scheduler_Run()
{
    uint32_t n=millis();
    if(n-p1>=Settings::TASK_1MS_PERIOD_MS){p1+=Settings::TASK_1MS_PERIOD_MS;uint32_t t=micros();Scheduler_Task1ms();update(st.task1msMaxDurationUs,t,Settings::TASK_1MS_MAX_US);++st.task1msCount;last1=n;Watchdog_MarkTask1msAlive();}
    if(n-p10>=Settings::TASK_10MS_PERIOD_MS){p10+=Settings::TASK_10MS_PERIOD_MS;uint32_t t=micros();Scheduler_Task10ms();update(st.task10msMaxDurationUs,t,Settings::TASK_10MS_MAX_US);++st.task10msCount;last10=n;Watchdog_MarkTask10msAlive();}
    if(n-p100>=Settings::TASK_100MS_PERIOD_MS){p100+=Settings::TASK_100MS_PERIOD_MS;uint32_t t=micros();Scheduler_Task100ms();update(st.task100msMaxDurationUs,t,Settings::TASK_100MS_MAX_US);++st.task100msCount;last100=n;Watchdog_MarkTask100msAlive();}
    if(n-p1000>=Settings::TASK_1S_PERIOD_MS){p1000+=Settings::TASK_1S_PERIOD_MS;uint32_t t=micros();Scheduler_Task1s();update(st.task1sMaxDurationUs,t,Settings::TASK_1S_MAX_US);++st.task1sCount;}
}
void Scheduler_Task1ms(){
#if FEATURE_EFUSE_ENABLED
EFuse_Task1ms();
#endif
}
void Scheduler_Task10ms(){
#if FEATURE_CAN_ENABLED
CANMessages_Task10ms();
#endif
#if FEATURE_BUTTONS_ENABLED
Buttons_Task10ms();
#endif
#if FEATURE_EFUSE_ENABLED
EFuse_Task10ms();
#endif
#if FEATURE_SUPPLY_VOLTAGE_ENABLED
SupplyVoltage_Task10ms();
#endif
FaultManager_Task10ms();
#if !TEST_MODE_ENABLED
PDMControl_Task10ms();
#endif
#if FEATURE_OUTPUTS_ENABLED
Outputs_Task10ms();
#endif
#if FEATURE_LEDS_ENABLED
LEDs_Task10ms();
#endif
}
void Scheduler_Task100ms(){
#if FEATURE_TEMPERATURE_ENABLED
Temperature_Task100ms();
#endif
#if FEATURE_SUPPLY_VOLTAGE_ENABLED
SupplyVoltage_Task100ms();
#endif
#if FEATURE_EFUSE_ENABLED
EFuse_Task100ms();
#endif
FaultManager_Task100ms();
#if FEATURE_CAN_ENABLED
CANMessages_SendStatus(); CANMessages_SendCurrents(); static bool slow=false; slow=!slow; if(slow) CANMessages_SendSensors(); CANMessages_SendFaults();
#endif
#if FEATURE_LEDS_ENABLED
LEDs_Task100ms();
#endif
HardwareTest_Task100ms();
}
void Scheduler_Task1s(){PDMControl_Task1s();CANMessages_SendDiagnostics();UARTDebug_Task1s();FaultManager_Task1s();ResetMonitor_Task1s();}
bool Scheduler_IsHealthy(){uint32_t n=millis();return healthy&&(n-last1<10)&&(n-last10<30)&&(n-last100<300);}
bool Scheduler_HasTask1msRunRecently(){return millis()-last1<10;}
bool Scheduler_HasTask10msRunRecently(){return millis()-last10<30;}
bool Scheduler_HasTask100msRunRecently(){return millis()-last100<300;}
uint16_t Scheduler_GetOverrunCount(){return st.overrunCount;}
SchedulerStatistics Scheduler_GetStatistics(){return st;}
void Scheduler_ClearStatistics(){st={};healthy=true;}
