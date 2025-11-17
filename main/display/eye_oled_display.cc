#include "eye_oled_display.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <esp_random.h>
#include <cstring>

#define TAG "EyeOledDisplay"

EyeOledDisplay::EyeOledDisplay(esp_lcd_panel_io_handle_t panel_io,
                               esp_lcd_panel_handle_t panel,
                               int width, int height,
                               bool mirror_x, bool mirror_y)
    : OledDisplay(panel_io, panel, width, height, mirror_x, mirror_y) {

    InitializeEyeCanvas();

    ESP_LOGI(TAG, "EyeOledDisplay initialized with eye animations");
}

EyeOledDisplay::~EyeOledDisplay() {
    StopIdleAnimation();
    face_.reset();
    if (eye_canvas_) {
        lv_obj_del(eye_canvas_);
        eye_canvas_ = nullptr;
    }
}

void EyeOledDisplay::InitializeEyeCanvas() {
    if (!Lock(100)) {
        ESP_LOGE(TAG, "Failed to lock display for canvas initialization");
        return;
    }

    // Calculate canvas dimensions (below status bar)
    const int status_bar_height = 16;  // Status bar is 16 pixels tall
    int canvas_height = height_ - status_bar_height;  // 48px for eyes
    int canvas_width = width_;

    // Create canvas for drawing eyes (absolute positioning)
    eye_canvas_ = lv_canvas_create(lv_scr_act());  // Direct child of screen root
    if (!eye_canvas_) {
        ESP_LOGE(TAG, "Failed to create eye canvas");
        Unlock();
        return;
    }

    // Position canvas absolutely at Y=16 (below status bar)
    lv_obj_set_pos(eye_canvas_, 0, status_bar_height);  // Position at Y=16
    lv_obj_set_size(eye_canvas_, canvas_width, canvas_height);  // 128x48

    // Remove padding, border, and ensure visibility
    lv_obj_set_style_pad_all(eye_canvas_, 0, 0);
    lv_obj_set_style_border_width(eye_canvas_, 0, 0);
    lv_obj_set_style_radius(eye_canvas_, 0, 0);
    lv_obj_set_style_bg_opa(eye_canvas_, LV_OPA_TRANSP, 0);  // Transparent background style
    lv_obj_clear_flag(eye_canvas_, LV_OBJ_FLAG_HIDDEN);  // Ensure visible
    lv_obj_clear_flag(eye_canvas_, LV_OBJ_FLAG_CLICKABLE);  // Not clickable
    lv_obj_clear_flag(eye_canvas_, LV_OBJ_FLAG_SCROLLABLE);  // Not scrollable

    // Allocate buffer for canvas (1 bit per pixel for monochrome)
    // For I1 format: (width * height + 7) / 8 bytes
    // 128x32 = 4096 bits = 512 bytes, allocate 1024 for safety
    static uint8_t cbuf[(128 * 64 + 7) / 8];
    memset(cbuf, 0, sizeof(cbuf));  // Clear buffer
    lv_canvas_set_buffer(eye_canvas_, cbuf, canvas_width, canvas_height, LV_COLOR_FORMAT_I1);

    // Set up color palette for I1 format for BLUE OLED
    // Index 0 = white (pixels ON = BLUE background)
    // Index 1 = black (pixels OFF = DARK eye outlines)
    lv_color32_t color_black;
    color_black.red = 0x00;
    color_black.green = 0x00;
    color_black.blue = 0x00;
    color_black.alpha = 0xFF;

    lv_color32_t color_white;
    color_white.red = 0xFF;
    color_white.green = 0xFF;
    color_white.blue = 0xFF;
    color_white.alpha = 0xFF;

    lv_canvas_set_palette(eye_canvas_, 0, color_white);  // Index 0 = BLUE background (pixels ON)
    lv_canvas_set_palette(eye_canvas_, 1, color_black);  // Index 1 = DARK eyes (pixels OFF)

    // Fill canvas with index 0 (BLUE background)
    lv_canvas_fill_bg(eye_canvas_, lv_color_make(0, 0, 0), LV_OPA_COVER);

    // Create face object with adjusted dimensions
    int eye_size = 30;  // Eye size for 48px height canvas
    face_ = std::make_unique<eye::Face>(eye_canvas_, canvas_width, canvas_height, eye_size);

    // Force initial draw
    face_->Update();

    Unlock();

    ESP_LOGI(TAG, "Eye display layout: status_bar(16px) + eyes(48px)");
    ESP_LOGI(TAG, "Eye canvas: %dx%d at Y=%d (absolute), eye_size=%d",
             canvas_width, canvas_height, status_bar_height, eye_size);
}

void EyeOledDisplay::SetEmotion(const char* emotion) {
    if (!emotion) return;

    if (eye_animation_enabled_ && face_) {
        // Use eye animations
        if (Lock(100)) {
            face_->SetEmotionByName(emotion);
            face_->Update();
            Unlock();
        }
    } else {
        // Fall back to parent implementation
        OledDisplay::SetEmotion(emotion);
    }
}

void EyeOledDisplay::SetEyeAnimationMode(bool enabled) {
    eye_animation_enabled_ = enabled;

    if (!Lock(100)) {
        ESP_LOGE(TAG, "Failed to lock display");
        return;
    }

    if (enabled && face_) {
        // Hide content UI elements (but keep status bar and container visible)
        HideDefaultUI(true);

        // Show eye canvas
        lv_obj_clear_flag(eye_canvas_, LV_OBJ_FLAG_HIDDEN);

        // Update eyes
        face_->Update();

        ESP_LOGI(TAG, "Eyes visible, status bar visible, content hidden");
    } else {
        // Hide eye canvas
        if (eye_canvas_) {
            lv_obj_add_flag(eye_canvas_, LV_OBJ_FLAG_HIDDEN);
        }

        // Show default UI elements
        HideDefaultUI(false);
    }

    Unlock();

    ESP_LOGI(TAG, "Eye animation mode: %s", enabled ? "enabled" : "disabled");
}

void EyeOledDisplay::UpdateEyeDisplay() {
    if (face_ && eye_animation_enabled_) {
        if (Lock(100)) {
            face_->Update();
            Unlock();
        }
    }
}

void EyeOledDisplay::IdleAnimationTimerCallback(void* arg) {
    auto* self = static_cast<EyeOledDisplay*>(arg);
    if (!self || !self->face_ || !self->eye_animation_enabled_) {
        return;
    }

    if (!self->Lock(100)) {
        return;
    }

    // Listening mode: breathing effect (smooth pulsing), no blinking or looking
    if (self->listening_mode_) {
        // Breathing is handled automatically in DrawEyes() when enabled
        // Just update the display to animate the breathing
        self->face_->Update();
        self->Unlock();
        return;
    }

    // Random actions to make eyes look alive - more left/right looking
    // (Same for both idle and speaking modes, speaking just runs faster)
    uint32_t random = esp_random();
    uint8_t action = random % 100;

    if (action < 10) {
        // 10% chance: Blink
        self->face_->Blink();
    } else if (action < 40) {
        // 30% chance: Look left
        float look_x = -0.6f - (random % 40) / 100.0f;  // -0.6 to -1.0
        self->face_->LookAt(look_x, 0.0f);
    } else if (action < 70) {
        // 30% chance: Look right
        float look_x = 0.6f + (random % 40) / 100.0f;   // 0.6 to 1.0
        self->face_->LookAt(look_x, 0.0f);
    } else if (action < 80) {
        // 10% chance: Look up/down slightly
        float look_y = ((random % 60) - 30) / 100.0f;   // -0.3 to 0.3
        self->face_->LookAt(0.0f, look_y);
    } else {
        // 20% chance: Return to center
        self->face_->LookAt(0.0f, 0.0f);
    }

    self->face_->Update();
    self->Unlock();
}

void EyeOledDisplay::StartIdleAnimation() {
    if (idle_timer_) {
        return;  // Already running
    }

    const esp_timer_create_args_t timer_args = {
        .callback = &EyeOledDisplay::IdleAnimationTimerCallback,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "eye_idle_anim",
        .skip_unhandled_events = true
    };

    esp_timer_handle_t timer;
    if (esp_timer_create(&timer_args, &timer) == ESP_OK) {
        // Different speeds for different modes
        uint64_t interval_us;
        const char* mode_name;

        if (listening_mode_) {
            interval_us = (300000 + (esp_random() % 300000));  // 0.3-0.6 seconds (listening - fastest)
            mode_name = "listening";
        } else if (speaking_mode_) {
            interval_us = (400000 + (esp_random() % 400000));  // 0.4-0.8 seconds (speaking)
            mode_name = "speaking";
        } else {
            interval_us = (800000 + (esp_random() % 700000));  // 0.8-1.5 seconds (idle)
            mode_name = "idle";
        }

        if (esp_timer_start_periodic(timer, interval_us) == ESP_OK) {
            idle_timer_ = timer;
            ESP_LOGI(TAG, "Idle animation started (%s mode)", mode_name);
        } else {
            esp_timer_delete(timer);
        }
    }
}

void EyeOledDisplay::StopIdleAnimation() {
    if (idle_timer_) {
        esp_timer_stop((esp_timer_handle_t)idle_timer_);
        esp_timer_delete((esp_timer_handle_t)idle_timer_);
        idle_timer_ = nullptr;
        ESP_LOGI(TAG, "Idle animation stopped");
    }
}

void EyeOledDisplay::SetIdleAnimationEnabled(bool enabled) {
    idle_animation_enabled_ = enabled;

    if (enabled) {
        StartIdleAnimation();
    } else {
        StopIdleAnimation();
    }

    ESP_LOGI(TAG, "Idle animation: %s", enabled ? "enabled" : "disabled");
}

void EyeOledDisplay::SetSpeakingMode(bool enabled) {
    speaking_mode_ = enabled;

    // Restart timer with different speed if animations are enabled
    if (idle_animation_enabled_) {
        StopIdleAnimation();
        StartIdleAnimation();
    }

    ESP_LOGI(TAG, "Speaking mode: %s", enabled ? "enabled" : "disabled");
}

void EyeOledDisplay::SetListeningMode(bool enabled) {
    listening_mode_ = enabled;

    // Enable breathing effect for listening mode
    if (face_) {
        face_->SetBreathing(enabled);
        if (enabled) {
            // Center eyes when entering listening mode
            face_->LookAt(0.0f, 0.0f);
        }
    }

    // Restart timer with different speed if animations are enabled
    if (idle_animation_enabled_) {
        StopIdleAnimation();
        StartIdleAnimation();
    }

    ESP_LOGI(TAG, "Listening mode: %s", enabled ? "enabled" : "disabled");
}
