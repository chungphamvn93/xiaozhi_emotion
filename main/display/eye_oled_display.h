#pragma once

#include "oled_display.h"
#include "eye_face.h"
#include <memory>

/**
 * OLED Display with animated eyes
 * Extends OledDisplay to add parametric eye animation support
 */
class EyeOledDisplay : public OledDisplay {
public:
    EyeOledDisplay(esp_lcd_panel_io_handle_t panel_io,
                   esp_lcd_panel_handle_t panel,
                   int width, int height,
                   bool mirror_x, bool mirror_y);
    virtual ~EyeOledDisplay();

    /**
     * Override SetEmotion to use eye animations
     */
    virtual void SetEmotion(const char* emotion) override;

    /**
     * Enable/disable eye animation mode
     * When enabled, emotions are displayed as animated eyes
     * When disabled, falls back to default text-based display
     */
    void SetEyeAnimationMode(bool enabled);

    /**
     * Check if eye animation mode is enabled
     */
    bool IsEyeAnimationMode() const { return eye_animation_enabled_; }

    /**
     * Get the face object for direct control
     */
    eye::Face* GetFace() { return face_.get(); }

    /**
     * Enable/disable idle animations (auto blink and subtle movements)
     */
    void SetIdleAnimationEnabled(bool enabled);

    /**
     * Enable/disable speaking mode (eyes look right-left alternately)
     */
    void SetSpeakingMode(bool enabled);

    /**
     * Enable/disable listening mode (eyes blink only, no looking around)
     */
    void SetListeningMode(bool enabled);

private:
    void InitializeEyeCanvas();
    void UpdateEyeDisplay();
    void StartIdleAnimation();
    void StopIdleAnimation();
    static void IdleAnimationTimerCallback(void* arg);

    std::unique_ptr<eye::Face> face_;
    lv_obj_t* eye_canvas_ = nullptr;
    bool eye_animation_enabled_ = true;
    bool idle_animation_enabled_ = false;
    bool speaking_mode_ = false;
    bool listening_mode_ = false;
    void* idle_timer_ = nullptr;
};
