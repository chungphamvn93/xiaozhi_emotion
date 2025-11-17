#include "eye_drawer.h"
#include <esp_log.h>

#define TAG "EyeDrawer"
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// For I1 format (1-bit indexed), we need to use palette indices
// Index 0 = background (pixels ON = BLUE glow)
// Index 1 = foreground (pixels OFF = DARK eyes)
#define I1_INDEX_0 lv_color_make(0, 0, 0)  // Even value → index 0 (BLUE bg)
#define I1_INDEX_1 lv_color_make(1, 1, 1)  // Odd value → index 1 (DARK eyes)

namespace eye {

void EyeDrawer::Draw(lv_obj_t* canvas, int16_t center_x, int16_t center_y,
                     const EyeConfig* config, lv_color_t color) {
    if (canvas == nullptr || config == nullptr) {
        return;
    }

    // For I1 format: use index 1 for eyes (DARK pixels on OLED)
    color = I1_INDEX_1;

    // Amount by which corners will be shifted up/down based on requested "slope"
    int32_t delta_y_top = config->height * config->slope_top / 2.0f;
    int32_t delta_y_bottom = config->height * config->slope_bottom / 2.0f;

    // Full extent of the eye, after accounting for slope added at top and bottom
    int32_t total_height = config->height + delta_y_top - delta_y_bottom;

    // Working copy of config to adjust radii if needed
    EyeConfig cfg = *config;

    // If the requested top/bottom radius would exceed the height of the eye, adjust them downwards
    if (cfg.radius_bottom > 0 && cfg.radius_top > 0 &&
        total_height - 1 < cfg.radius_bottom + cfg.radius_top) {
        int32_t corrected_radius_top =
            (float)cfg.radius_top * (total_height - 1) /
            (cfg.radius_bottom + cfg.radius_top);
        int32_t corrected_radius_bottom =
            (float)cfg.radius_bottom * (total_height - 1) /
            (cfg.radius_bottom + cfg.radius_top);
        cfg.radius_top = corrected_radius_top;
        cfg.radius_bottom = corrected_radius_bottom;
    }

    // Calculate _inside_ corners of eye (TL, TR, BL, and BR) before any slope or
    // rounded corners are applied
    int32_t TLc_y = center_y + cfg.offset_y - cfg.height / 2 + cfg.radius_top -
                    delta_y_top;
    int32_t TLc_x = center_x + cfg.offset_x - cfg.width / 2 + cfg.radius_top;
    int32_t TRc_y = center_y + cfg.offset_y - cfg.height / 2 + cfg.radius_top +
                    delta_y_top;
    int32_t TRc_x = center_x + cfg.offset_x + cfg.width / 2 - cfg.radius_top;
    int32_t BLc_y = center_y + cfg.offset_y + cfg.height / 2 -
                    cfg.radius_bottom - delta_y_bottom;
    int32_t BLc_x = center_x + cfg.offset_x - cfg.width / 2 + cfg.radius_bottom;
    int32_t BRc_y = center_y + cfg.offset_y + cfg.height / 2 -
                    cfg.radius_bottom + delta_y_bottom;
    int32_t BRc_x = center_x + cfg.offset_x + cfg.width / 2 - cfg.radius_bottom;

    // Calculate interior extents
    int32_t min_c_x = MIN(TLc_x, BLc_x);
    int32_t max_c_x = MAX(TRc_x, BRc_x);
    int32_t min_c_y = MIN(TLc_y, TRc_y);
    int32_t max_c_y = MAX(BLc_y, BRc_y);

    // Fill eye centre
    FillRectangle(canvas, min_c_x, min_c_y, max_c_x, max_c_y, color);

    // Fill eye outwards to meet edges of rounded corners
    FillRectangle(canvas, TRc_x, TRc_y, BRc_x + cfg.radius_bottom, BRc_y,
                  color);  // Right
    FillRectangle(canvas, TLc_x - cfg.radius_top, TLc_y, BLc_x, BLc_y,
                  color);  // Left
    FillRectangle(canvas, TLc_x, TLc_y - cfg.radius_top, TRc_x, TRc_y,
                  color);  // Top
    FillRectangle(canvas, BLc_x, BLc_y, BRc_x, BRc_y + cfg.radius_bottom,
                  color);  // Bottom

    // Draw slanted edges at top of bottom of eyes
    // +ve Slope_Top means eyes slope downwards towards middle of face
    // For I1 format: use index 0 for erasing/background (BLUE pixels on OLED)
    lv_color_t black_color = I1_INDEX_0;
    if (cfg.slope_top > 0) {
        FillRectangularTriangle(canvas, TLc_x, TLc_y - cfg.radius_top, TRc_x,
                               TRc_y - cfg.radius_top, black_color);
        FillRectangularTriangle(canvas, TRc_x, TRc_y - cfg.radius_top, TLc_x,
                               TLc_y - cfg.radius_top, color);
    } else if (cfg.slope_top < 0) {
        FillRectangularTriangle(canvas, TRc_x, TRc_y - cfg.radius_top, TLc_x,
                               TLc_y - cfg.radius_top, black_color);
        FillRectangularTriangle(canvas, TLc_x, TLc_y - cfg.radius_top, TRc_x,
                               TRc_y - cfg.radius_top, color);
    }

    // Draw slanted edges at bottom of eyes
    if (cfg.slope_bottom > 0) {
        FillRectangularTriangle(canvas, BRc_x + cfg.radius_bottom,
                               BRc_y + cfg.radius_bottom,
                               BLc_x - cfg.radius_bottom,
                               BLc_y + cfg.radius_bottom, black_color);
        FillRectangularTriangle(canvas, BLc_x - cfg.radius_bottom,
                               BLc_y + cfg.radius_bottom,
                               BRc_x + cfg.radius_bottom,
                               BRc_y + cfg.radius_bottom, color);
    } else if (cfg.slope_bottom < 0) {
        FillRectangularTriangle(canvas, BLc_x - cfg.radius_bottom,
                               BLc_y + cfg.radius_bottom,
                               BRc_x + cfg.radius_bottom,
                               BRc_y + cfg.radius_bottom, black_color);
        FillRectangularTriangle(canvas, BRc_x + cfg.radius_bottom,
                               BRc_y + cfg.radius_bottom,
                               BLc_x - cfg.radius_bottom,
                               BLc_y + cfg.radius_bottom, color);
    }

    // Draw corners (which extend "outwards" towards corner of screen from
    // supplied coordinate values)
    if (cfg.radius_top > 0) {
        FillEllipseCorner(canvas, T_L, TLc_x, TLc_y, cfg.radius_top,
                         cfg.radius_top, color);
        FillEllipseCorner(canvas, T_R, TRc_x, TRc_y, cfg.radius_top,
                         cfg.radius_top, color);
    }
    if (cfg.radius_bottom > 0) {
        FillEllipseCorner(canvas, B_L, BLc_x, BLc_y, cfg.radius_bottom,
                         cfg.radius_bottom, color);
        FillEllipseCorner(canvas, B_R, BRc_x, BRc_y, cfg.radius_bottom,
                         cfg.radius_bottom, color);
    }
}

void EyeDrawer::FillRectangle(lv_obj_t* canvas, int32_t x0, int32_t y0,
                              int32_t x1, int32_t y1, lv_color_t color) {
    // Always draw from TL->BR
    int32_t l = MIN(x0, x1);
    int32_t r = MAX(x0, x1);
    int32_t t = MIN(y0, y1);
    int32_t b = MAX(y0, y1);

    if (r <= l || b <= t) return;

    // Use pixel-level drawing for LVGL v9
    for (int32_t y = t; y < b; y++) {
        for (int32_t x = l; x < r; x++) {
            lv_canvas_set_px(canvas, x, y, color, LV_OPA_COVER);
        }
    }
}

void EyeDrawer::FillRectangularTriangle(lv_obj_t* canvas, int32_t x0,
                                        int32_t y0, int32_t x1, int32_t y1,
                                        lv_color_t color) {
    // Draw a right-angled triangle using scanline algorithm
    int32_t min_x = MIN(x0, x1);
    int32_t max_x = MAX(x0, x1);
    int32_t min_y = MIN(y0, y1);
    int32_t max_y = MAX(y0, y1);

    if (x0 == x1 || y0 == y1) return;  // Degenerate triangle

    // For a right triangle with corners at (x0,y0), (x1,y1), (x1,y0)
    for (int32_t y = min_y; y <= max_y; y++) {
        int32_t x_start = min_x;
        int32_t x_end;

        if (y0 < y1) {
            // Triangle points down
            float ratio = (float)(y - min_y) / (float)(max_y - min_y);
            x_end = x0 + (int32_t)(ratio * (x1 - x0));
        } else {
            // Triangle points up
            float ratio = (float)(max_y - y) / (float)(max_y - min_y);
            x_end = x0 + (int32_t)(ratio * (x1 - x0));
        }

        DrawHLine(canvas, MIN(x_start, x_end), y, abs(x_end - x_start), color);
    }
}

void EyeDrawer::FillEllipseCorner(lv_obj_t* canvas, CornerType corner,
                                  int16_t x0, int16_t y0, int32_t rx,
                                  int32_t ry, lv_color_t color) {
    if (rx < 2) return;
    if (ry < 2) return;

    int32_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    if (corner == T_R) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y;
             x++) {
            DrawHLine(canvas, x0, y0 - y, x, color);
            if (s >= 0) {
                s += fx2 * (1 - y);
                y--;
            }
            s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x;
             y++) {
            DrawHLine(canvas, x0, y0 - y, x, color);
            if (s >= 0) {
                s += fy2 * (1 - x);
                x--;
            }
            s += rx2 * ((4 * y) + 6);
        }
    } else if (corner == B_R) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y;
             x++) {
            DrawHLine(canvas, x0, y0 + y - 1, x, color);
            if (s >= 0) {
                s += fx2 * (1 - y);
                y--;
            }
            s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x;
             y++) {
            DrawHLine(canvas, x0, y0 + y - 1, x, color);
            if (s >= 0) {
                s += fy2 * (1 - x);
                x--;
            }
            s += rx2 * ((4 * y) + 6);
        }
    } else if (corner == T_L) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y;
             x++) {
            DrawHLine(canvas, x0 - x, y0 - y, x, color);
            if (s >= 0) {
                s += fx2 * (1 - y);
                y--;
            }
            s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x;
             y++) {
            DrawHLine(canvas, x0 - x, y0 - y, x, color);
            if (s >= 0) {
                s += fy2 * (1 - x);
                x--;
            }
            s += rx2 * ((4 * y) + 6);
        }
    } else if (corner == B_L) {
        for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry); ry2 * x <= rx2 * y;
             x++) {
            DrawHLine(canvas, x0 - x, y0 + y - 1, x, color);
            if (s >= 0) {
                s += fx2 * (1 - y);
                y--;
            }
            s += ry2 * ((4 * x) + 6);
        }
        for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx); rx2 * y <= ry2 * x;
             y++) {
            DrawHLine(canvas, x0 - x, y0 + y, x, color);
            if (s >= 0) {
                s += fy2 * (1 - x);
                x--;
            }
            s += rx2 * ((4 * y) + 6);
        }
    }
}

void EyeDrawer::DrawHLine(lv_obj_t* canvas, int16_t x, int16_t y, int16_t len,
                          lv_color_t color) {
    if (len <= 0) return;

    // Use pixel-level drawing for LVGL v9
    for (int16_t i = 0; i < len; i++) {
        lv_canvas_set_px(canvas, x + i, y, color, LV_OPA_COVER);
    }
}

void EyeDrawer::SetPixel(lv_obj_t* canvas, int16_t x, int16_t y,
                         lv_color_t color) {
    lv_canvas_set_px(canvas, x, y, color, LV_OPA_COVER);
}

}  // namespace eye
