#include "ui_button_circular.h"

UiButtonCircular::UiButtonCircular(TFT_eSPI* p_display, UiPoint p_center, int16_t p_radius) :
    display(p_display),
    centerPoint(p_center),
    radius(p_radius)
{

}
void UiButtonCircular::draw() {
    // background:
    display->fillCircle(centerPoint.x, centerPoint.y, radius, backgroundColor); 
    // border:
    drawBorder();
    // text:
    int16_t width = display->textWidth(text, font);
    int16_t height = display->fontHeight(font);
    display->setTextColor(foregroundColor, backgroundColor);  
    display->setTextFont(font); 
    display->drawString(text, centerPoint.x-width/2, centerPoint.y-height/2);
}
void UiButtonCircular::drawBorder() {
    // border:
    if(pressed) {
        display->drawCircle(centerPoint.x, centerPoint.y, radius, pressedBorderColor);
    } else {
        display->drawCircle(centerPoint.x, centerPoint.y, radius, borderColor);
    }
}
void UiButtonCircular::setDimensions(UiPoint center, int16_t radius) {
    this->centerPoint = center;
    this->radius = radius;
    draw();
}
void UiButtonCircular::setText(String text, uint16_t color, uint8_t font) {
    this->text = text;
    this->foregroundColor = color;
    this->font = font;
    draw();
}
void UiButtonCircular::setColors(uint16_t background, uint16_t border, uint16_t borderPressed) {
    this->backgroundColor = background;
    this->borderColor = border;
    this->pressedBorderColor = borderPressed;
    draw();
}
void UiButtonCircular::update() {
    if(pressed && pressedWatch.getTimeSinceStart() > 1500) {
        pressed = false;
        draw();
    }
}
void UiButtonCircular::handleTouchEvents(TouchInputEvent& event) {
    if(event.get_location().distance_to(centerPoint) > radius) {
        return;
    }
    if(event.get_type() == EventType::CLICK) {
        pressed = true;
        pressedWatch.restart();
        draw();
    }
}
bool UiButtonCircular::isPressed() {
    return pressed;
}