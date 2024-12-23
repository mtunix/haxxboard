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

        //ESP_LOGI("StateMachine", "No state matches, this shouldn't happen...");
        return MovementKind::touch_down; // need some return value
    }

    int8_t clamp(const int value) {
        return static_cast<int8_t>(
            std::clamp(
                value,
                static_cast<int>(std::numeric_limits<int8_t>::min()),
                static_cast<int>(std::numeric_limits<int8_t>::max())));
    }
}


std::unique_ptr<State> Idle::transition(const TouchData &current_touch) {
    //ESP_LOGI("State Machine", "Idle transition");
    switch (movement_to_kind(_last_touch, current_touch)) {
        case MovementKind::stay_off:
            //ESP_LOGI("State Machine", "Idle stay off");
            return std::make_unique<Idle>(current_touch);

        case MovementKind::touch_down:
            //ESP_LOGI("State Machine", "Idle touch down");
            return std::make_unique<Tracking>(current_touch, 0, 0);

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
    //ESP_LOGI("State Machine", "Tracking transition");
    switch (movement_to_kind(_last_touch, current_touch)) {
        case MovementKind::stay_off:
            ESP_LOGE("State Machine", "Tracking state machine got stay_off, impossible!");
            break;

        case MovementKind::touch_down:
            ESP_LOGE("State Machine", "Tracking state machine got touch_down, impossible!");
            break;

        case MovementKind::lift_off:
            //ESP_LOGI("State Machine", "Tracking liftoff");
            return std::make_unique<Idle>(current_touch);

        case MovementKind::same_position:
            //ESP_LOGI("State Machine", "Tracking same position");
            return std::make_unique<Tracking>(current_touch, _dx, _dy);

        case MovementKind::new_position:
            //ESP_LOGI("State Machine", "Tracking new position");
            constexpr int slowdown = 4;

            const auto [x_quot, x_rem] = std::div(_dx + current_touch.x - _last_touch.x, slowdown);
            const auto [y_quot, y_rem] = std::div(_dy + current_touch.y - _last_touch.y, slowdown);

            ESP_LOGI("State Machine", "sending mouse report: %d %d", x_quot, y_quot);
            tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE, 0x00, clamp(x_quot), clamp(y_quot), 0, 0);
        //ESP_LOGI("State Machine", "mouse report sent");
            return std::make_unique<Tracking>(current_touch, x_rem, y_rem);
    }
    //ESP_LOGI("State Machine", "Tracking impossible state");
    return nullptr; // this shouldn't happen
}

void MouseController::tick() {
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("State Machine", "got data: x y z %d %d %d", data->x, data->y, data->z);
        _state = _state->transition(data.value());
    }
}
