#include "adc_driver.h"
#include "pdm_settings.h"
#include <avr/io.h>

static uint8_t adcPrescalerBits()
{
    switch (Settings::ADC_PRESCALER)
    {
        case 2:   return _BV(ADPS0);
        case 4:   return _BV(ADPS1);
        case 8:   return _BV(ADPS1) | _BV(ADPS0);
        case 16:  return _BV(ADPS2);
        case 32:  return _BV(ADPS2) | _BV(ADPS0);
        case 64:  return _BV(ADPS2) | _BV(ADPS1);
        default:  return _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    }
}

void ADC_Init()
{
    ADMUX = _BV(REFS0); // AVCC reference
    ADCSRA = _BV(ADEN) | adcPrescalerBits();
    DIDR0 = _BV(ADC0D) | _BV(ADC3D) | _BV(ADC4D) |
            _BV(ADC5D) | _BV(ADC6D) | _BV(ADC7D);
    (void)ADC_ReadRaw(0);
}

uint16_t ADC_ReadRaw(uint8_t channel)
{
    static uint8_t previousChannel = 0xFF;
    channel &= 0x07;
    ADMUX = _BV(REFS0) | channel;
    if (channel != previousChannel)
    {
        ADCSRA |= _BV(ADSC);
        while (ADCSRA & _BV(ADSC)) {}
        (void)ADC;
        previousChannel = channel;
    }
    ADCSRA |= _BV(ADSC);
    while (ADCSRA & _BV(ADSC)) {}
    return ADC;
}

uint16_t ADC_ReadAverage(uint8_t channel, uint8_t sampleCount)
{
    if (sampleCount == 0) sampleCount = 1;
    uint32_t total = 0;
    for (uint8_t i = 0; i < sampleCount; ++i) total += ADC_ReadRaw(channel);
    return static_cast<uint16_t>(total / sampleCount);
}

uint16_t ADC_RawToMillivolts(uint16_t rawValue)
{
    if (rawValue > Settings::ADC_MAX_COUNT) rawValue = Settings::ADC_MAX_COUNT;
    return static_cast<uint16_t>((static_cast<uint32_t>(rawValue) * Settings::ADC_REFERENCE_MV + 511UL) / Settings::ADC_MAX_COUNT);
}

bool ADC_IsRawPlausible(uint16_t rawValue)
{
    return rawValue <= Settings::ADC_MAX_COUNT;
}
