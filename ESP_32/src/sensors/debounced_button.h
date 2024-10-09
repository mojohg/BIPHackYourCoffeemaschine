#pragma once
#include <Arduino.h>
#include "arduino_stopwatch.h"

using namespace ArduinoStopwatch;

enum class ButtonType {
    NORMALY_OPEN,
    NORMALY_CLOSED
};
enum class InputType {
    DEFAULT_INPUT,
    ENABLE_PULLUP,
    ENABLE_PULLDOWN
};

class DebouncedButton {
public:
    const uint16_t DEFAULT_DEBOUNCE_DELAY_MILLIS = 50;
    const uint16_t DEFAULT_RELEASE_DELAY_MILLIS = 100;
public:
    DebouncedButton(int pin, ButtonType button=ButtonType::NORMALY_OPEN, InputType input=InputType::DEFAULT_INPUT);
    void setDelays(uint16_t debounceDelayMillis, uint16_t releaseDelayMillis);
    void update();
    ButtonType getButtonType() const;
    InputType getInputType() const;
    int getPin() const;
    bool isPressed() const;
private:
    bool isButtonPressed() const;
private:
    enum class State {RELEASED, DEBOUNCE, PRESSED, BLOCKED};
    Stopwatch16MS watch;
    int pin;
    ButtonType buttonType;
    InputType inputType;
    uint16_t debounceDelayMillis;
    uint16_t releaseDelayMillis;
    State state;
};