#pragma once

#include <cstdint>

namespace eye {

/**
 * Eye configuration structure
 * Defines the shape and appearance of an eye
 */
struct EyeConfig {
    int16_t offset_x;
    int16_t offset_y;

    int16_t height;
    int16_t width;

    float slope_top;
    float slope_bottom;

    int16_t radius_top;
    int16_t radius_bottom;

    int16_t inverse_radius_top;
    int16_t inverse_radius_bottom;

    int16_t inverse_offset_top;
    int16_t inverse_offset_bottom;
};

}  // namespace eye
