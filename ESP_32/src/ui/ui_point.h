#pragma once
#include <Arduino.h>

//    +-⇾ X
//    ↓
//    Y

class UiPoint {
public:
    UiPoint(s16_t p_x=0, s16_t p_y=0);
    void swap_axis();
    UiPoint& operator+=(const UiPoint& rhs);
    UiPoint& operator-=(const UiPoint& rhs);
    s16_t distance_to_zero() const;
    s16_t distance_to(const UiPoint& rhs) const;
    String to_string() const;
    s16_t x;
    s16_t y;
    friend UiPoint operator+(UiPoint lhs, const UiPoint& rhs);
    friend UiPoint operator-(UiPoint lhs, const UiPoint& rhs);
    static s16_t distance(UiPoint lhs, const UiPoint& rhs);
};