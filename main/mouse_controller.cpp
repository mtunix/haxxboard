#include "mouse_controller.h"
#include "esp_log.h"

StateUpdater idle(const Movement &movement) {
    ESP_LOGI("StateMachine", "Hi from idle");
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
