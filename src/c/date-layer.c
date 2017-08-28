#include <pebble.h>
#include <pebble-events/pebble-events.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>
#include <ctype.h>
#include "logging.h"
#include "date-layer.h"

typedef struct {
    struct tm tick_time;
    EventHandle tick_timer_event_handle;
} Data;

static inline void strupp(char *s) {
    while ((*s++ = (char) toupper((int) *s)));
}

static void prv_update_proc(DateLayer *this, GContext *ctx) {
    logf();
    GRect frame = layer_get_frame(this);
    Data *data = layer_get_data(this);
    FFont *font = ffont_create_from_resource(RESOURCE_ID_ROBOTO_FFONT);

    FContext fctx;
    fctx_init_context(&fctx, ctx);

    fctx_set_text_em_height(&fctx, font, 20);

    char s[11];
    strftime(s, sizeof(s), "%b %d %a", &data->tick_time);
    strupp(s);
    fixed_t str_width = fctx_string_width(&fctx, s, font);
    fixed_t sides = (INT_TO_FIXED(PBL_DISPLAY_WIDTH) - str_width) / 2;

    fctx_set_offset(&fctx, FPoint(sides, INT_TO_FIXED(frame.origin.y)));
    fctx_set_fill_color(&fctx, PBL_IF_COLOR_ELSE(GColorYellow, GColorWhite));

    strftime(s, sizeof(s), "%b %d ", &data->tick_time);
    strupp(s);
    str_width = fctx_string_width(&fctx, s, font);

    fctx_begin_fill(&fctx);
    fctx_draw_string(&fctx, s, font, GTextAlignmentLeft, FTextAnchorTop);
    fctx_end_fill(&fctx);

    fctx_set_offset(&fctx, FPoint(sides + str_width, INT_TO_FIXED(frame.origin.y)));
    fctx_set_fill_color(&fctx, GColorWhite);

    strftime(s, sizeof(s), "%a", &data->tick_time);
    strupp(s);

    fctx_begin_fill(&fctx);
    fctx_draw_string(&fctx, s, font, GTextAlignmentLeft, FTextAnchorTop);
    fctx_end_fill(&fctx);

    fctx_deinit_context(&fctx);
    ffont_destroy(font);
}

static void prv_tick_handler(struct tm *tick_time, TimeUnits units_changed, void *this) {
    logf();
    Data *data = layer_get_data(this);
    memcpy(&data->tick_time, tick_time, sizeof(struct tm));;
    layer_mark_dirty(this);
}

DateLayer *date_layer_create(GPoint origin) {
    logf();
    DateLayer *this = layer_create_with_data(GRect(origin.x, origin.y, PBL_DISPLAY_WIDTH, PBL_DISPLAY_HEIGHT), sizeof(Data));
    layer_set_update_proc(this, prv_update_proc);
    Data *data = layer_get_data(this);

    time_t now = time(NULL);
    prv_tick_handler(localtime(&now), DAY_UNIT, this);
    data->tick_timer_event_handle = events_tick_timer_service_subscribe_context(DAY_UNIT, prv_tick_handler, this);

    return this;
}

void date_layer_destroy(DateLayer *this) {
    logf();
    Data *data = layer_get_data(this);
    events_tick_timer_service_unsubscribe(data->tick_timer_event_handle);
    layer_destroy(this);
}
