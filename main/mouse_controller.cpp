#include "mouse_controller.h"

#include "esp_log.h"

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
    }
}


std::unique_ptr<State> Idle::transition(const TouchData &current_touch) {
    switch (movement_to_kind(_last_touch, current_touch)) {
        case MovementKind::stay_off:
            return std::unique_ptr<State>(this);
        case MovementKind::touch_down:
            return std::make_unique<Tracking>();
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
            return std::make_unique<Idle>();
        case MovementKind::same_position:
            break;
        case MovementKind::new_position:
            ESP_LOGE("State Machine", "Idle state machine got new_position, impossible!");
            break;
    }
    return nullptr; // this shouldn't happen
}

std::unique_ptr<State> Glide::transition(const TouchData &current_touch) {
}

void MouseController::tick() {
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("State Machine", "got data: %d", data->z);
        _state = _state->transition(data.value());
    }
}
