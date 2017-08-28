#include <pebble.h>
#include "logging.h"
#include "hour-layer.h"
#include "minute-layer.h"
#include "battery-layer.h"
#include "date-layer.h"

static Window *s_window;
static HourLayer *s_hour_layer;
static MinuteLayer *s_minute_layer;
static BatteryLayer *s_battery_layer;
static DateLayer *s_date_layer;

static void prv_window_load(Window *window) {
    logf();
    Layer *root_layer = window_get_root_layer(window);

    s_hour_layer = hour_layer_create(GPoint(86, 0));
    layer_add_child(root_layer, s_hour_layer);

    s_minute_layer = minute_layer_create(GPoint(126, 10));
    layer_add_child(root_layer, s_minute_layer);

    s_battery_layer = battery_layer_create(GRect(12, 100, PBL_DISPLAY_WIDTH - 24, 2));
    layer_add_child(root_layer, s_battery_layer);

    s_date_layer = date_layer_create(GPoint(0, 120));
    layer_add_child(root_layer, s_date_layer);

    window_set_background_color(window, GColorBlack);
}

static void prv_window_unload(Window *window) {
    logf();
    date_layer_destroy(s_date_layer);
    battery_layer_destroy(s_battery_layer);
    minute_layer_destroy(s_minute_layer);
    hour_layer_destroy(s_hour_layer);
}

static void prv_init(void) {
    logf();
    s_window = window_create();
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = prv_window_load,
        .unload = prv_window_unload
    });
    window_stack_push(s_window, true);
}

static void prv_deinit(void) {
    logf();
    window_destroy(s_window);
}

int main(void) {
    logf();
    prv_init();
    app_event_loop();
    prv_deinit();
}
