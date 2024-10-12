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
    return std::make_unique<Idle>();
}

void MouseController::tick() {
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("State Machine", "got data: %d", data->z);
        _state = _state->transition(data.value());
    }
}
