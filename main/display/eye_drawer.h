#pragma once

#include "eye_config.h"
#include <lvgl.h>
#include <cstdint>
#include <algorithm>

namespace eye {

enum CornerType { T_R, T_L, B_L, B_R };

/**
 * LVGL-based eye drawer
 * Draws parametric eyes on an LVGL canvas
 * Ported from esp32-eyes (u8g2) to LVGL
 */
class EyeDrawer {
public:
    /**
     * Draw an eye on the canvas at the specified center position
     * @param canvas LVGL canvas object to draw on
     * @param center_x X coordinate of eye center
     * @param center_y Y coordinate of eye center
     * @param config Eye configuration
     * @param color Color to draw (LV_COLOR_WHITE for normal, LV_COLOR_BLACK to erase)
     */
    static void Draw(lv_obj_t* canvas, int16_t center_x, int16_t center_y,
                     const EyeConfig* config, lv_color_t color);

private:
    /**
     * Fill a rectangle on the canvas
     */
    static void FillRectangle(lv_obj_t* canvas, int32_t x0, int32_t y0,
                             int32_t x1, int32_t y1, lv_color_t color);

    /**
     * Fill a rectangular triangle (right angle triangle)
     */
    static void FillRectangularTriangle(lv_obj_t* canvas, int32_t x0, int32_t y0,
                                       int32_t x1, int32_t y1, lv_color_t color);

    /**
     * Fill an ellipse corner (rounded corner)
     */
    static void FillEllipseCorner(lv_obj_t* canvas, CornerType corner,
                                 int16_t x0, int16_t y0, int32_t rx, int32_t ry,
                                 lv_color_t color);

    /**
     * Draw a horizontal line on the canvas
     */
    static void DrawHLine(lv_obj_t* canvas, int16_t x, int16_t y,
                         int16_t len, lv_color_t color);

    /**
     * Set a pixel on the canvas
     */
    static void SetPixel(lv_obj_t* canvas, int16_t x, int16_t y, lv_color_t color);
};

}  // namespace eye
