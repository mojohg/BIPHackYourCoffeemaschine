#pragma once
#include <Arduino.h>

class I_Adc {
public:
    virtual bool isNewValueAvailable(u8_t ch) const = 0;
    virtual int16_t getNewValue(u8_t ch) = 0;
    virtual bool tryGetNewValue(u8_t ch, int16_t& value) = 0;
};