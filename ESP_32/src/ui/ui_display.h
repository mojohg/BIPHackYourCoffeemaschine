#pragma once
#include <Arduino.h>
#include "ui_point.h"

#define TFT_GREY 0x5AEB

// DISPLAY SIZE:            240x240 pixels
const s16_t DISPLAY_WIDTH = 240;
const s16_t DISPLAY_HEIGHT = 240;
// CENTER PIXEL:            120x120
const s16_t DISPLAY_OFFSET_X = 120;
const s16_t DISPLAY_OFFSET_Y = 120;
// ROWS FROM CENTER:        -120(top)  +119(bottom)
const s16_t DISPLAY_MIN_Y = -120;
const s16_t DISPLAY_MAX_Y = 119;
// COLUMNS FROM CENTER:     -120(left) +119(right)
const s16_t DISPLAY_MIN_X = -120;
const s16_t DISPLAY_MAX_X = 119;
// RADIUS:
const s16_t DISPLAY_RADIUS = 119;

UiPoint toDisplayCoordinates(UiPoint p);
UiPoint toCenterCoordinates(UiPoint p);