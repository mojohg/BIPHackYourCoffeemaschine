#include "ui_point.h"

UiPoint::UiPoint(s16_t p_x, s16_t p_y) :
    x(p_x),
    y(p_y)
{

}
void UiPoint::swap_axis() {
    s16_t temp = x;
    x = y;
    y = temp;
}
UiPoint& UiPoint::operator+=(const UiPoint& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}
UiPoint& UiPoint::operator-=(const UiPoint& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}
s16_t UiPoint::distance_to_zero() const {
    return sqrt(x*x + y*y);
}
s16_t UiPoint::distance_to(const UiPoint& rhs) const {
    return distance(*this, rhs);
}
String UiPoint::to_string() const {
    return (String(x) + "," + String(y));
}
UiPoint operator+(UiPoint lhs, const UiPoint& rhs) {
    return UiPoint(lhs.x + rhs.x, lhs.y + rhs.y);
}
UiPoint operator-(UiPoint lhs, const UiPoint& rhs) {
    return UiPoint(lhs.x - rhs.x, lhs.y - rhs.y);
}
s16_t UiPoint::distance(UiPoint lhs, const UiPoint& rhs) {
    return (lhs - rhs).distance_to_zero();
}