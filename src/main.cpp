#include <Arduino.h>
#include "outputs.h"
#include "reset_monitor.h"
#include "uart_debug.h"
#include "adc_driver.h"
#include "buttons.h"
#include "leds.h"
#include "supply_voltage.h"
#include "temperature.h"
#include "efuse.h"
#include "fault_manager.h"
#include "pdm_control.h"
#include "can_driver.h"
#include "can_messages.h"
#include "scheduler.h"
#include "watchdog.h"
#include "hardware_test.h"
#include "test_config.h"

void setup()
{
    ResetMonitor_CaptureEarly();
    Outputs_InitSafe();
    EFuse_Init();
    ResetMonitor_Init();
    UARTDebug_Init();
    UARTDebug_PrintStartupBanner();
    ADC_Init();
    Buttons_Init();
    LEDs_Init();
    SupplyVoltage_Init();
    Temperature_Init();
    FaultManager_Init();
    CANMessages_Init();
    PDMControl_Init();
#if FEATURE_CAN_ENABLED
    if(!CAN_Init()) FaultManager_Latch(SystemFault::CANInitialization);
#endif
    HardwareTest_Init();
    Scheduler_Init();
    Watchdog_Init();
#if FEATURE_WATCHDOG_ENABLED
    Watchdog_Enable();
#endif
}

void loop()
{
    Watchdog_MarkMainLoopAlive();
#if FEATURE_CAN_ENABLED
    CAN_Process();
#endif
#if FEATURE_CAN_ENABLED
    CANMessages_ProcessReceived();
#endif
    HardwareTest_ProcessSerial();
#if !TEST_MODE_ENABLED
    UARTDebug_ProcessInput();
#endif
    Scheduler_Run();
#if FEATURE_WATCHDOG_ENABLED
    Watchdog_ServiceIfHealthy();
#endif
}
