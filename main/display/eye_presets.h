#pragma once

#include "eye_config.h"

namespace eye {

/**
 * Eye presets for different emotions
 * Based on esp32-eyes project by Alastair Aitchison and Luis Llamas
 */

static const EyeConfig PRESET_NORMAL = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 30,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 6,
    .radius_bottom = 6,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_HAPPY = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 8,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 8,
    .radius_bottom = 0,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_GLEE = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 6,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 6,
    .radius_bottom = 0,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 4,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SAD = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 12,
    .width = 30,
    .slope_top = -0.5f,
    .slope_bottom = 0,
    .radius_top = 1,
    .radius_bottom = 8,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_WORRIED = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 20,
    .width = 30,
    .slope_top = -0.1f,
    .slope_bottom = 0,
    .radius_top = 5,
    .radius_bottom = 8,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_FOCUSED = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 11,
    .width = 30,
    .slope_top = 0.2f,
    .slope_bottom = 0,
    .radius_top = 2,
    .radius_bottom = 1,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_ANNOYED = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 10,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 0,
    .radius_bottom = 8,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SURPRISED = {
    .offset_x = -2,
    .offset_y = 0,
    .height = 34,
    .width = 34,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 12,
    .radius_bottom = 12,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SKEPTIC = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 30,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 8,
    .radius_bottom = 8,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_FRUSTRATED = {
    .offset_x = 3,
    .offset_y = -5,
    .height = 10,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 0,
    .radius_bottom = 8,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SLEEPY = {
    .offset_x = 0,
    .offset_y = -2,
    .height = 11,
    .width = 30,
    .slope_top = -0.5f,
    .slope_bottom = -0.5f,
    .radius_top = 3,
    .radius_bottom = 3,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SUSPICIOUS = {
    .offset_x = 0,
    .offset_y = 0,
    .height = 17,
    .width = 30,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 6,
    .radius_bottom = 3,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SQUINT = {
    .offset_x = -8,
    .offset_y = -3,
    .height = 26,
    .width = 26,
    .slope_top = 0,
    .slope_bottom = 0,
    .radius_top = 6,
    .radius_bottom = 6,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_ANGRY = {
    .offset_x = -3,
    .offset_y = 0,
    .height = 15,
    .width = 30,
    .slope_top = 0.3f,
    .slope_bottom = 0,
    .radius_top = 2,
    .radius_bottom = 9,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_FURIOUS = {
    .offset_x = -2,
    .offset_y = 0,
    .height = 23,
    .width = 30,
    .slope_top = 0.4f,
    .slope_bottom = 0,
    .radius_top = 2,
    .radius_bottom = 6,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_SCARED = {
    .offset_x = -3,
    .offset_y = 0,
    .height = 30,
    .width = 30,
    .slope_top = -0.1f,
    .slope_bottom = 0,
    .radius_top = 9,
    .radius_bottom = 6,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

static const EyeConfig PRESET_AWE = {
    .offset_x = 2,
    .offset_y = 0,
    .height = 26,
    .width = 34,
    .slope_top = -0.1f,
    .slope_bottom = 0.1f,
    .radius_top = 9,
    .radius_bottom = 9,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

}  // namespace eye
