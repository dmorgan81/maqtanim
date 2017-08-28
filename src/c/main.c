#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include <pebble-hourly-vibes/hourly-vibes.h>
#include <pebble-connection-vibes/connection-vibes.h>
#include "enamel.h"
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

static EventHandle s_settings_received_event_handle;

static void prv_settings_received_handler(void *context) {
    logf();
    hourly_vibes_set_enabled(enamel_get_HOURLY_VIBE());
    connection_vibes_set_state(atoi(enamel_get_CONNECTION_VIBE()));
#ifdef PBL_HEALTH
    connection_vibes_enable_health(enamel_get_ENABLE_HEALTH());
    hourly_vibes_enable_health(enamel_get_ENABLE_HEALTH());
#endif
}

static void prv_window_load(Window *window) {
    logf();
    Layer *root_layer = window_get_root_layer(window);

    s_hour_layer = hour_layer_create(GPoint(90, 35));
    layer_add_child(root_layer, s_hour_layer);

    s_minute_layer = minute_layer_create(GPoint(122, 45));
    layer_add_child(root_layer, s_minute_layer);

    s_battery_layer = battery_layer_create(GRect(12, 100, PBL_DISPLAY_WIDTH - 24, 2));
    layer_add_child(root_layer, s_battery_layer);

    s_date_layer = date_layer_create(GPoint(0, 110));
    layer_add_child(root_layer, s_date_layer);

    prv_settings_received_handler(NULL);
    s_settings_received_event_handle = enamel_settings_received_subscribe(prv_settings_received_handler, NULL);

    window_set_background_color(window, GColorBlack);
}

static void prv_window_unload(Window *window) {
    logf();
    enamel_settings_received_unsubscribe(s_settings_received_event_handle);

    date_layer_destroy(s_date_layer);
    battery_layer_destroy(s_battery_layer);
    minute_layer_destroy(s_minute_layer);
    hour_layer_destroy(s_hour_layer);
}

static void prv_init(void) {
    logf();
    enamel_init();
    connection_vibes_init();
    hourly_vibes_init();
    uint32_t const pattern[] = { 100 };
    hourly_vibes_set_pattern((VibePattern) {
        .durations = pattern,
        .num_segments = 1
    });
    events_app_message_open();

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

    connection_vibes_deinit();
    hourly_vibes_deinit();
    enamel_deinit();
}

int main(void) {
    logf();
    prv_init();
    app_event_loop();
    prv_deinit();
}
