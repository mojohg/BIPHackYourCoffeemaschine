#pragma once
#include <Arduino.h>
#include "Adafruit_ADS1X15.h"
#include "arduino_moving_average.h"
#include "i_adc.h"
#include "esp_log.h"

using Ads1115MovingAverageFilter = ArduinoMovingAverage::MovingAverageFilter<int16_t, uint8_t, 32>;

class Ads1115Continuous : public I_Adc {
public:
    Ads1115Continuous(TwoWire* i2c);
    bool begin(bool enableCh0=true, bool enableCh1=true, bool enableCh2=true, bool enableCh3=true, uint8_t movingAverageSize=8);
    void update();
    void enable(u8_t ch);
    void disable(u8_t ch);
    void setMovingAverageSize(u8_t ch, uint8_t size);
    bool isNewValueAvailable(u8_t ch) const;
    int16_t getNewValue(u8_t ch);
    bool tryGetNewValue(u8_t ch, int16_t& value);
private:
    u8_t checkChannel(u8_t ch) const;
    bool enables[4] = {true, true, true, true};
    Ads1115MovingAverageFilter filters[4] = {Ads1115MovingAverageFilter(8, INT16_MIN, INT16_MAX), Ads1115MovingAverageFilter(8, INT16_MIN, INT16_MAX), Ads1115MovingAverageFilter(8, INT16_MIN, INT16_MAX), Ads1115MovingAverageFilter(8, INT16_MIN, INT16_MAX)};
    bool available[4] = {false, false, false, false};
    Adafruit_ADS1115 ads; 
    u8_t currentChannel =0;
    const char* ADS_LOG_TAG = "Ads1115Continuous";
    TwoWire* i2c0;
};
