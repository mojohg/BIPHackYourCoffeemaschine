#include <Arduino.h>
#include <ArduinoJson.h>
#include <arduino_stopwatch.h>
#include "esp_log.h"
#include "wifiCredentials.h"
#include "wifi/wifi_handler.h"
#include "mqtt/mqtt_handler.h"
#include "ntp/ntp_handler.h"
#include "sensors/ads1115_continuous.h"
#include "sensors/debounced_button.h"
#include "sensors/ldr_blink_sensor.h"
#include "sensors/threshold_sensor.h"
#include "sensors/slider_button.h"
#include <TFT_eSPI.h>
#include <SPI.h>
#include "ui/touch_handler_cst816s.h"
#include "ui/ui_display.h"
#include "ui/ui_button_circular.h"

using ArduinoStopwatch::Stopwatch32MS;

// logging:
static const char* ESP_LOG_TAG = "ESP";
// wifi:
static WiFiHandler wifiHandler("IOT-Coffeemaker10");
// mqtt:
static MqttHandler mqttHandler("IOT-Coffeemaker10");
static const IPAddress MQTT_BROKER_IP(192, 168, 178, 21); // broker on the BaristaBandwidth network
static const u16_t MQTT_BROKER_PORT = 1883;
Stopwatch32MS publishWatch;
static const u32_t PUBLISH_INTERVAL_MILLIS = 200;
static const char* PUBLISH_TOPIC = "bipfinnland/ESP10/State";
static const char* SUBSCRIBE_TOPIC = "bipfinnland/Predict10/RemainingCups";
static JsonDocument sub_jsonDoc;
void mqttCallback(char* topic, byte* payload, unsigned int length) {    
    String s;
    for (int i=0;i<length;i++) {
        s += (char)payload[i];
    }
    deserializeJson(sub_jsonDoc, s);
}
// json:
static JsonDocument jsonDoc;
// NTP:
static const IPAddress NTP_SERVER_IP(192, 168, 178, 21);
static const u16_t NTP_UPDATE_INTERVAL_MILLIS = 60000;
static const u32_t TIMEZONE_OFFSET = 7200;
// NtpHandler ntpHandler(NTP_SERVER_IP, NTP_UPDATE_INTERVAL_MILLIS, TIMEZONE_OFFSET);
// ADC:
static const int I2C_SCL_PIN = 15;
static const int I2C_SDA_PIN = 16;
static const u8_t ADC_FILTER_SIZE = 4;
TwoWire i2c0 = TwoWire(1);
static Ads1115Continuous adcConverter(&i2c0);
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

// Display:
TFT_eSPI tftDisplay = TFT_eSPI();
const int TOUCH_SDA_PIN = 6;
const int TOUCH_SCL_PIN = 7;
const int TOUCH_RST_PIN = 13;
const int TOUCH_IRQ_PIN = 5;
TouchHandlerCst816s touchHandler(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_RST_PIN, TOUCH_IRQ_PIN);
UiButtonCircular waterFilledButton(&tftDisplay, UiPoint(-45,37), 40);
UiButtonCircular padButton(&tftDisplay, UiPoint(45,35), 40);
void handleTouchEvents(TouchInputEvent& event) {
    waterFilledButton.handleTouchEvents(event);
    padButton.handleTouchEvents(event);
    event.handle();
}

void drawCup(UiPoint position, int16_t fillHight) {
    int16_t length = 10;
    int16_t handleLength = 5;
    UiPoint topLeft(-length,-length);
    UiPoint bottomLeft(-length,length);
    UiPoint bottomRight(length,length); 
    UiPoint topRight(length,-length);
    UiPoint handleTopLeft(length,handleLength);
    UiPoint handleBottomLeft(length,-handleLength);
    UiPoint handleBottomRight(length+handleLength*2,-handleLength); 
    UiPoint handleTopRight(length+handleLength*2,handleLength);
    UiPoint fillTopLeft(-(length-2),(length-2)-fillHight);
    UiPoint fillSize((length-2)*2,fillHight);

    tftDisplay.drawLine(topLeft.x+position.x, topLeft.y+position.y, bottomLeft.x+position.x, bottomLeft.y+position.y, TFT_WHITE);
    tftDisplay.drawLine(bottomLeft.x+position.x, bottomLeft.y+position.y, bottomRight.x+position.x, bottomRight.y+position.y, TFT_WHITE);
    tftDisplay.drawLine(bottomRight.x+position.x, bottomRight.y+position.y, topRight.x+position.x, topRight.y+position.y, TFT_WHITE);

    tftDisplay.drawLine(handleTopLeft.x+position.x, handleTopLeft.y+position.y, handleTopRight.x+position.x, handleTopRight.y+position.y, TFT_WHITE);
    tftDisplay.drawLine(handleTopRight.x+position.x, handleTopRight.y+position.y, handleBottomRight.x+position.x, handleBottomRight.y+position.y, TFT_WHITE);
    tftDisplay.drawLine(handleBottomRight.x+position.x, handleBottomRight.y+position.y, handleBottomLeft.x+position.x, handleBottomLeft.y+position.y, TFT_WHITE);
    tftDisplay.fillRect(fillTopLeft.x+position.x, fillTopLeft.y+position.y, fillSize.x, fillSize.y, TFT_WHITE);
}

void drawCoffeeText(int16_t small, int16_t large, int16_t smallAddittional) {
    // first line:
    tftDisplay.setTextColor(TFT_WHITE, TFT_BLACK); 
    tftDisplay.setTextFont(4); 
    tftDisplay.drawString(" " + String(small) + "x", -60, -80);
    drawCup(UiPoint(0, -70), 10);
    tftDisplay.setTextColor(TFT_WHITE, TFT_BLACK); 
    tftDisplay.setTextFont(4); 
    tftDisplay.drawString("or", 35, -80);
    // second line:
    tftDisplay.setTextColor(TFT_WHITE, TFT_BLACK); 
    tftDisplay.setTextFont(4); 
    tftDisplay.drawString(" " + String(large) + "x", -80, -40);
    drawCup(UiPoint(-30, -30), 20);
    tftDisplay.setTextColor(TFT_WHITE, TFT_BLACK); 
    tftDisplay.setTextFont(4); 
    tftDisplay.drawString("& " + String(smallAddittional) + "x", 0, -40);
    drawCup(UiPoint(70, -30), 10);
}

void setup(){
    // Serial:
    Serial.begin(115200);
    ESP_LOGI(ESP_LOG_TAG, "start of setup()");
    // ADC:
    i2c0.setPins(I2C_SDA_PIN, I2C_SCL_PIN);
    if (!adcConverter.begin(true, true, true, false, ADC_FILTER_SIZE)) {
        ESP_LOGE(ESP_LOG_TAG, "Failed to initialize ADS1115.");
        delay(5000);
        exit(0);
    }
    // Display:
    tftDisplay.init();
    tftDisplay.setRotation(1);
    tftDisplay.fillScreen(TFT_BLACK);
    tftDisplay.setTextSize(1);
    tftDisplay.setTextColor(TFT_YELLOW, TFT_BLACK);
    tftDisplay.setOrigin(120, 120);
    // touch panel:
    touchHandler.set_swap_axis(true);
    touchHandler.set_invert_x_axis(true);
    touchHandler.set_x_axis_limits(DISPLAY_MIN_X, DISPLAY_MAX_X);
    touchHandler.set_y_axis_limits(DISPLAY_MIN_Y, DISPLAY_MAX_Y);
    touchHandler.handle_event_callback = handleTouchEvents;
    touchHandler.begin();
    // drawing:
    tftDisplay.drawCircle(0, 0, DISPLAY_RADIUS, TFT_BLUE);
    waterFilledButton.setColors(TFT_SKYBLUE, TFT_GREY, TFT_BLUE);
    waterFilledButton.setText("Filled", TFT_WHITE);
    padButton.setColors(TFT_BROWN, TFT_GREY, TFT_BLUE);
    padButton.setText("Pad", TFT_WHITE);
    drawCoffeeText(0,0,0);
    // WiFi:
    if(!wifiHandler.connect(WIFI_SSID, WIFI_PWD)) {
        delay(5000);
        exit(0);
    }
    // NTP:
    // if(!ntpHandler.begin(10000)) {
    //     delay(5000);
    //     exit(0);
    // }
    // Mqtt:
    if(!mqttHandler.connect(MQTT_BROKER_IP, MQTT_BROKER_PORT, mqttCallback)) {
        // no error
    }
    mqttHandler.subscribe(SUBSCRIBE_TOPIC);
    publishWatch.restart();
    // Json:
    jsonDoc["ButtonCoffeeLeft"] = false;
    jsonDoc["ButtonCoffeeRight"] = false;
    jsonDoc["SliderPosition"] = "None";
    jsonDoc["LightLeft"] = "OFF";
    jsonDoc["LightRight"] = "OFF";
    jsonDoc["WaterSwitch"] = false;
    jsonDoc["Refilled"] = false;
    jsonDoc["NewPad"] = false;
    jsonDoc["Timestamp"] = "00:00:00";//ntpHandler.getFormattedTime();
    //
    ESP_LOGI(ESP_LOG_TAG, "end of setup()");
}

bool buttonLeftFlag = false;
bool buttonRightFlag = false;
bool buttonRefilledFlag = false;
bool buttonNewPadFlag = false;

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
    mqttHandler.update();
    // ntpHandler.update();
    touchHandler.handleEvents();
    waterFilledButton.update();
    padButton.update();
    if(buttonCoffeeLeft.isPressed()) {
        buttonLeftFlag = true;
    }
    if(buttonCoffeeRight.isPressed()) {
        buttonRightFlag = true;
    }
    if(waterFilledButton.isPressed()) {
        buttonRefilledFlag = true;
    }
    if(padButton.isPressed()) {
        buttonNewPadFlag = true;
    }

    if (publishWatch.getTimeSinceStart() >= PUBLISH_INTERVAL_MILLIS) {
        int small = sub_jsonDoc["small cups"];
        int large = sub_jsonDoc["large cups"];
        int largeAdditional = sub_jsonDoc["small cups remaining"];
        drawCoffeeText(small,large,largeAdditional);
        //////////////
        ESP_LOGI(ESP_LOG_TAG, "Cycletime: %ims", publishWatch.getTimeSinceStart());
        publishWatch.restart();
        // update jsonDoc:
        jsonDoc["ButtonCoffeeLeft"] = buttonLeftFlag;
        jsonDoc["ButtonCoffeeRight"] = buttonRightFlag;
        jsonDoc["SliderPosition"] = slider.getStateAsString();
        jsonDoc["LightLeft"] = sensorLightLeft.getStateString();
        jsonDoc["LightRight"] = sensorLightRight.getStateString();
        jsonDoc["WaterSwitch"] = switchWaterLevel.getState();
        jsonDoc["Refilled"] = buttonRefilledFlag;
        jsonDoc["NewPad"] = buttonNewPadFlag;
        jsonDoc["Timestamp"] = "00:00:00"; //ntpHandler.getFormattedTime();
        // publish:
        mqttHandler.publish(PUBLISH_TOPIC, jsonDoc);

        buttonLeftFlag = false;
        buttonRightFlag = false;
        buttonRefilledFlag = false;
        buttonNewPadFlag = false;
    }
}