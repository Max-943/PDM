#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <Arduino.h>

/*
 * PDM PCB pin allocation for MightyCore "Standard pinout".
 *
 * Ownership rule:
 * - can_driver.cpp owns MCP2515 pins.
 * - efuse.cpp owns SHDN1/SHDN2 and reads FLT1/FLT2.
 * - outputs.cpp owns PC0/PC1.
 * - buttons.cpp owns PC2/PC3 as inputs.
 * - leds.cpp owns all LED pins.
 * - adc_driver.cpp owns the ADC peripheral.
 */
namespace Board
{
    // Port B, MightyCore Standard digital numbers
    constexpr uint8_t CAN_CS       = 0;  // PB0
    constexpr uint8_t LED_BUTTON   = 1;  // PB1
    constexpr uint8_t CAN_INT      = 2;  // PB2 / INT2
    constexpr uint8_t EFUSE1_SHDN  = 3;  // PB3
    constexpr uint8_t EFUSE2_SHDN  = 4;  // PB4 / AVR hardware SS

    // ADC channel numbers, not Arduino digital pin numbers
    constexpr uint8_t ADC_IMON1 = 0;  // PA0 / ADC0
    constexpr uint8_t ADC_IMON2 = 3;  // PA3 / ADC3
    constexpr uint8_t ADC_NTC1  = 4;  // PA4 / ADC4
    constexpr uint8_t ADC_NTC2  = 5;  // PA5 / ADC5
    constexpr uint8_t ADC_NTC3  = 6;  // PA6 / ADC6
    constexpr uint8_t ADC_VIN   = 7;  // PA7 / ADC7

    // Direct-register Port C bits
    constexpr uint8_t OUTPUT_PC0_BIT = PC0;
    constexpr uint8_t OUTPUT_PC1_BIT = PC1;
    constexpr uint8_t BUTTON1_PC2_BIT = PC2;
    constexpr uint8_t BUTTON2_PC3_BIT = PC3;
    constexpr uint8_t EFUSE1_FLT_PC4_BIT = PC4;
    constexpr uint8_t EFUSE2_FLT_PC7_BIT = PC7;

    // Direct-register Port D bits
    constexpr uint8_t LED_CAN_PD4_BIT     = PD4;
    constexpr uint8_t LED_SYSTEM_PD5_BIT  = PD5;
    constexpr uint8_t LED_EFUSE1_PD6_BIT  = PD6;
    constexpr uint8_t LED_EFUSE2_PD7_BIT  = PD7;

    // UART allocation
    constexpr uint8_t DEBUG_UART_INDEX = 0;  // USART0: PD0/PD1, MCP2221A
    constexpr uint8_t EXTERNAL_UART_INDEX = 1; // USART1: PD2/PD3

    // Board electrical characteristics
    constexpr bool LEDS_ACTIVE_HIGH = true;
    constexpr bool BUTTONS_ACTIVE_LOW = true;
    constexpr bool EFUSE_FLT_ACTIVE_LOW = true;
    constexpr bool EFUSE_SHDN_ACTIVE_HIGH = true;
}

#endif
