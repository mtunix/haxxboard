#include "mouse_controller.h"

#include "esp_log.h"
#include <class/hid/hid_device.h>
#include <limits>

#include "functional"

namespace {
    enum class MovementKind {
        touch_down,
        lift_off,
        stay_off,
        same_position,
        new_position
    };

    MovementKind movement_to_kind(const TouchData &from, const TouchData &to) {
        if (from.z == 0 and to.z == 0) {
            return MovementKind::stay_off;
        }

        if (from.z == 0 and to.z > 0) {
            return MovementKind::touch_down;
        }


        if (from.z > 0 and to.z == 0) {
            return MovementKind::lift_off;
        }

        if (from.x == to.x and from.y == to.y) {
            return MovementKind::same_position;
        }

        if (from.x != to.x or from.y != to.y) {
            return MovementKind::new_position;
        }

        ESP_LOGI("StateMachine", "No state matches, this shouldn't happen...");
        return MovementKind::touch_down; // need some return value
    }
}


std::unique_ptr<State> Idle::transition(const TouchData &current_touch) {
    switch (movement_to_kind(_last_touch, current_touch)) {
        case MovementKind::stay_off:
            return std::make_unique<Idle>(current_touch);
        case MovementKind::touch_down:
            return std::make_unique<Tracking>(current_touch);
        case MovementKind::lift_off:
            ESP_LOGE("State Machine", "Idle state machine got liftoff, impossible!");
            break;
        case MovementKind::same_position:
            ESP_LOGE("State Machine", "Idle state machine got same_position, impossible!");
            break;
        case MovementKind::new_position:
            ESP_LOGE("State Machine", "Idle state machine got new_position, impossible!");
            break;
    }
    return nullptr; // this shouldn't happen
}

std::unique_ptr<State> Tracking::transition(const TouchData &current_touch) {
    switch (movement_to_kind(_last_touch, current_touch)) {
        case MovementKind::stay_off:
            ESP_LOGE("State Machine", "Tracking state machine got stay_off, impossible!");
            break;
        case MovementKind::touch_down:
            ESP_LOGE("State Machine", "Tracking state machine got touch_down, impossible!");
            break;
        case MovementKind::lift_off:
        case MovementKind::same_position:
            return std::make_unique<Idle>(current_touch);
        case MovementKind::new_position:
            const int dx = current_touch.x - _last_touch.x;
            const int8_t dx_clamped = static_cast<int8_t>(std::clamp(
                dx, static_cast<int>(std::numeric_limits<int8_t>::min()),
                static_cast<int>(std::numeric_limits<int8_t>::max())));
            const int dy = current_touch.y - _last_touch.y;
            const int8_t dy_clamped = static_cast<int8_t>(std::clamp(
                dy, static_cast<int>(std::numeric_limits<int8_t>::min()),
                static_cast<int>(std::numeric_limits<int8_t>::max())));

            tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE, 0x00, dx_clamped, dy_clamped, 0, 0);
            return std::unique_ptr<State>(this);
    }
    return nullptr; // this shouldn't happen
}

void MouseController::tick() {
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("State Machine", "got data: %d", data->z);
        //_state = _state->transition(data.value());
    }
}
