#include "mouse_controller.h"
#include "esp_log.h"

enum class MovementKind {
    touch_down,
    lift_off,
    stay_off,
    same_position,
    new_position
};

namespace {
    MovementKind movement_to_kind(const Movement &movement) {
        if (movement.from.z == 0 and movement.to.z == 0) {
            return MovementKind::stay_off;
        }

        if(movement.from.z == 0 and movement.to.z > 0) {
            return MovementKind::touch_down;
        }


        if(movement.from.z > 0 and movement.to.z == 0) {
            return MovementKind::lift_off;
        }
    }
}

StateUpdater idle(const Movement &movement) {
    ESP_LOGI("StateMachine", "Hi from idle");
    if (movement)
        return &initial_touch;
}

StateUpdater initial_touch(const Movement &movement) {
    ESP_LOGI("StateMachine", "Hi from initial touch");
    return &tracking;
}

StateUpdater tracking(const Movement &movement) {
    ESP_LOGI("StateMachine", "Hi from tracking");
    return &idle;
}

void MouseController::tick() {
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("StateMachine", "got data: %d", data->z);
        _latest_movement.from = _latest_movement.to;
        _latest_movement.to = data.value();
        _state_updater = _state_updater(_latest_movement);
    }
}
