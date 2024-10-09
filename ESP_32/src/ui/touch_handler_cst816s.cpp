#include "touch_handler_cst816s.h"

TouchHandlerCst816s::TouchHandlerCst816s(
    int p_sda_pin, 
    int p_scl_pin, 
    int p_reset_pin, 
    int p_interrupt_pin
) :
    m_swap_axis(false),
    m_invert_x_axis(false),
    m_invert_y_axis(false),
    m_x_min_value(0),
    m_x_max_value(255),
    m_y_min_value(0),
    m_y_max_value(255), 
    m_touch_screen(p_sda_pin, p_scl_pin, p_reset_pin, p_interrupt_pin)
{

}
void TouchHandlerCst816s::set_swap_axis(bool p_swap) {
    m_swap_axis = p_swap;
}
void TouchHandlerCst816s::set_invert_x_axis(bool p_invert) {
    m_invert_x_axis = p_invert;
}
void TouchHandlerCst816s::set_invert_y_axis(bool p_invert) {
    m_invert_y_axis = p_invert;
}
void TouchHandlerCst816s::set_x_axis_limits(s16_t p_min_value, s16_t p_max_value) {
    m_x_min_value = p_min_value;
    m_x_max_value = p_max_value;
}
void TouchHandlerCst816s::set_y_axis_limits(s16_t p_min_value, s16_t p_max_value) {
    m_y_min_value = p_min_value;
    m_y_max_value = p_max_value;
}
bool TouchHandlerCst816s::begin() {
    m_touch_screen.begin();
    return true;
}
void TouchHandlerCst816s::handleEvents() {
    if (handle_event_callback == nullptr) {
        return;
    }
    if (!m_touch_screen.available()) {
        return;
    }
    if (m_touch_screen.data.gestureID != GESTURE::NONE) {
        return;
    }
    UiPoint position = calculate_touch_position(m_touch_screen.data.x, m_touch_screen.data.y);
    if(is_on_screen_callback != nullptr && !is_on_screen_callback(position)) {
        return;
    }
    UiPoint down_difference = position - m_down_position;
    // DOWN:
    if(m_touch_screen.data.event == 0) {
        m_is_down = true;
        m_down_position = position;
        m_down_watch.restart();
        TouchInputEvent event(EventType::DOWN, position);
        handle_event_callback(event);    
        return;
    } 
    // MOVE:
    if(m_touch_screen.data.event == 2) {
        TouchInputEvent event(EventType::MOVE, position);
        handle_event_callback(event);
        return;
    }
    // SWIPE:
    if(
        m_is_down && 
        m_touch_screen.data.event == 1 && 
        m_down_watch.getTimeSinceStart() <= MAX_SWIPE_MILLIS &&
        down_difference.distance_to_zero() >= MIN_SWIPE_DISTANCE
    ) {
        if(abs(down_difference.x) >= 2*abs(down_difference.y)) { // left - right:
            EventType type = (down_difference.x > 0) ? EventType::SWIPE_RIGHT : EventType::SWIPE_LEFT;
            TouchInputEvent event(type, m_down_position);
            handle_event_callback(event);
        } else if(abs(down_difference.y) >= 2*abs(down_difference.x)) { // up - down:
            EventType type = (down_difference.y > 0) ? EventType::SWIPE_DOWN : EventType::SWIPE_UP;
            TouchInputEvent event(type, m_down_position);
            handle_event_callback(event);
        }
    }
    // CLICK:
    if(
        m_is_down && 
        m_touch_screen.data.event == 1 && 
        down_difference.distance_to_zero() <= MAX_CLICK_DISTANCE &&
        m_down_watch.getTimeSinceStart() >= MIN_CLICK_MILLIS &&
        m_down_watch.getTimeSinceStart() <= MAX_CLICK_MILLIS
    ) {
        TouchInputEvent event(EventType::CLICK, m_down_position);
        handle_event_callback(event);
    }   
    // UP:
    if(m_touch_screen.data.event == 1) {
        m_is_down = false;
        TouchInputEvent event(EventType::UP, position);
        handle_event_callback(event);
    } 
}
//////////////////////// private:
UiPoint TouchHandlerCst816s::calculate_touch_position(int p_raw_x, int p_raw_y) const {
    UiPoint position(p_raw_x, p_raw_y);
    if(m_swap_axis) {
        position.swap_axis();
    }
    if(m_invert_x_axis) {
        position.x = map(position.x, 0, 250, m_x_min_value, m_x_max_value);
    } else {
        position.x = map(position.x, 0, 250, m_x_max_value, m_x_min_value);
    }
    if(m_invert_y_axis) {
        position.y = map(position.y, 0, 250, m_y_min_value, m_y_max_value);
    } else {
        position.y = map(position.y, 0, 250, m_y_max_value, m_y_min_value);
    } 
    return position;
}
