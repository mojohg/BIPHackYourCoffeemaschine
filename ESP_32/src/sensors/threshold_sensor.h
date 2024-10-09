#pragma once
#include <Arduino.h>
#include "arduino_moving_average.h"
#include "i_adc.h"

class ThresholdSensor {
public:
    static const int16_t DEFAULT_ON_THRESHOLD = 6000;
    static const int16_t DEFAULT_OFF_THRESHOLD = 5600;
    static const uint8_t DEFAULT_FILTER_SIZE = 3;
public:
    ThresholdSensor(
        int16_t onThreshold=DEFAULT_ON_THRESHOLD, 
        int16_t offThreshold=DEFAULT_OFF_THRESHOLD,
        uint8_t filterSize=DEFAULT_FILTER_SIZE
    );
    void update(I_Adc& adc, u8_t ch);
    void update(bool isNewValue, int16_t newValue);
    bool getState() const;
private:
    int16_t onThreshold;
    int16_t offThreshold;
    ArduinoMovingAverage::BinaryMovingAverageFilter<uint8_t, 8> stateFilter;
};