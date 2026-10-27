#ifndef PDM_SETTINGS_H
#define PDM_SETTINGS_H

#include <Arduino.h>

/*
 * Central calibration and behavior configuration.
 * No module should duplicate these constants locally.
 */
namespace Settings
{
    // Build and board identity
    constexpr uint8_t HARDWARE_REVISION = 1;
    constexpr uint8_t CAN_NODE_ID = 1;

    // CPU and ADC
    constexpr uint32_t CPU_FREQUENCY_HZ = 8000000UL;
    constexpr uint16_t ADC_REFERENCE_MV = 5000; // Replace with measured AVCC
    constexpr uint16_t ADC_MAX_COUNT = 1023;
    constexpr uint8_t ADC_PRESCALER = 64;
    constexpr uint8_t CURRENT_ADC_SAMPLES = 4;
    constexpr uint8_t SLOW_ADC_SAMPLES = 16;
    constexpr uint16_t ADC_SHORT_THRESHOLD = 5;
    constexpr uint16_t ADC_OPEN_THRESHOLD = 1018;

    // TPS26631 IMON
    constexpr uint32_t IMON_RESISTOR_OHM = 18000UL;
    constexpr uint32_t IMON_GAIN_NA_PER_A = 27900UL;
    constexpr uint16_t EFUSE1_GAIN_NUM = 1000;
    constexpr uint16_t EFUSE1_GAIN_DEN = 1000;
    constexpr int16_t EFUSE1_OFFSET_MA = 0;
    constexpr uint16_t EFUSE2_GAIN_NUM = 1000;
    constexpr uint16_t EFUSE2_GAIN_DEN = 1000;
    constexpr int16_t EFUSE2_OFFSET_MA = 0;
    constexpr uint16_t CURRENT_PLAUSIBILITY_MAX_MA = 8000;
    constexpr uint8_t CURRENT_FILTER_SHIFT = 2; // IIR alpha = 1/4

    // Software eFuse protection. Hardware ILIM remains mandatory.
    constexpr uint16_t EFUSE1_RUN_LIMIT_MA = 5000;
    constexpr uint16_t EFUSE2_RUN_LIMIT_MA = 5000;
    constexpr uint16_t EFUSE1_START_LIMIT_MA = 6000;
    constexpr uint16_t EFUSE2_START_LIMIT_MA = 6000;
    constexpr uint16_t EFUSE_START_WINDOW_MS = 100;
    constexpr uint16_t EFUSE_OVERCURRENT_DELAY_MS = 10;
    constexpr uint16_t EFUSE_RETRY_DELAY_MS = 1000;
    constexpr uint16_t EFUSE_MINIMUM_OFF_TIME_MS = 100;
    constexpr uint8_t EFUSE_MAX_RETRIES = 3;
    constexpr bool EFUSE1_AUTO_RETRY = true;
    constexpr bool EFUSE2_AUTO_RETRY = true;
    constexpr uint8_t FLT_ACTIVE_SAMPLES = 2;
    constexpr uint8_t FLT_CLEAR_SAMPLES = 3;

    // NTC divider. Confirm exact fitted NTC R25 and Beta before use.
    constexpr uint32_t NTC_PULLUP_OHM = 4700UL;
    constexpr uint32_t NTC_R25_OHM = 10000UL; // TODO: confirm part number
    constexpr uint16_t NTC_BETA_K = 4000;     // TODO: confirm part number
    constexpr int16_t NTC1_OFFSET_DC = 0;
    constexpr int16_t NTC2_OFFSET_DC = 0;
    constexpr int16_t NTC3_OFFSET_DC = 0;
    constexpr int16_t TEMP_WARNING_DC = 850;  // 85.0 C
    constexpr int16_t TEMP_SHUTDOWN_DC = 1000; // 100.0 C
    constexpr int16_t TEMP_RESTART_DC = 800;  // 80.0 C
    constexpr uint8_t TEMPERATURE_FILTER_SHIFT = 2;

    // 12 V divider and protection
    constexpr uint32_t VIN_TOP_RESISTOR_OHM = 10000UL;
    constexpr uint32_t VIN_BOTTOM_RESISTOR_OHM = 3000UL;
    constexpr uint16_t VIN_CAL_NUM = 1000;
    constexpr uint16_t VIN_CAL_DEN = 1000;
    constexpr uint16_t VIN_UNDERVOLTAGE_MV = 9000;
    constexpr uint16_t VIN_RECOVERY_MV = 10000;
    constexpr uint16_t VIN_OVERVOLTAGE_MV = 16000;
    constexpr uint8_t VIN_FILTER_SHIFT = 3;

    // CAN and DBC behavior
    constexpr uint32_t CAN_BITRATE = 500000UL;
    constexpr uint32_t MCP2515_OSCILLATOR_HZ = 16000000UL;
    constexpr uint8_t CAN_RX_QUEUE_LENGTH = 8;
    constexpr uint32_t CAN_COMMAND_TIMEOUT_MS = 500;
    constexpr uint8_t CAN_COMMAND_DLC = 8;
    constexpr uint8_t CAN_COUNTER_MAX = 255;
    constexpr uint8_t CAN_CRC8_POLYNOMIAL = 0x1D;
    constexpr uint8_t CAN_CRC8_INITIAL = 0xFF;
    constexpr bool CAN_BUS_OFF_AUTORECOVERY = true;
    constexpr uint16_t CAN_BUS_OFF_RECOVERY_MS = 1000;

    // DBC CAN identifiers
    constexpr uint16_t CAN_ID_COMMAND = 0x100;
    constexpr uint16_t CAN_ID_FAULT_RESET = 0x101;
    constexpr uint16_t CAN_ID_STATUS = 0x200;
    constexpr uint16_t CAN_ID_CURRENTS = 0x201;
    constexpr uint16_t CAN_ID_SENSORS = 0x202;
    constexpr uint16_t CAN_ID_FAULTS = 0x203;
    constexpr uint16_t CAN_ID_DIAGNOSTICS = 0x204;

    // CAN transmit periods
    constexpr uint16_t CAN_STATUS_PERIOD_MS = 100;
    constexpr uint16_t CAN_CURRENT_PERIOD_MS = 100;
    constexpr uint16_t CAN_SENSOR_PERIOD_MS = 500;
    constexpr uint16_t CAN_FAULT_PERIOD_MS = 100;
    constexpr uint16_t CAN_DIAGNOSTIC_PERIOD_MS = 1000;

    // Buttons
    constexpr uint8_t BUTTON_DEBOUNCE_COUNT = 3;
    constexpr uint16_t BUTTON_SAMPLE_PERIOD_MS = 10;
    constexpr uint16_t BUTTON_LONG_PRESS_MS = 1000;

    // LEDs
    constexpr uint16_t LED_CAN_PULSE_MS = 50;
    constexpr uint16_t LED_SLOW_FLASH_MS = 500;
    constexpr uint16_t LED_FAST_FLASH_MS = 100;

    // Scheduler
    constexpr uint16_t TASK_1MS_PERIOD_MS = 1;
    constexpr uint16_t TASK_10MS_PERIOD_MS = 10;
    constexpr uint16_t TASK_100MS_PERIOD_MS = 100;
    constexpr uint16_t TASK_1S_PERIOD_MS = 1000;
    constexpr uint16_t TASK_1MS_MAX_US = 300;
    constexpr uint16_t TASK_10MS_MAX_US = 2000;
    constexpr uint16_t TASK_100MS_MAX_US = 10000;
    constexpr uint16_t TASK_1S_MAX_US = 50000;

    // UART diagnostics
    constexpr uint32_t DEBUG_BAUDRATE = 57600UL;
    constexpr uint16_t DEBUG_SUMMARY_PERIOD_MS = 1000;

    // Watchdog and persistent maintenance
    constexpr uint16_t WATCHDOG_TIMEOUT_MS = 500;
    constexpr uint16_t FAULT_HISTORY_DEPTH = 16;
    constexpr uint32_t EEPROM_MIN_SAVE_INTERVAL_MS = 60000UL;
    constexpr uint32_t RUNTIME_COUNTER_PERIOD_MS = 1000UL;
}

#endif
