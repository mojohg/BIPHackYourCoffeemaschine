#include "diff_threshold_sensor.h"

DiffThresholdSensor::DiffThresholdSensor(
    int16_t onThreshold, 
    int16_t offThreshold,
    uint8_t filterSize
) :
    onThreshold(onThreshold),
    offThreshold(offThreshold),
    oldValue(0),
    firstValue(true),
    stateFilter(ArduinoMovingAverage::BinaryMovingAverageFilter<uint8_t, 8>(filterSize))
{

}
void DiffThresholdSensor::update(I_Adc& adc, u8_t ch) {
    update(adc.isNewValueAvailable(ch), adc.getNewValue(ch));
}
void DiffThresholdSensor::update(bool isNewValue, int16_t newValue) {
    if(!isNewValue) {
        return;
    }
    if(firstValue) {
        firstValue = false;
        oldValue = newValue;
        return;
    }
    int16_t diff = newValue - oldValue;
    bool ldrOnOld = stateFilter.getCurrentValue();
    if(ldrOnOld && diff <= offThreshold) {
        stateFilter.addValue(false);
    } else if(!ldrOnOld && diff >= onThreshold) {
        stateFilter.addValue(true);
    }    
    oldValue = newValue;
}
bool DiffThresholdSensor::getState() const {
    return stateFilter.getCurrentValue();
}