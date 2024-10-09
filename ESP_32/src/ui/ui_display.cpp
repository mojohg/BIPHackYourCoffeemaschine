#include "ui_display.h"

UiPoint toDisplayCoordinates(UiPoint p) {
    return UiPoint(p.x-DISPLAY_OFFSET_X, p.y-DISPLAY_OFFSET_Y);
}

UiPoint toCenterCoordinates(UiPoint p) {
    return UiPoint(p.x+DISPLAY_OFFSET_X, p.y+DISPLAY_OFFSET_Y);
}