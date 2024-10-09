#include "slider_button.h"

SliderButton::SliderButton() :
    currentState(State::None)
{
}
void SliderButton::update(bool leftButton, bool rightButton) {
    if(currentState != State::Left && leftButton && !rightButton) {
        currentState = State::Left;
    } else if(currentState != State::Right && !leftButton && rightButton) {
        currentState = State::Right;
    }
}
SliderButton::State SliderButton::getState() const {
    return currentState;
}
String SliderButton::getStateAsString() const {
    switch (currentState)
    {
    default:
    case State::None: return String("None");
    case State::Left: return String("Left");
    case State::Right: return String("Right");
    }
}