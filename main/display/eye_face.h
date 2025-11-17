#pragma once

#include "eye_config.h"
#include "eye_drawer.h"
#include "eye_presets.h"
#include <lvgl.h>
#include <cstdint>
#include <string>

namespace eye {

/**
 * Emotion types
 */
enum class Emotion {
    NORMAL,
    HAPPY,
    GLEE,
    SAD,
    WORRIED,
    FOCUSED,
    ANNOYED,
    SURPRISED,
    SKEPTIC,
    FRUSTRATED,
    SLEEPY,
    SUSPICIOUS,
    SQUINT,
    ANGRY,
    FURIOUS,
    SCARED,
    AWE
};

/**
 * Face class that manages two eyes
 * Provides high-level API for emotion control
 */
class Face {
public:
    /**
     * Constructor
     * @param canvas LVGL canvas object for drawing
     * @param screen_width Width of the screen
     * @param screen_height Height of the screen
     * @param eye_size Base size of each eye
     */
    Face(lv_obj_t* canvas, int screen_width, int screen_height, int eye_size);
    ~Face();

    /**
     * Set the current emotion
     */
    void SetEmotion(Emotion emotion);

    /**
     * Set emotion by name string
     */
    void SetEmotionByName(const char* emotion_name);

    /**
     * Get the current emotion
     */
    Emotion GetEmotion() const { return current_emotion_; }

    /**
     * Update and render the face
     * Call this periodically to animate
     */
    void Update();

    /**
     * Blink the eyes
     */
    void Blink();

    /**
     * Set eye look direction
     * @param x Horizontal direction (-1.0 to 1.0)
     * @param y Vertical direction (-1.0 to 1.0)
     */
    void LookAt(float x, float y);

    /**
     * Enable/disable breathing effect (smooth pulsing for listening mode)
     */
    void SetBreathing(bool enabled);

    /**
     * Clear the canvas
     */
    void Clear();

private:
    /**
     * Get preset config for emotion
     */
    const EyeConfig* GetPresetForEmotion(Emotion emotion);

    /**
     * Draw both eyes
     */
    void DrawEyes();

    lv_obj_t* canvas_;
    int screen_width_;
    int screen_height_;
    int eye_size_;

    Emotion current_emotion_;
    EyeConfig left_eye_config_;
    EyeConfig right_eye_config_;

    // Eye positions
    int16_t left_eye_center_x_;
    int16_t left_eye_center_y_;
    int16_t right_eye_center_x_;
    int16_t right_eye_center_y_;

    // Look direction
    float look_x_;
    float look_y_;

    // Blink state
    bool is_blinking_;
    uint32_t blink_start_time_;
    static const uint32_t BLINK_DURATION_MS = 150;

    // Breathing state (smooth pulsing for listening mode)
    bool is_breathing_;
    float breathing_phase_;  // 0.0 to 2*PI for smooth sine wave
};

}  // namespace eye
