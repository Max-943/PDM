#include "fault_manager.h"
#include "can_driver.h"
#include "can_messages.h"
#include "supply_voltage.h"
#include "temperature.h"
#include "scheduler.h"

namespace { FaultManagerStatus s={}; uint16_t criticalMask=static_cast<uint16_t>(SystemFault::Overtemperature)|static_cast<uint16_t>(SystemFault::ADCFault)|static_cast<uint16_t>(SystemFault::ConfigurationInvalid)|static_cast<uint16_t>(SystemFault::InternalError)|static_cast<uint16_t>(SystemFault::EmergencyOff); }
static uint16_t fault_bit(SystemFault fault)
{
    return static_cast<uint16_t>(fault);
}
void FaultManager_Init(){s={};}
void FaultManager_Task10ms()
{
    if(CANMessages_HasTimedOut())FaultManager_SetActive(SystemFault::CANTimeout);else FaultManager_ClearActive(SystemFault::CANTimeout);
    if(SupplyVoltage_IsUndervoltage())FaultManager_SetActive(SystemFault::SupplyUndervoltage);else FaultManager_ClearActive(SystemFault::SupplyUndervoltage);
    if(SupplyVoltage_IsOvervoltage())FaultManager_SetActive(SystemFault::SupplyOvervoltage);else FaultManager_ClearActive(SystemFault::SupplyOvervoltage);
    if(CAN_GetState()==CANDriverState::BusOff)FaultManager_SetActive(SystemFault::CANBusOff);else FaultManager_ClearActive(SystemFault::CANBusOff);
}
void FaultManager_Task100ms()
{
    if(!Temperature_AreAllSensorsValid())FaultManager_SetActive(SystemFault::TemperatureSensor);else FaultManager_ClearActive(SystemFault::TemperatureSensor);
    if(Temperature_RequiresShutdown())FaultManager_SetActive(SystemFault::Overtemperature);else FaultManager_ClearActive(SystemFault::Overtemperature);
    if(!Scheduler_IsHealthy())FaultManager_SetActive(SystemFault::SchedulerOverrun);else FaultManager_ClearActive(SystemFault::SchedulerOverrun);
}
void FaultManager_Task1s(){}
void FaultManager_SetActive(SystemFault f){uint16_t m=fault_bit(f);if(!(s.activeSystemFaults&m)){s.activeSystemFaults|=m;s.historicalSystemFaults|=m;++s.faultSequenceCounter;++s.totalFaultCount;}}
void FaultManager_ClearActive(SystemFault f){s.activeSystemFaults&=static_cast<uint16_t>(~fault_bit(f));}
void FaultManager_Latch(SystemFault f){uint16_t m=fault_bit(f);s.latchedSystemFaults|=m;s.historicalSystemFaults|=m;++s.faultSequenceCounter;++s.totalFaultCount;}
bool FaultManager_ClearLatched(SystemFault f){if(fault_bit(f)&criticalMask)return false;s.latchedSystemFaults&=static_cast<uint16_t>(~fault_bit(f));return true;}
void FaultManager_ClearAllNonCritical(){s.latchedSystemFaults&=criticalMask;}
bool FaultManager_IsActive(SystemFault f){return(s.activeSystemFaults&fault_bit(f))!=0;}
bool FaultManager_IsLatched(SystemFault f){return(s.latchedSystemFaults&fault_bit(f))!=0;}
bool FaultManager_HasAnyFault(){return(s.activeSystemFaults|s.latchedSystemFaults)!=0;}
bool FaultManager_HasCriticalFault(){return((s.activeSystemFaults|s.latchedSystemFaults)&criticalMask)!=0;}
uint16_t FaultManager_GetActiveFaults(){return s.activeSystemFaults;}
uint16_t FaultManager_GetLatchedFaults(){return s.latchedSystemFaults;}
uint16_t FaultManager_GetHistoricalFaults(){return s.historicalSystemFaults;}
uint8_t FaultManager_GetSequenceCounter(){return s.faultSequenceCounter;}
uint16_t FaultManager_GetTotalFaultCount(){return s.totalFaultCount;}
FaultManagerStatus FaultManager_GetStatus(){return s;}
