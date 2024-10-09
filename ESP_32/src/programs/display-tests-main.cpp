#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include "ui/touch_handler_cst816s.h"
#include "esp_log.h"
#include "ui/ui_display.h"
#include "ui/ui_button_circular.h"

const char* ESP_LOG_TAG = "ESP";

TFT_eSPI tftDisplay = TFT_eSPI();
const int TOUCH_SDA_PIN = 6;
const int TOUCH_SCL_PIN = 7;
const int TOUCH_RST_PIN = 13;
const int TOUCH_IRQ_PIN = 5;
TouchHandlerCst816s touchHandler(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_RST_PIN, TOUCH_IRQ_PIN);
UiButtonCircular waterFilledButton(&tftDisplay);

void handleTouchEvents(TouchInputEvent& event) {
    waterFilledButton.handleTouchEvents(event);
    switch (event.get_type())
    {
    case EventType::CLICK:
        Serial.print("Click: x,y=");
        Serial.println(event.get_location().to_string());
        tftDisplay.fillCircle(event.get_location().x, event.get_location().y, 10, TFT_RED);
        break;
    case EventType::SWIPE_UP:
        Serial.print("SWIPE-UP: x,y=");
        Serial.println(event.get_location().to_string());
        for(s16_t y = DISPLAY_MAX_Y; y > DISPLAY_MIN_Y; y--) {
            tftDisplay.drawLine(DISPLAY_MIN_X, y, DISPLAY_MAX_X, y, TFT_GREY);
        }
        break;
    case EventType::SWIPE_DOWN:
        Serial.print("SWIPE-DOWN: x,y=");
        Serial.println(event.get_location().to_string());
        break;
    case EventType::SWIPE_LEFT:
        Serial.print("SWIPE-LEFT: x,y=");
        Serial.println(event.get_location().to_string());
        break;
    case EventType::SWIPE_RIGHT:
        Serial.print("SWIPE-RIGHT: x,y=");
        Serial.println(event.get_location().to_string());
        break;
    // case EventType::UP:
    //     Serial.print("UP: x,y=");
    //     Serial.println(event.get_location().to_string());
    //     break;
    // case EventType::DOWN:
    //     Serial.print("DOWN: x,y=");
    //     Serial.println(event.get_location().to_string());
    //     break;
    // case EventType::MOVE:
    //     Serial.print("MOVE: x,y=");
    //     Serial.println(event.get_location().to_string());
    //     break;
    }
    event.handle();
}

//////////// start clock:
uint32_t targetTime = 0;
uint8_t hh = 12, mm = 28, ss = 16;
byte omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
unsigned int colour = 0;

void setup(){
    // Serial:
    Serial.begin(115200);
    ESP_LOGI(ESP_LOG_TAG, "start of setup()");

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

    // buttons:
    waterFilledButton.setDimensions(UiPoint(60,60), 40);
    waterFilledButton.setText("Filled", TFT_WHITE);

    // clock:
    targetTime = millis() + 1000;
    ESP_LOGI(ESP_LOG_TAG, "end of setup()");
}

void loop() {
    touchHandler.handleEvents();
    waterFilledButton.update();
    // clock:
    if (targetTime < millis()) {
        // Set next update for 1 second later
        targetTime = millis() + 1000;

        // Adjust the time values by adding 1 second
        ss++;              // Advance second
        if (ss == 60) {    // Check for roll-over
            ss = 0;          // Reset seconds to zero
            omm = mm;        // Save last minute time for display update
            mm++;            // Advance minute
            if (mm > 59) {   // Check for roll-over
                mm = 0;
                hh++;          // Advance hour
                if (hh > 23) { // Check for 24hr roll-over (could roll-over on 13)
                hh = 0;      // 0 for 24 hour clock, set to 1 for 12 hour clock
                }
            }
        }
        tftDisplay.drawCircle(0, 0, 119, TFT_YELLOW);

        // touch calibartion:
        // X: 188 - 64 = 124 mapped to 120 | center = 126
        // Y: 187 - 65 = 122 mapped to 120 | center = 126
        // tftDisplay.fillCircle(60, 60, 10, TFT_GREEN);    // 189 |  68  
        tftDisplay.fillCircle(60, -60, 10, TFT_ORANGE);   // 186 | 185
        tftDisplay.fillCircle(-60, 60, 10, TFT_BLUE);   //  61 |  62
        tftDisplay.fillCircle(-60, -60, 10, TFT_PURPLE);  //  67 | 189

        uint8_t font = 6;
        // Update digital time
        int xpos = -100;
        int ypos = -20; // Top left corner ot clock text, about half way down
        int ysecs = ypos;

        if (omm != mm) { // Redraw hours and minutes time every minute
            omm = mm;
            // Draw hours and minutes
            if (hh < 10) xpos += tftDisplay.drawChar('0', xpos, ypos, font); // Add hours leading zero for 24 hr clock
            xpos += tftDisplay.drawNumber(hh, xpos, ypos, font);             // Draw hours
            xcolon = xpos; // Save colon coord for later to flash on/off later
            xpos += tftDisplay.drawChar(':', xpos, ypos, font);
            if (mm < 10) xpos += tftDisplay.drawChar('0', xpos, ypos, font); // Add minutes leading zero
            xpos += tftDisplay.drawNumber(mm, xpos, ypos, font);             // Draw minutes
            xsecs = xpos; // Sae seconds 'x' position for later display updates
        }
        if (oss != ss) { // Redraw seconds time every second
            oss = ss;
            xpos = xsecs;

            if (ss % 2) { // Flash the colons on/off
                tftDisplay.setTextColor(0x39C4, TFT_BLACK);        // Set colour to grey to dim colon
                tftDisplay.drawChar(':', xcolon, ypos, font);     // Hour:minute colon
                xpos += tftDisplay.drawChar(':', xsecs, ysecs, font); // Seconds colon
                tftDisplay.setTextColor(TFT_YELLOW, TFT_BLACK);    // Set colour back to yellow
            }
            else {
                tftDisplay.drawChar(':', xcolon, ypos, font);     // Hour:minute colon
                xpos += tftDisplay.drawChar(':', xsecs, ysecs, font); // Seconds colon
            }

            //Draw seconds
            if (ss < 10) xpos += tftDisplay.drawChar('0', xpos, ysecs, font); // Add leading zero
            tftDisplay.drawNumber(ss, xpos, ysecs, font);                     // Draw seconds
        }
    }
}