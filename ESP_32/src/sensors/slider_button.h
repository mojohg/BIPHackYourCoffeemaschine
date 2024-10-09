#pragma once
#include <Arduino.h>



class SliderButton {
public:
    enum class State {None, Left, Right};
    SliderButton();
    void update(bool leftButton, bool rightButton);
    State getState() const;
    String getStateAsString() const;
private:
    State currentState;
};
