#include "threshold_sensor.h"

ThresholdSensor::ThresholdSensor(
    int16_t onThreshold, 
    int16_t offThreshold,
    uint8_t filterSize
) :
    onThreshold(onThreshold),
    offThreshold(offThreshold),
    stateFilter(ArduinoMovingAverage::BinaryMovingAverageFilter<uint8_t, 8>(filterSize))
{

}
void ThresholdSensor::update(I_Adc& adc, u8_t ch) {
    update(adc.isNewValueAvailable(ch), adc.getNewValue(ch));
}
void ThresholdSensor::update(bool isNewValue, int16_t newValue) {
    if(!isNewValue) {
        return;
    }
    bool ldrOnOld = stateFilter.getCurrentValue();
    if(ldrOnOld && newValue <= offThreshold) {
        stateFilter.addValue(false);
    } else if(!ldrOnOld && newValue >= onThreshold) {
        stateFilter.addValue(true);
    }    
}
bool ThresholdSensor::getState() const {
    return stateFilter.getCurrentValue();
}