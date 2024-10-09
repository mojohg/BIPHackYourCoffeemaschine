#pragma once
#include <Arduino.h>
#include <CST816S.h>
#include "ui_point.h"
#include "touch_input_event.h"
#include "arduino_stopwatch.h"

using ArduinoStopwatch::Stopwatch32MS;

using IsOnScreenCallback = bool(*)(UiPoint position);

class TouchHandlerCst816s {
public:
    TouchHandlerCst816s(
        int p_sda_pin, 
        int p_scl_pin, 
        int p_reset_pin, 
        int p_interrupt_pin
    );
    void set_swap_axis(bool p_swap);
    void set_invert_x_axis(bool p_invert);
    void set_invert_y_axis(bool p_invert);
    void set_x_axis_limits(s16_t p_min_value, s16_t p_max_value);
    void set_y_axis_limits(s16_t p_min_value, s16_t p_max_value);
    bool begin();
    void handleEvents();
    TouchInputEventCallback handle_event_callback;
    IsOnScreenCallback is_on_screen_callback;
private:
    static const u32_t MAX_SWIPE_MILLIS = 3000;
    static const s16_t MIN_SWIPE_DISTANCE = 60;
    static const s16_t MAX_CLICK_DISTANCE = 40;
    static const u32_t MIN_CLICK_MILLIS = 20;
    static const u32_t MAX_CLICK_MILLIS = 1000;
    bool m_swap_axis;
    bool m_invert_x_axis;
    bool m_invert_y_axis;
    s16_t m_x_min_value;
    s16_t m_x_max_value;
    s16_t m_y_min_value;
    s16_t m_y_max_value;    
    CST816S m_touch_screen;
    UiPoint m_down_position;
    bool m_is_down;
    Stopwatch32MS m_down_watch;
    UiPoint calculate_touch_position(int p_raw_x, int p_raw_y) const;
};