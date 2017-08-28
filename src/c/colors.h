#include <pebble.h>
#include "enamel.h"

inline GColor colors_get_accent_color(void) {
    return PBL_IF_COLOR_ELSE(enamel_get_COLOR_ACCENT(), GColorWhite);
}
