#include "eye_face.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <cstring>
#include <cmath>

#define TAG "EyeFace"

namespace eye {

Face::Face(lv_obj_t* canvas, int screen_width, int screen_height, int eye_size)
    : canvas_(canvas),
      screen_width_(screen_width),
      screen_height_(screen_height),
      eye_size_(eye_size),
      current_emotion_(Emotion::NORMAL),
      look_x_(0.0f),
      look_y_(0.0f),
      is_blinking_(false),
      blink_start_time_(0),
      is_breathing_(false),
      breathing_phase_(0.0f) {

    // Calculate eye positions (centered horizontally, with spacing)
    // Based on FluxGarage RoboEyes spacing calculation
    int eye_spacing = 20;  // Space between the two eyes (gap between eye edges)
    int total_width = eye_size + eye_spacing + eye_size;  // Total width of both eyes + gap
    int start_x = (screen_width - total_width) / 2;  // Left edge of left eye

    left_eye_center_x_ = start_x + eye_size / 2;  // Center of left eye
    left_eye_center_y_ = screen_height / 2;
    right_eye_center_x_ = start_x + eye_size + eye_spacing + eye_size / 2;  // Center of right eye
    right_eye_center_y_ = screen_height / 2;

    // Initialize with normal emotion
    SetEmotion(Emotion::NORMAL);

    ESP_LOGI(TAG, "Face initialized: %dx%d, eye_size=%d, positions: L(%d,%d) R(%d,%d)",
             screen_width, screen_height, eye_size,
             left_eye_center_x_, left_eye_center_y_,
             right_eye_center_x_, right_eye_center_y_);
}

Face::~Face() {
    // Cleanup if needed
}

const EyeConfig* Face::GetPresetForEmotion(Emotion emotion) {
    switch (emotion) {
        case Emotion::NORMAL:
            return &PRESET_NORMAL;
        case Emotion::HAPPY:
            return &PRESET_HAPPY;
        case Emotion::GLEE:
            return &PRESET_GLEE;
        case Emotion::SAD:
            return &PRESET_SAD;
        case Emotion::WORRIED:
            return &PRESET_WORRIED;
        case Emotion::FOCUSED:
            return &PRESET_FOCUSED;
        case Emotion::ANNOYED:
            return &PRESET_ANNOYED;
        case Emotion::SURPRISED:
            return &PRESET_SURPRISED;
        case Emotion::SKEPTIC:
            return &PRESET_SKEPTIC;
        case Emotion::FRUSTRATED:
            return &PRESET_FRUSTRATED;
        case Emotion::SLEEPY:
            return &PRESET_SLEEPY;
        case Emotion::SUSPICIOUS:
            return &PRESET_SUSPICIOUS;
        case Emotion::SQUINT:
            return &PRESET_SQUINT;
        case Emotion::ANGRY:
            return &PRESET_ANGRY;
        case Emotion::FURIOUS:
            return &PRESET_FURIOUS;
        case Emotion::SCARED:
            return &PRESET_SCARED;
        case Emotion::AWE:
            return &PRESET_AWE;
        default:
            return &PRESET_NORMAL;
    }
}

void Face::SetEmotion(Emotion emotion) {
    current_emotion_ = emotion;
    const EyeConfig* preset = GetPresetForEmotion(emotion);

    if (preset) {
        left_eye_config_ = *preset;
        right_eye_config_ = *preset;
        ESP_LOGI(TAG, "Emotion changed to %d", static_cast<int>(emotion));
    }
}

void Face::SetEmotionByName(const char* emotion_name) {
    if (emotion_name == nullptr) return;

    if (strcasecmp(emotion_name, "happy") == 0) {
        SetEmotion(Emotion::HAPPY);
    } else if (strcasecmp(emotion_name, "sad") == 0) {
        SetEmotion(Emotion::SAD);
    } else if (strcasecmp(emotion_name, "angry") == 0) {
        SetEmotion(Emotion::ANGRY);
    } else if (strcasecmp(emotion_name, "surprised") == 0) {
        SetEmotion(Emotion::SURPRISED);
    } else if (strcasecmp(emotion_name, "worried") == 0) {
        SetEmotion(Emotion::WORRIED);
    } else if (strcasecmp(emotion_name, "sleepy") == 0) {
        SetEmotion(Emotion::SLEEPY);
    } else if (strcasecmp(emotion_name, "normal") == 0) {
        SetEmotion(Emotion::NORMAL);
    } else if (strcasecmp(emotion_name, "glee") == 0) {
        SetEmotion(Emotion::GLEE);
    } else if (strcasecmp(emotion_name, "focused") == 0) {
        SetEmotion(Emotion::FOCUSED);
    } else if (strcasecmp(emotion_name, "annoyed") == 0) {
        SetEmotion(Emotion::ANNOYED);
    } else if (strcasecmp(emotion_name, "skeptic") == 0) {
        SetEmotion(Emotion::SKEPTIC);
    } else if (strcasecmp(emotion_name, "frustrated") == 0) {
        SetEmotion(Emotion::FRUSTRATED);
    } else if (strcasecmp(emotion_name, "suspicious") == 0) {
        SetEmotion(Emotion::SUSPICIOUS);
    } else if (strcasecmp(emotion_name, "squint") == 0) {
        SetEmotion(Emotion::SQUINT);
    } else if (strcasecmp(emotion_name, "furious") == 0) {
        SetEmotion(Emotion::FURIOUS);
    } else if (strcasecmp(emotion_name, "scared") == 0) {
        SetEmotion(Emotion::SCARED);
    } else if (strcasecmp(emotion_name, "awe") == 0) {
        SetEmotion(Emotion::AWE);
    } else {
        ESP_LOGW(TAG, "Unknown emotion: %s, using NORMAL", emotion_name);
        SetEmotion(Emotion::NORMAL);
    }
}

void Face::LookAt(float x, float y) {
    // Clamp values to -1.0 to 1.0 range
    look_x_ = (x < -1.0f) ? -1.0f : (x > 1.0f) ? 1.0f : x;
    look_y_ = (y < -1.0f) ? -1.0f : (y > 1.0f) ? 1.0f : y;
}

void Face::Blink() {
    is_blinking_ = true;
    blink_start_time_ = esp_timer_get_time() / 1000;  // Convert to milliseconds
}

void Face::SetBreathing(bool enabled) {
    is_breathing_ = enabled;
    if (enabled) {
        breathing_phase_ = 0.0f;  // Reset phase
    }
    ESP_LOGI(TAG, "Breathing effect: %s", enabled ? "enabled" : "disabled");
}

void Face::Clear() {
    if (canvas_) {
        // For I1 format: index 0 = BLUE background (pixels ON)
        lv_canvas_fill_bg(canvas_, lv_color_make(0, 0, 0), LV_OPA_COVER);
    }
}

void Face::DrawEyes() {
    if (!canvas_) return;

    // Handle blink animation (only when eyes are centered, not looking around)
    bool is_looking = (look_x_ != 0.0f || look_y_ != 0.0f);

    if (is_blinking_ && !is_looking) {
        uint32_t current_time = esp_timer_get_time() / 1000;
        if (current_time - blink_start_time_ > BLINK_DURATION_MS) {
            is_blinking_ = false;
        } else {
            // Draw closed eyes as one continuous thin line across both eyes
            EyeConfig blink_config = left_eye_config_;
            blink_config.height = 2;  // Thin line for blink
            // Use full width to span both eyes + gap for continuous line
            blink_config.width = screen_width_;  // Full screen width for continuous line

            // Draw single blink line centered vertically
            EyeDrawer::Draw(canvas_, screen_width_ / 2, left_eye_center_y_,
                           &blink_config, lv_color_hex(0xFFFFFF));
            return;
        }
    }

    // Apply look direction offset
    int16_t look_offset_x = static_cast<int16_t>(look_x_ * eye_size_ * 0.2f);
    int16_t look_offset_y = static_cast<int16_t>(look_y_ * eye_size_ * 0.2f);

    // Create 3D perspective effect by scaling eye size based on look direction
    // When looking right: right eye bigger (closer), left eye smaller (farther)
    // When looking left: left eye bigger (closer), right eye smaller (farther)
    const int16_t size_change = 4;  // Max pixels to change width/height

    // Calculate size adjustment based on look_x (-1.0 to 1.0)
    float look_factor = look_x_;  // -1.0 (full left) to +1.0 (full right)

    // Left eye: gets smaller when looking right (positive look_x)
    EyeConfig left_config = left_eye_config_;
    int16_t left_size_adjust = static_cast<int16_t>(-look_factor * size_change);
    left_config.width += left_size_adjust;
    left_config.height += left_size_adjust;

    // Right eye: gets bigger when looking right (positive look_x)
    EyeConfig right_config = right_eye_config_;
    int16_t right_size_adjust = static_cast<int16_t>(look_factor * size_change);
    right_config.width += right_size_adjust;
    right_config.height += right_size_adjust;

    // Apply breathing effect (smooth pulsing for listening mode)
    if (is_breathing_) {
        const float breathing_speed = 0.15f;  // Increment per update
        const int16_t breathing_amplitude = 5;  // ±5px oscillation

        // Update breathing phase (0 to 2*PI for one full cycle)
        breathing_phase_ += breathing_speed;
        if (breathing_phase_ > 6.28318f) {  // 2*PI
            breathing_phase_ -= 6.28318f;
        }

        // Calculate smooth breathing offset using sine wave
        float breathing_factor = std::sin(breathing_phase_);  // -1.0 to 1.0
        int16_t breathing_adjust = static_cast<int16_t>(breathing_factor * breathing_amplitude);

        // Apply breathing to both eyes equally
        left_config.width += breathing_adjust;
        left_config.height += breathing_adjust;
        right_config.width += breathing_adjust;
        right_config.height += breathing_adjust;
    }

    // Draw left eye with adjusted size
    int16_t left_x = left_eye_center_x_ + look_offset_x;
    int16_t left_y = left_eye_center_y_ + look_offset_y;
    EyeDrawer::Draw(canvas_, left_x, left_y, &left_config, lv_color_hex(0xFFFFFF));

    // Draw right eye with adjusted size
    int16_t right_x = right_eye_center_x_ + look_offset_x;
    int16_t right_y = right_eye_center_y_ + look_offset_y;
    EyeDrawer::Draw(canvas_, right_x, right_y, &right_config,
                   lv_color_hex(0xFFFFFF));
}

void Face::Update() {
    Clear();
    DrawEyes();
}

}  // namespace eye
