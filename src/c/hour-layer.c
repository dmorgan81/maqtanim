#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include "logging.h"
#include "hour-layer.h"

typedef struct {
    struct tm tick_time;
    EventHandle tick_timer_event_handle;
} Data;

static void prv_update_proc(HourLayer *this, GContext *ctx) {
    logf();
    GRect frame = layer_get_frame(this);
    Data *data = layer_get_data(this);
    FFont *font = ffont_create_from_resource(RESOURCE_ID_ROBOTO_CONDENSED_FFONT);

    FContext fctx;
    fctx_init_context(&fctx, ctx);

    fctx_set_offset(&fctx, g2fpoint(frame.origin));
    fctx_set_fill_color(&fctx, GColorWhite);
    fctx_set_text_em_height(&fctx, font, 72);

    char s[3];
    strftime(s, sizeof(s), clock_is_24h_style() ? "%H" : "%I", &data->tick_time);

    fctx_begin_fill(&fctx);
    fctx_draw_string(&fctx, s, font, GTextAlignmentRight, FTextAnchorTop);
    fctx_end_fill(&fctx);

    fctx_deinit_context(&fctx);
    ffont_destroy(font);
}

static void prv_tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    logf();
    Data *data = layer_get_data(this);
    memcpy(&data->tick_time, tick_time, sizeof(struct tm));
#ifdef DEMO
    data->tick_time.tm_hour = 12;
#endif
    layer_mark_dirty(this);
}

HourLayer *hour_layer_create(GPoint origin) {
    logf();
    HourLayer *this = layer_create_with_data(GRect(origin.x, origin.y, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT), sizeof(Data));
    layer_set_update_proc(this, prv_update_proc);
    Data *data = layer_get_data(this);

    time_t now = time(NULL);
    prv_tick_handler(localtime(&now), HOUR_UNIT, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(HOUR_UNIT, prv_tick_handler, this);

    return this;
}

void hour_layer_destroy(HourLayer *this) {
    logf();
    Data *data = layer_get_data(this);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    layer_destroy(this);
}
