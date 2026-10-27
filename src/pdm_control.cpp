#include "pdm_control.h"
#include "can_messages.h"
#include "efuse.h"
#include "outputs.h"
#include "temperature.h"
#include "supply_voltage.h"
#include "fault_manager.h"
#include "leds.h"

namespace { PDMControlStatus s={PDMState::Startup,false,false,false,false,false,0,0}; }
void PDMControl_Init(){s={PDMState::Startup,false,false,false,false,false,0,0};EFuse_SetSystemPermission(EFuseId::Channel1,false);EFuse_SetSystemPermission(EFuseId::Channel2,false);Outputs_ForceAllOff(OutputInhibitReason::SystemFault);}
void PDMControl_Task10ms()
{
    const PDMCommand&c=CANMessages_GetCommand();s.canCommandValid=CANMessages_HasValidCommand();s.emergencyOffActive=c.emergencyOffRequested;s.voltagePermission=SupplyVoltage_AllowsOutputs();s.temperaturePermission=Temperature_AreAllSensorsValid()&&!Temperature_RequiresShutdown();s.systemPermission=!FaultManager_HasCriticalFault()&&s.canCommandValid&&s.voltagePermission&&s.temperaturePermission&&!s.emergencyOffActive;
    if(s.emergencyOffActive)s.state=PDMState::EmergencyOff;else if(FaultManager_HasCriticalFault())s.state=PDMState::CriticalFault;else if(!s.canCommandValid)s.state=PDMState::CANTimeout;else if(SupplyVoltage_IsUndervoltage())s.state=PDMState::Undervoltage;else if(SupplyVoltage_IsOvervoltage())s.state=PDMState::Overvoltage;else if(!s.temperaturePermission)s.state=PDMState::Overtemperature;else s.state=PDMState::Operational;
    EFuse_RequestState(EFuseId::Channel1,c.efuse1Requested);EFuse_RequestState(EFuseId::Channel2,c.efuse2Requested);EFuse_SetSystemPermission(EFuseId::Channel1,s.systemPermission);EFuse_SetSystemPermission(EFuseId::Channel2,s.systemPermission);
    Outputs_Request(OutputId::OutputPC0,c.outputPC0Requested);Outputs_Request(OutputId::OutputPC1,c.outputPC1Requested);Outputs_SetSystemPermission(s.systemPermission,OutputInhibitReason::SystemFault);
    if(s.emergencyOffActive){EFuse_ForceAllOff(EFuseFault::EmergencyOff);Outputs_ForceAllOff(OutputInhibitReason::EmergencyOff);}
    LEDs_SetPattern(LEDId::SystemFault,FaultManager_HasCriticalFault()?LEDPattern::Solid:(FaultManager_HasAnyFault()?LEDPattern::SlowFlash:LEDPattern::Off));LEDs_SetPattern(LEDId::EFuse1Fault,EFuse_GetLatchedFaults(EFuseId::Channel1)?LEDPattern::Solid:(EFuse_GetState(EFuseId::Channel1)==EFuseState::RetryWait?LEDPattern::SlowFlash:LEDPattern::Off));LEDs_SetPattern(LEDId::EFuse2Fault,EFuse_GetLatchedFaults(EFuseId::Channel2)?LEDPattern::Solid:(EFuse_GetState(EFuseId::Channel2)==EFuseState::RetryWait?LEDPattern::SlowFlash:LEDPattern::Off));
}
void PDMControl_Task100ms(){}
void PDMControl_Task1s(){++s.runtimeSeconds;++s.aliveCounter;}
void PDMControl_EmergencyShutdown(){s.emergencyOffActive=true;FaultManager_Latch(SystemFault::EmergencyOff);EFuse_ForceAllOff(EFuseFault::EmergencyOff);Outputs_ForceAllOff(OutputInhibitReason::EmergencyOff);}
bool PDMControl_RequestFaultReset(){if(s.emergencyOffActive||Temperature_RequiresShutdown())return false;bool a=EFuse_ClearFault(EFuseId::Channel1),b=EFuse_ClearFault(EFuseId::Channel2);FaultManager_ClearAllNonCritical();return a&&b;}
PDMState PDMControl_GetState(){return s.state;}
bool PDMControl_AllowsOutputs(){return s.systemPermission;}
bool PDMControl_IsEmergencyOffActive(){return s.emergencyOffActive;}
uint8_t PDMControl_GetAliveCounter(){return s.aliveCounter;}
uint32_t PDMControl_GetRuntimeSeconds(){return s.runtimeSeconds;}
PDMControlStatus PDMControl_GetStatus(){return s;}
