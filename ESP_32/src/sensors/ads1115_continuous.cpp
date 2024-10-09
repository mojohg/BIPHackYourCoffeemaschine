#include "ads1115_continuous.h"

Ads1115Continuous::Ads1115Continuous(TwoWire* i2c) :
i2c0(i2c) {
    
}
bool Ads1115Continuous::begin(bool enableCh0, bool enableCh1, bool enableCh2, bool enableCh3, uint8_t movingAverageSize) {
    if (!ads.begin(ADS1X15_ADDRESS, i2c0)) {
        return false;
    }
    enables[0] = enableCh0;
    enables[1] = enableCh1;
    enables[2] = enableCh2;
    enables[3] = enableCh3;
    for(uint8_t i=0; i < 4; i++) {
        filters[i] = Ads1115MovingAverageFilter(movingAverageSize, INT16_MIN, INT16_MAX);
        available[i] = false;
    }
    ads.setGain(adsGain_t::GAIN_ONE); // 4.096V
    for(uint8_t i=0; i < 4; i++) {
        uint8_t nextChannel = (currentChannel + i + 1) % 4;
        if(enables[nextChannel]) {
            currentChannel = nextChannel;
            break;
        }
    }
    ads.startADCReading(MUX_BY_CHANNEL[currentChannel], false);
    return true;
}
void Ads1115Continuous::update() {
    if(!ads.conversionComplete()) {
        return;
    }
    filters[uint8_t(currentChannel)].addValue(ads.getLastConversionResults());
    available[uint8_t(currentChannel)] = true;

    for(uint8_t i=0; i < 4; i++) {
        uint8_t nextChannel = (currentChannel + i + 1) % 4;
        if(enables[nextChannel]) {
            currentChannel = nextChannel;
            break;
        }
    }
    ads.startADCReading(MUX_BY_CHANNEL[currentChannel], false);
}
void Ads1115Continuous::enable(u8_t ch) {
    enables[checkChannel(ch)] = true;
}
void Ads1115Continuous::disable(u8_t ch) {
    enables[checkChannel(ch)] = false;
}
void Ads1115Continuous::setMovingAverageSize(u8_t ch, uint8_t size) {
    filters[checkChannel(ch)] = Ads1115MovingAverageFilter(size, INT16_MIN, INT16_MAX);
}
bool Ads1115Continuous::isNewValueAvailable(u8_t ch) const {
    return available[checkChannel(ch)];
}
int16_t Ads1115Continuous::getNewValue(u8_t ch) {
    int16_t value = filters[checkChannel(ch)].getCurrentValue();
    if (available[checkChannel(ch)]) {
        available[checkChannel(ch)] = false;
    }
    return value;
}
bool Ads1115Continuous::tryGetNewValue(u8_t ch, int16_t& value) {
    if (!available[checkChannel(ch)]) {
        return false;
    }
    value = filters[checkChannel(ch)].getCurrentValue();
    available[checkChannel(ch)] = false;
    return true;
}
////////////////////////////////////
inline u8_t Ads1115Continuous::checkChannel(u8_t ch) const {
    if(ch > 3) {
        ESP_LOGE(ADS_LOG_TAG, "Channel out of range: %i", ch);
        return 0;
    }
    return ch;
}