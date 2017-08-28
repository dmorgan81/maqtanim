#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include "colors.h"
#include "logging.h"
#include "battery-layer.h"

typedef struct {
    BatteryChargeState charge_state;
    EventHandle battery_state_event_handle;
} Data;

static void prv_update_proc(BatteryLayer *this, GContext *ctx) {
    logf();
    Data *data = layer_get_data(this);
    GRect bounds = layer_get_bounds(this);

    uint8_t w = bounds.size.w * data->charge_state.charge_percent / 100;
    uint8_t x = (bounds.size.w - w) / 2;

    graphics_context_set_stroke_color(ctx, colors_get_accent_color());
    graphics_context_set_stroke_width(ctx, 2);
    graphics_draw_line(ctx, GPoint(x, 0), GPoint(x + w, 0));
}

static void prv_battery_state_handler(BatteryChargeState charge_state, void *this) {
    logf();
    Data *data = layer_get_data(this);
    memcpy(&data->charge_state, &charge_state, sizeof(BatteryChargeState));
    layer_mark_dirty(this);
}

BatteryLayer *battery_layer_create(GRect frame) {
    logf();
    BatteryLayer *this = layer_create_with_data(frame, sizeof(Data));
    layer_set_update_proc(this, prv_update_proc);
    Data *data = layer_get_data(this);

    prv_battery_state_handler(battery_state_service_peek(), this);
    data->battery_state_event_handle = events_battery_state_service_subscribe_context(prv_battery_state_handler, this);

    return this;
}

void battery_layer_destroy(BatteryLayer *this) {
    logf();
    Data *data = layer_get_data(this);
    events_battery_state_service_unsubscribe(data->battery_state_event_handle);
    layer_destroy(this);
}
