#include <Arduino.h>
#include "esp_log.h"
#include "sensors/ads1115_continuous.h"
#include "sensors/debounced_button.h"
#include "sensors/ldr_blink_sensor.h"
#include "sensors/threshold_sensor.h"
#include "sensors/slider_button.h"

const char* ESP_LOG_TAG = "ESP";
// ADC:
static const int I2C_SCL_PIN = 15;
static const int I2C_SDA_PIN = 16;
static const u8_t ADC_FILTER_SIZE = 4;
static Ads1115Continuous adcConverter;
static const u8_t LIGHT_LEFT_CHANNEL = 0;
static const u8_t LIGHT_RIGHT_CHANNEL = 1;
static const u8_t WATER_LEVEL_CHANNEL = 2;
// LDRs:
static LdrBlinkSensor sensorLightLeft;
static LdrBlinkSensor sensorLightRight;
// Water Level:
static const u16_t WATER_LEVEL_ON_THRESHOLD = 13000;
static const u16_t WATER_LEVEL_OFF_THRESHOLD = 7000;
static const u8_t WATER_LEVEL_FILTER_SIZE = 3;
static ThresholdSensor switchWaterLevel(WATER_LEVEL_ON_THRESHOLD, WATER_LEVEL_OFF_THRESHOLD, WATER_LEVEL_FILTER_SIZE);
// Buttons:
static const int BUTTON_COFFEE_LEFT_PIN = 33;
static const int BUTTON_COFFEE_RIGHT_PIN = 17;
static const int BUTTON_SLIDER_LEFT_PIN = 21;
static const int BUTTON_SLIDER_RIGHT_PIN = 18;
static DebouncedButton buttonCoffeeLeft(BUTTON_COFFEE_LEFT_PIN, ButtonType::NORMALY_CLOSED, InputType::ENABLE_PULLUP);
static DebouncedButton buttonCoffeeRight(BUTTON_COFFEE_RIGHT_PIN, ButtonType::NORMALY_CLOSED, InputType::ENABLE_PULLUP);
static DebouncedButton buttonSliderLeft(BUTTON_SLIDER_LEFT_PIN, ButtonType::NORMALY_CLOSED, InputType::ENABLE_PULLUP);
static DebouncedButton buttonSliderRight(BUTTON_SLIDER_RIGHT_PIN, ButtonType::NORMALY_CLOSED, InputType::ENABLE_PULLUP);
static SliderButton slider;

void setup(){
    // Serial:
    Serial.begin(115200);
    ESP_LOGI(ESP_LOG_TAG, "start of setup()");

    // ADC:
    Wire.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!adcConverter.begin(true, true, true, false, ADC_FILTER_SIZE)) {
        Serial.println("Failed to initialize ADS.");
        while (1);
    }

    ESP_LOGI(ESP_LOG_TAG, "end of setup()");
}

uint32_t oldMillis;

void loop() {
    adcConverter.update();
    buttonCoffeeLeft.update();
    buttonCoffeeRight.update();
    buttonSliderLeft.update();
    buttonSliderRight.update();
    sensorLightLeft.update(adcConverter, LIGHT_LEFT_CHANNEL);
    sensorLightRight.update(adcConverter, LIGHT_RIGHT_CHANNEL);
    switchWaterLevel.update(adcConverter, WATER_LEVEL_CHANNEL);
    slider.update(buttonSliderLeft.isPressed(), buttonSliderRight.isPressed());

    uint32_t currentMillis = millis();
    if(currentMillis > oldMillis + 100) {
        oldMillis = currentMillis;
        Serial.print("AIN"); 
        Serial.print(" A0="); Serial.print(adcConverter.getNewValue(LIGHT_LEFT_CHANNEL)); 
        Serial.print(" A1="); Serial.print(adcConverter.getNewValue(LIGHT_RIGHT_CHANNEL)); 
        Serial.print(" A2="); Serial.print(adcConverter.getNewValue(WATER_LEVEL_CHANNEL)); 
        Serial.print(" SL="); Serial.print(buttonSliderLeft.isPressed()); 
        Serial.print(" SR="); Serial.print(buttonSliderRight.isPressed()); 

        Serial.print(" LL="); Serial.print(sensorLightLeft.getStateString()); 
        Serial.print(" LR="); Serial.print(sensorLightRight.getStateString()); 
        Serial.print("  S="); Serial.print(slider.getStateAsString());
        Serial.print(" WL="); Serial.print(switchWaterLevel.getState()); 
        Serial.print(" BL="); Serial.print(buttonCoffeeLeft.isPressed()); 
        Serial.print(" BR="); Serial.print(buttonCoffeeRight.isPressed()); 
        Serial.println();
    }
}