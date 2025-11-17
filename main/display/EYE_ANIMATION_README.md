# Eye Animation System

LVGL-based parametric eye animation system for xiaozhi-esp32, ported from [esp32-eyes](https://github.com/playfultechnology/esp32-eyes).

## Overview

This system provides expressive animated eyes using parametric drawing instead of pre-rendered bitmaps. Eyes are drawn dynamically based on configurable parameters, allowing for smooth animations and customizable emotions.

## Architecture

### Core Components

1. **EyeConfig** (`eye_config.h`)
   - Defines eye shape parameters (width, height, slope, corner radius, etc.)
   - Stored in a simple struct for easy manipulation

2. **Eye Presets** (`eye_presets.h`)
   - Pre-defined configurations for 17 different emotions
   - Normal, Happy, Sad, Angry, Surprised, and more

3. **EyeDrawer** (`eye_drawer.h`, `eye_drawer.cc`)
   - LVGL-based drawing engine
   - Converts parametric eye config to rendered graphics
   - Handles rounded corners, slopes, and complex eye shapes

4. **Face** (`eye_face.h`, `eye_face.cc`)
   - High-level API for managing two eyes
   - Emotion control
   - Blink animation
   - Look direction control

5. **EyeOledDisplay** (`eye_oled_display.h`, `eye_oled_display.cc`)
   - Integration with OLED displays
   - Drop-in replacement for standard OledDisplay

## Available Emotions

The system supports 17 emotions:

- **NORMAL** - Neutral expression
- **HAPPY** - Smiling eyes (crescents)
- **GLEE** - Very happy, joyful
- **SAD** - Sad, drooping eyes
- **WORRIED** - Concerned expression
- **FOCUSED** - Concentrated, narrow eyes
- **ANNOYED** - Slightly irritated
- **SURPRISED** - Wide open eyes
- **SKEPTIC** - Raised eyebrow look
- **FRUSTRATED** - Annoyed, offset eyes
- **SLEEPY** - Drowsy, half-closed
- **SUSPICIOUS** - Narrow, watchful
- **SQUINT** - Very narrow, squinting
- **ANGRY** - Angry slanted eyebrows
- **FURIOUS** - Very angry
- **SCARED** - Frightened, wide eyes
- **AWE** - Amazed, wonder

## Usage

### Basic Integration (OLED Display)

```cpp
#include "display/eye_oled_display.h"

// Create eye-enabled OLED display
auto display = new EyeOledDisplay(panel_io, panel, 128, 64, false, false);

// Set emotion
display->SetEmotion("happy");    // Use any emotion name
display->SetEmotion("angry");
display->SetEmotion("surprised");

// Enable/disable eye animation mode
display->SetEyeAnimationMode(true);   // Show animated eyes
display->SetEyeAnimationMode(false);  // Revert to default display
```

### Advanced Control (Direct Face API)

```cpp
#include "display/eye_face.h"

// Get face object for direct control
auto face = display->GetFace();

// Set emotions by enum
face->SetEmotion(eye::Emotion::HAPPY);
face->SetEmotion(eye::Emotion::SURPRISED);

// Control eye direction
face->LookAt(0.5f, -0.3f);  // Look right and up
// x: -1.0 (left) to 1.0 (right)
// y: -1.0 (up) to 1.0 (down)

// Trigger blink animation
face->Blink();

// Update display (call periodically)
face->Update();
```

### Custom Eye Shapes

You can create custom eye configurations:

```cpp
#include "display/eye_config.h"

eye::EyeConfig custom_eye = {
    .offset_x = 0,          // Horizontal offset from center
    .offset_y = 5,          // Vertical offset from center
    .height = 35,           // Eye height in pixels
    .width = 45,            // Eye width in pixels
    .slope_top = 0.2f,      // Top edge angle (-1.0 to 1.0)
    .slope_bottom = -0.1f,  // Bottom edge angle
    .radius_top = 10,       // Top corner radius
    .radius_bottom = 8,     // Bottom corner radius
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};

// Apply custom config (requires direct canvas access)
eye::EyeDrawer::Draw(canvas, center_x, center_y, &custom_eye, LV_COLOR_WHITE);
```

## Integration with Existing Display System

### Option 1: Replace OledDisplay (Recommended for eye-only displays)

In your board configuration file:

```cpp
// Before:
// #include "display/oled_display.h"
// display_ = new OledDisplay(panel_io, panel, 128, 64, false, false);

// After:
#include "display/eye_oled_display.h"
display_ = new EyeOledDisplay(panel_io, panel, 128, 64, false, false);
```

### Option 2: Standalone Canvas (For custom layouts)

```cpp
#include "display/eye_face.h"

// Create canvas
lv_obj_t* eye_canvas = lv_canvas_create(lv_scr_act());
static lv_color_t cbuf[128 * 64];
lv_canvas_set_buffer(eye_canvas, cbuf, 128, 64, LV_COLOR_FORMAT_NATIVE_MONO);

// Create face
auto face = new eye::Face(eye_canvas, 128, 64, 40);

// Use it
face->SetEmotion(eye::Emotion::HAPPY);
face->Update();
```

## Customization Guide

### Adjusting Eye Parameters

**Size:**
- `width` and `height` - Overall eye dimensions
- Larger values = bigger eyes

**Shape:**
- `slope_top` - Positive = slopes down toward center, negative = slopes up
- `slope_bottom` - Controls bottom edge angle
- Example: Angry eyes use positive slope_top

**Corners:**
- `radius_top` - Rounded corners at top (0 = sharp)
- `radius_bottom` - Rounded corners at bottom
- Example: Happy eyes have large top radius to create crescent shape

**Position:**
- `offset_x` - Move eye left (negative) or right (positive)
- `offset_y` - Move eye up (negative) or down (positive)

### Creating New Emotions

Add to `eye_presets.h`:

```cpp
static const EyeConfig PRESET_MY_EMOTION = {
    .offset_x = 0,
    .offset_y = -5,        // Shift up slightly
    .height = 30,
    .width = 50,           // Wide eyes
    .slope_top = -0.3f,    // Upward slant
    .slope_bottom = 0.1f,
    .radius_top = 15,      // Very rounded
    .radius_bottom = 5,
    .inverse_radius_top = 0,
    .inverse_radius_bottom = 0,
    .inverse_offset_top = 0,
    .inverse_offset_bottom = 0
};
```

Then add to `eye_face.cc` in `GetPresetForEmotion()`:

```cpp
case Emotion::MY_EMOTION:
    return &PRESET_MY_EMOTION;
```

## Performance Considerations

- **Canvas Buffer**: Uses 128x64 = 8KB for monochrome displays
- **Drawing Speed**: Parametric drawing is fast enough for real-time animation
- **Memory**: Minimal - only stores config structs, not bitmap data
- **CPU Usage**: Low - drawing is optimized with ellipse algorithms

## Troubleshooting

### Eyes not showing
- Check canvas buffer allocation
- Verify LVGL lock/unlock calls
- Ensure canvas is not hidden: `lv_obj_clear_flag(canvas, LV_OBJ_FLAG_HIDDEN)`

### Eyes look distorted
- Adjust eye_size parameter in Face constructor
- Check screen dimensions match actual display
- Verify radius values don't exceed height/width

### Performance issues
- Reduce update frequency
- Use simpler eye shapes (smaller radius values)
- Consider using lower resolution canvas

## Example: Emotion Cycling Demo

```cpp
#include "display/eye_oled_display.h"

void demo_emotions(EyeOledDisplay* display) {
    const char* emotions[] = {
        "normal", "happy", "sad", "angry", "surprised",
        "sleepy", "worried", "focused", "glee"
    };

    for (int i = 0; i < 9; i++) {
        display->SetEmotion(emotions[i]);
        vTaskDelay(pdMS_TO_TICKS(2000));  // 2 seconds each
    }
}
```

## Credits

Based on the [esp32-eyes](https://github.com/playfultechnology/esp32-eyes) project by:
- Alastair Aitchison (Playful Technology)
- Luis Llamas (www.luisllamas.es)

Ported to LVGL for xiaozhi-esp32 project.

## License

Inherits from esp32-eyes project: GNU Affero General Public License v3.0
