#include "touch_input_event.h"

TouchInputEvent::TouchInputEvent() : 
    m_state(InputEventState::HANDLED)
{

}
TouchInputEvent::TouchInputEvent(EventType p_type, UiPoint p_location)  : 
    m_type(p_type),
    m_location(p_location),
    m_state(InputEventState::PENDING)
{
    
}
bool TouchInputEvent::is_handled() const {
    return (m_state == InputEventState::HANDLED);
}
void TouchInputEvent::handle() {
    m_state = InputEventState::HANDLED;
}
EventType TouchInputEvent::get_type() const {
    return m_type;
}
String TouchInputEvent::get_type_as_string() const {
    switch (m_type)
    {
    case EventType::CLICK:  return "CLICK";
    case EventType::SWIPE_UP: return "SWIPE UP";
    case EventType::SWIPE_DOWN: return "SWIPE DOWN";
    case EventType::SWIPE_LEFT: return "SWIPE LEFT";
    case EventType::SWIPE_RIGHT: return "SWIPE RIGHT";
    case EventType::UP: return "UP";
    case EventType::DOWN: return "DOWN";
    case EventType::MOVE: return "MOVE";
    default: return "UNKNOWN";
    }
}
UiPoint TouchInputEvent::get_location() const {
    return m_location;
}