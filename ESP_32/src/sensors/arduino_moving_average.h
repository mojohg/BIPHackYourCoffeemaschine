#pragma once
#include <Arduino.h>

namespace ArduinoMovingAverage {

template <typename TValue, typename TSize, TSize maxSize> 
class MovingAverageFilter
{
public:
    MovingAverageFilter(TSize size, TValue min, TValue max) :
        currentValue(0),
        oldestValueIndex(0),
        currentLength(0),
        minValue(min),
        maxValue(max)
    {
        if (size < maxSize) {
            this->size = size;
        } else {
            this->size = maxSize;
        }
    }

    TValue addValue(TValue newValue) {
        //////// math:
        // if (currentLength < size):
        //      currentValue += (newValue - currentValue) / currentLength
        // else
        //      currentValue += (newValue - oldestValue) / currentLength 
        ///////////////

        // first value:
        if(currentLength == 0) {
            currentValue = newValue;
            values[oldestValueIndex] = newValue;
            increaseOldestValueIndex();
            currentLength++;
            return currentValue;
        }

        // add newValue:
        TValue oldestValue;
        if(currentLength < size) {
            oldestValue = currentValue;
            currentLength++;
        } else {
            oldestValue = values[oldestValueIndex];
        }
        values[oldestValueIndex] = newValue;
        increaseOldestValueIndex();
        // calculate currentValue:
        if(newValue >= oldestValue) { // increase:
            TValue change = (newValue - oldestValue) / currentLength;
            if(maxValue - currentValue >= change) {
                currentValue += change;
            } else {
                currentValue = maxValue;
            }
        } else { // decrease:
            TValue change = (oldestValue - newValue) / currentLength;
            if(currentValue - minValue >= change) {
                currentValue -= change;
            } else {
                currentValue = minValue;
            }
        }
        return currentValue;
    }
    TValue getCurrentValue() const {
        return currentValue;
    }
private:
    void increaseOldestValueIndex() {
        if(oldestValueIndex < size-1) {
            oldestValueIndex++;
        } else {
            oldestValueIndex = 0;
        }
    }
private:
    TValue values[maxSize];
    TValue currentValue;
    TSize oldestValueIndex;
    TSize currentLength;
    TSize size;
    TValue minValue;
    TValue maxValue;
};

template <typename TSize, TSize maxSize> 
class BinaryMovingAverageFilter
{
public:
    BinaryMovingAverageFilter(TSize size) :
        oldestValueIndex(0),
        currentLength(0),
        currentValue(0),
        trueCount(0)
    {
        if (size < maxSize) {
            this->size = size;
        } else {
            this->size = maxSize;
        }
    }

    bool addValue(bool newValue) {
        // remove old value:
        if(currentLength == size && values[oldestValueIndex]) {
            trueCount--;
        }
        // add new value:
        values[oldestValueIndex] = newValue;
        increaseOldestValueIndex();
        if(currentLength < size) {
            currentLength++;
        }
        if(newValue) {
            trueCount++;
        }
        // calculate currentValue:        
        currentValue = (trueCount > currentLength/2);
        return currentValue;
    }
    bool getCurrentValue() const {
        return currentValue;
    }
private:
    void increaseOldestValueIndex() {
        if(oldestValueIndex < size-1) {
            oldestValueIndex++;
        } else {
            oldestValueIndex = 0;
        }
    }
private:
    bool values[maxSize];
    TSize oldestValueIndex;
    TSize currentLength;
    TSize size;
    TSize trueCount;
    bool currentValue;
};

} 