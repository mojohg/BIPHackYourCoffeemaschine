#include "debounced_button.h"

DebouncedButton::DebouncedButton(int pin, ButtonType button, InputType input) :
    pin(pin),
    buttonType(button),
    inputType(input),
    debounceDelayMillis(DEFAULT_DEBOUNCE_DELAY_MILLIS),
    releaseDelayMillis(DEFAULT_RELEASE_DELAY_MILLIS),
    state(State::RELEASED)
{
    switch (inputType)
    {
    default:
    case InputType::DEFAULT_INPUT:
            pinMode(pin, INPUT);
        break;
    case InputType::ENABLE_PULLUP:
            pinMode(pin, INPUT_PULLUP);
        break;
    case InputType::ENABLE_PULLDOWN:
            pinMode(pin, INPUT_PULLDOWN);
        break;
    }
    watch.restart();
}
void DebouncedButton::setDelays(uint16_t debounceDelayMillis, uint16_t releaseDelayMillis) {
    this->debounceDelayMillis = debounceDelayMillis;
    this->releaseDelayMillis = releaseDelayMillis;
}
void DebouncedButton::update() {
    bool buttonPressed = isButtonPressed();
    switch (state)
    {
    default:
    case State::RELEASED:
        if(buttonPressed) {
            state = State::DEBOUNCE;
        }
        break;
    case State::DEBOUNCE:
        if(watch.getTimeSinceStart() >= debounceDelayMillis) {
            state = State::PRESSED;
        } else if(!buttonPressed) {
            watch.restart();
            state = State::RELEASED;
        }
        break;
    case State::PRESSED:
        if(!buttonPressed) {
            watch.restart();
            state = State::BLOCKED;
        } 
        break;
    case State::BLOCKED:
        if(watch.getTimeSinceStart() >= releaseDelayMillis) {
            watch.restart();
            state = State::RELEASED;
        }
        break;
    }
}
ButtonType DebouncedButton::getButtonType() const {
    return buttonType;
}
InputType DebouncedButton::getInputType() const {
    return inputType;
}
int DebouncedButton::getPin() const {
    return pin;
}
bool DebouncedButton::isPressed() const {
    return (state == State::PRESSED);
}
////////////////////////////// private:
bool DebouncedButton::isButtonPressed() const {
    switch (buttonType)
    {
    default:
    case ButtonType::NORMALY_OPEN: return digitalRead(pin);
    case ButtonType::NORMALY_CLOSED: return !digitalRead(pin);
    }
}