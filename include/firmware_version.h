#ifndef FIRMWARE_VERSION_H
#define FIRMWARE_VERSION_H

#include <Arduino.h>

namespace FirmwareVersion
{
    constexpr uint8_t MAJOR = 1;
    constexpr uint8_t MINOR = 0;
    constexpr uint8_t PATCH = 0;
    constexpr uint8_t HARDWARE_REVISION = 1;
    constexpr const char *NAME = "ATmega1284P Motorsport PDM";
}

#endif
