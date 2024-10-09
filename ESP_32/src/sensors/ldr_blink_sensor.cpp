#include "ldr_blink_sensor.h"

LdrBlinkSensor::LdrBlinkSensor(
    int16_t ldrOnThreshold, 
    int16_t ldrOffThreshold,
    uint16_t fastBlinkUpperThreshold, 
    uint16_t fastBlinkLowerThreshold,
    uint16_t slowBlinkUpperThreshold, 
    uint16_t slowBlinkLowerThreshold
) :
    fastBlinkUpperThreshold(fastBlinkUpperThreshold),
    fastBlinkLowerThreshold(fastBlinkLowerThreshold),
    slowBlinkUpperThreshold(slowBlinkUpperThreshold),
    slowBlinkLowerThreshold(slowBlinkLowerThreshold),
    ldrSensor(ldrOnThreshold, ldrOffThreshold, 1),
    state(STATE::OFF)
{

}
void LdrBlinkSensor::update(I_Adc& adc, u8_t ch) {
    update(adc.isNewValueAvailable(ch), adc.getNewValue(ch));
}
void LdrBlinkSensor::update(bool isNewValue, int16_t newValue) {
    if(!isNewValue) {
        return;
    }
    bool ldrOnOld = ldrSensor.getState();
    ldrSensor.update(isNewValue, newValue);
    bool ldrOn = ldrSensor.getState();

    switch (state)
    {
    default:
    case STATE::OFF:
        if(ldrOnOld == false && ldrOn == true) { // rising edge:
            blinkWatch.restart();
            state = STATE::EDGE;
        }
        break;
    case STATE::EDGE:
    case STATE::FAST_BLINKING:
    case STATE::SLOW_BLINKING:
        uint16_t blinkTime = blinkWatch.getTimeSinceStart();
        if(ldrOnOld == false && ldrOn == true) { // rising edge:
            blinkWatch.restart();
            if(fastBlinkLowerThreshold <= blinkTime && blinkTime <= fastBlinkUpperThreshold) { // fast blink:
                state = STATE::FAST_BLINKING;
            } else if(slowBlinkLowerThreshold <= blinkTime && blinkTime <= slowBlinkUpperThreshold) { // slow blink:
                state = STATE::SLOW_BLINKING;
            } else {
                state = STATE::OFF;
            }
        } else if(blinkTime >= 2*slowBlinkUpperThreshold) {
            state = STATE::OFF;
        }
        break;
    }
}
LdrBlinkSensor::STATE LdrBlinkSensor::getState() const {
    return (state == STATE::EDGE) ? STATE::OFF : state;
}
String LdrBlinkSensor::getStateString() const {
    switch (getState())
    {
    default:
    case STATE::OFF: return "OFF";
    case STATE::SLOW_BLINKING: return "SLOW-BLINKING";
    case STATE::FAST_BLINKING: return "FAST-BLINKING";
    }
}