#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

/*
 * Bench-test controls.
 * TEST_MODE_ENABLED is OFF by default so serial commands cannot energize outputs
 * in a normal firmware build. Set to 1 only on a current-limited test bench.
 */
#ifndef TEST_MODE_ENABLED
#define TEST_MODE_ENABLED 0
#endif

#ifndef FEATURE_CAN_ENABLED
#define FEATURE_CAN_ENABLED 1
#endif
#ifndef FEATURE_EFUSE_ENABLED
#define FEATURE_EFUSE_ENABLED 1
#endif
#ifndef FEATURE_TEMPERATURE_ENABLED
#define FEATURE_TEMPERATURE_ENABLED 1
#endif
#ifndef FEATURE_SUPPLY_VOLTAGE_ENABLED
#define FEATURE_SUPPLY_VOLTAGE_ENABLED 1
#endif
#ifndef FEATURE_BUTTONS_ENABLED
#define FEATURE_BUTTONS_ENABLED 1
#endif
#ifndef FEATURE_OUTPUTS_ENABLED
#define FEATURE_OUTPUTS_ENABLED 1
#endif
#ifndef FEATURE_LEDS_ENABLED
#define FEATURE_LEDS_ENABLED 1
#endif
#ifndef FEATURE_WATCHDOG_ENABLED
#define FEATURE_WATCHDOG_ENABLED 1
#endif
#ifndef FEATURE_PERIODIC_SERIAL_SUMMARY
#define FEATURE_PERIODIC_SERIAL_SUMMARY 1
#endif

#endif
