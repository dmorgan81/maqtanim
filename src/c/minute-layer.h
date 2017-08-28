#pragma once
#include <pebble.h>

typedef Layer MinuteLayer;

MinuteLayer *minute_layer_create(GPoint origin);
void minute_layer_destroy(MinuteLayer *this);
