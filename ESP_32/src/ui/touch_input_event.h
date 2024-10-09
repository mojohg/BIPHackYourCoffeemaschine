#pragma once
#include <Arduino.h>
#include "ui_point.h"

enum class EventType {
    CLICK,
    SWIPE_UP,
    SWIPE_DOWN,
    SWIPE_LEFT,
    SWIPE_RIGHT,
    UP,
    DOWN,
    MOVE
};

enum class InputEventState {
    PENDING,
    HANDLED
};

class TouchInputEvent {
public:
    TouchInputEvent();
    TouchInputEvent(EventType p_type, UiPoint p_location);
    bool is_handled() const;
    void handle();
    EventType get_type() const;
    String get_type_as_string() const;
    UiPoint get_location() const;
private:
    EventType m_type;
    UiPoint m_location;
    InputEventState m_state;
};

using TouchInputEventCallback = void(*)(TouchInputEvent& event);