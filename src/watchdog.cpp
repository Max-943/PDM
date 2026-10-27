#include "watchdog.h"
#include "scheduler.h"
#include <avr/wdt.h>

namespace { bool mainAlive=false,t1=false,t10=false,t100=false;uint32_t services=0; }
void Watchdog_Init(){wdt_disable();mainAlive=t1=t10=t100=false;}
void Watchdog_Enable(){wdt_enable(WDTO_500MS);}
void Watchdog_Disable(){wdt_disable();}
void Watchdog_MarkMainLoopAlive(){mainAlive=true;}
void Watchdog_MarkTask1msAlive(){t1=true;}
void Watchdog_MarkTask10msAlive(){t10=true;}
void Watchdog_MarkTask100msAlive(){t100=true;}
bool Watchdog_IsSoftwareHealthy(){return mainAlive&&t1&&t10&&t100&&Scheduler_IsHealthy();}
void Watchdog_ServiceIfHealthy(){if(Watchdog_IsSoftwareHealthy()){wdt_reset();++services;mainAlive=t1=t10=t100=false;}}
uint32_t Watchdog_GetServiceCount(){return services;}
