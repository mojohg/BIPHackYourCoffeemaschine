#pragma once
#include <Arduino.h>
#include "ui_point.h"
#include <TFT_eSPI.h>
#include "ui_display.h"
#include "touch_input_event.h"
#include <arduino_stopwatch.h>

using namespace ArduinoStopwatch;


class UiButtonCircular {
public:
    UiButtonCircular(TFT_eSPI* p_display, UiPoint p_center, int16_t p_radius);
    void draw();
    void drawBorder();
    void setDimensions(UiPoint center, int16_t radius);
    void setText(String text, uint16_t color, uint8_t font=4);
    void setColors(uint16_t background, uint16_t border, uint16_t borderPressed);
    void update();
    void handleTouchEvents(TouchInputEvent& event);
    bool isPressed();
private:
    TFT_eSPI* display;
    UiPoint centerPoint = UiPoint(0,0);
    int16_t radius = 30;
    uint16_t foregroundColor = TFT_WHITE;
    uint16_t backgroundColor = TFT_BLACK;
    uint16_t borderColor = TFT_GREY;
    uint16_t pressedBorderColor = TFT_BLUE;
    String text = "";
    uint8_t font = 4;
    bool pressed = false;
    Stopwatch16MS pressedWatch;
};