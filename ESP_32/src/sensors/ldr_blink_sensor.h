#pragma once
#include <Arduino.h>
#include "diff_threshold_sensor.h"
#include "arduino_moving_average.h"
#include <arduino_stopwatch.h>
#include "i_adc.h"

using ArduinoStopwatch::Stopwatch16MS;

class LdrBlinkSensor {
public:
    enum class STATE : uint8_t{
        OFF = 0,
        EDGE = 1,
        SLOW_BLINKING = 2,
        FAST_BLINKING = 3
    };
    static const int16_t DEFAULT_LDR_ON_THRESHOLD = 300;
    static const int16_t DEFAULT_LDR_OFF_THRESHOLD = -300;
    static const uint16_t DEFAULT_FAST_BLINK_UPPER_THRESHOLD = 500;
    static const uint16_t DEFAULT_FAST_BLINK_LOWER_THRESHOLD = 100;
    static const uint16_t DEFAULT_SLOW_BLINK_UPPER_THRESHOLD = 1000;
    static const uint16_t DEFAULT_SLOW_BLINK_LOWER_THRESHOLD = 600;
public:
    LdrBlinkSensor(
        int16_t ldrOnThreshold=DEFAULT_LDR_ON_THRESHOLD, 
        int16_t ldrOffThreshold=DEFAULT_LDR_OFF_THRESHOLD,
        uint16_t fastBlinkUpperThreshold=DEFAULT_FAST_BLINK_UPPER_THRESHOLD, 
        uint16_t fastBlinkLowerThreshold=DEFAULT_FAST_BLINK_LOWER_THRESHOLD,
        uint16_t slowBlinkUpperThreshold=DEFAULT_SLOW_BLINK_UPPER_THRESHOLD, 
        uint16_t slowBlinkLowerThreshold=DEFAULT_SLOW_BLINK_LOWER_THRESHOLD
    );
    void update(I_Adc& adc, u8_t ch);
    void update(bool isNewValue, int16_t newValue);
    STATE getState() const;
    String getStateString() const;
private:
    uint16_t fastBlinkUpperThreshold;
    uint16_t fastBlinkLowerThreshold;
    uint16_t slowBlinkUpperThreshold;
    uint16_t slowBlinkLowerThreshold;
    DiffThresholdSensor ldrSensor;
    STATE state;
    Stopwatch16MS blinkWatch;

};