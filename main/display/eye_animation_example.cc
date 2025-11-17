/**
 * Eye Animation System Usage Examples
 *
 * This file demonstrates different ways to use the eye animation system
 * in your xiaozhi-esp32 project.
 */

#include "eye_oled_display.h"
#include "eye_face.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "EyeExample"

// ============================================================================
// Example 1: Basic Usage with EyeOledDisplay
// ============================================================================

void example_basic_eye_display() {
    ESP_LOGI(TAG, "Example 1: Basic Eye Display");

    // Create eye-enabled OLED display (assuming panel_io and panel are initialized)
    // EyeOledDisplay* display = new EyeOledDisplay(panel_io, panel, 128, 64, false, false);

    // Set different emotions
    // display->SetEmotion("normal");
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // display->SetEmotion("happy");
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // display->SetEmotion("surprised");
    // vTaskDelay(pdMS_TO_TICKS(2000));

    // display->SetEmotion("angry");
}

// ============================================================================
// Example 2: Direct Face Control
// ============================================================================

void example_direct_face_control(EyeOledDisplay* display) {
    ESP_LOGI(TAG, "Example 2: Direct Face Control");

    // Get face object for advanced control
    eye::Face* face = display->GetFace();
    if (!face) return;

    // Set emotion by enum
    face->SetEmotion(eye::Emotion::HAPPY);
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Control eye direction
    face->LookAt(0.5f, 0.0f);  // Look right
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(1000));

    face->LookAt(-0.5f, 0.0f);  // Look left
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(1000));

    face->LookAt(0.0f, -0.5f);  // Look up
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(1000));

    face->LookAt(0.0f, 0.5f);  // Look down
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Trigger blink
    face->Blink();
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(200));
}

// ============================================================================
// Example 3: Emotion Cycle Demo
// ============================================================================

void example_emotion_cycle(EyeOledDisplay* display) {
    ESP_LOGI(TAG, "Example 3: Emotion Cycle");

    const char* emotions[] = {
        "normal", "happy", "glee", "sad", "worried",
        "focused", "annoyed", "surprised", "sleepy", "angry"
    };

    for (int i = 0; i < 10; i++) {
        ESP_LOGI(TAG, "Showing emotion: %s", emotions[i]);
        display->SetEmotion(emotions[i]);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// ============================================================================
// Example 4: Interactive Eyes (Following Input)
// ============================================================================

void example_interactive_eyes(EyeOledDisplay* display, float joystick_x, float joystick_y) {
    ESP_LOGI(TAG, "Example 4: Interactive Eyes");

    eye::Face* face = display->GetFace();
    if (!face) return;

    // Make eyes follow joystick or sensor input
    // joystick_x and joystick_y should be in range -1.0 to 1.0
    face->LookAt(joystick_x, joystick_y);
    face->Update();
}

// ============================================================================
// Example 5: Blinking Animation Loop
// ============================================================================

void example_blinking_loop(EyeOledDisplay* display) {
    ESP_LOGI(TAG, "Example 5: Blinking Animation");

    eye::Face* face = display->GetFace();
    if (!face) return;

    // Automatic blinking every 3 seconds
    for (int i = 0; i < 5; i++) {
        face->Update();
        vTaskDelay(pdMS_TO_TICKS(3000));

        face->Blink();
        face->Update();
        vTaskDelay(pdMS_TO_TICKS(150));  // Blink duration
    }
}

// ============================================================================
// Example 6: Custom Eye Configuration
// ============================================================================

void example_custom_eye_config() {
    ESP_LOGI(TAG, "Example 6: Custom Eye Configuration");

    // Create a custom eye shape
    eye::EyeConfig custom_eye = {
        .offset_x = 0,
        .offset_y = 5,
        .height = 35,
        .width = 45,
        .slope_top = 0.2f,      // Slight upward angle
        .slope_bottom = -0.1f,
        .radius_top = 10,       // Rounded top
        .radius_bottom = 8,
        .inverse_radius_top = 0,
        .inverse_radius_bottom = 0,
        .inverse_offset_top = 0,
        .inverse_offset_bottom = 0
    };

    // You would need to add this to eye_presets.h to use it with Face class
    ESP_LOGI(TAG, "Custom eye config created: w=%d h=%d", custom_eye.width, custom_eye.height);
}

// ============================================================================
// Example 7: Switching Between Eye Animation and Normal Display
// ============================================================================

void example_toggle_eye_mode(EyeOledDisplay* display) {
    ESP_LOGI(TAG, "Example 7: Toggle Eye Animation Mode");

    // Show eye animations
    display->SetEyeAnimationMode(true);
    display->SetEmotion("happy");
    vTaskDelay(pdMS_TO_TICKS(3000));

    // Switch to normal display mode (text/emoji)
    display->SetEyeAnimationMode(false);
    vTaskDelay(pdMS_TO_TICKS(3000));

    // Back to eyes
    display->SetEyeAnimationMode(true);
    display->SetEmotion("normal");
}

// ============================================================================
// Example 8: Responding to Events
// ============================================================================

void example_emotion_response_to_events(EyeOledDisplay* display) {
    ESP_LOGI(TAG, "Example 8: Emotion Response to Events");

    eye::Face* face = display->GetFace();
    if (!face) return;

    // Simulate different event responses

    // User speaks
    face->SetEmotion(eye::Emotion::FOCUSED);
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Got response
    face->SetEmotion(eye::Emotion::HAPPY);
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Error occurred
    face->SetEmotion(eye::Emotion::WORRIED);
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Idle/sleeping
    face->SetEmotion(eye::Emotion::SLEEPY);
    face->Update();
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Back to normal
    face->SetEmotion(eye::Emotion::NORMAL);
    face->Update();
}

// ============================================================================
// Example 9: Standalone Canvas (Custom Layout)
// ============================================================================

void example_standalone_canvas() {
    ESP_LOGI(TAG, "Example 9: Standalone Canvas");

    /*
    // Create your own canvas for eyes
    lv_obj_t* eye_canvas = lv_canvas_create(lv_scr_act());
    static lv_color_t cbuf[128 * 64];
    lv_canvas_set_buffer(eye_canvas, cbuf, 128, 64, LV_COLOR_FORMAT_I1);
    lv_obj_center(eye_canvas);

    // Create face
    eye::Face* face = new eye::Face(eye_canvas, 128, 64, 40);

    // Use it
    face->SetEmotion(eye::Emotion::SURPRISED);
    face->Update();

    // Clean up
    delete face;
    */
}

// ============================================================================
// Main Demo Task
// ============================================================================

void eye_animation_demo_task(void* pvParameters) {
    EyeOledDisplay* display = static_cast<EyeOledDisplay*>(pvParameters);

    ESP_LOGI(TAG, "Starting eye animation demo");

    // Run examples
    example_emotion_cycle(display);
    vTaskDelay(pdMS_TO_TICKS(1000));

    example_direct_face_control(display);
    vTaskDelay(pdMS_TO_TICKS(1000));

    example_blinking_loop(display);
    vTaskDelay(pdMS_TO_TICKS(1000));

    example_emotion_response_to_events(display);
    vTaskDelay(pdMS_TO_TICKS(1000));

    example_toggle_eye_mode(display);

    ESP_LOGI(TAG, "Demo complete");
    vTaskDelete(NULL);
}

/**
 * To use in your board initialization:
 *
 * // Create display with eye support
 * auto display = new EyeOledDisplay(panel_io, panel, 128, 64, false, false);
 *
 * // Option 1: Use simple API
 * display->SetEmotion("happy");
 *
 * // Option 2: Get direct face control
 * eye::Face* face = display->GetFace();
 * face->SetEmotion(eye::Emotion::SURPRISED);
 * face->LookAt(0.5f, -0.3f);
 * face->Update();
 *
 * // Option 3: Run demo task
 * xTaskCreate(eye_animation_demo_task, "eye_demo", 4096, display, 5, NULL);
 */
