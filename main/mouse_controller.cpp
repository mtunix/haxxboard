#include "mouse_controller.h"
#include "esp_log.h"


std::unique_ptr<State> Idle::update(Movement movement) {
    ESP_LOGI("StateMachine", "Hi from idle");
    return std::make_unique<InitialTouch>();
}

std::unique_ptr<State> InitialTouch::update(Movement movement) {
    ESP_LOGI("StateMachine", "Hi from initial touch");
    return std::make_unique<Tracking>();
}

std::unique_ptr<State> Tracking::update(Movement movement) {
    ESP_LOGI("StateMachine", "Hi from tracking");
    return std::make_unique<Idle>();
}

void MouseController::tick() {
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("StateMachine", "got data: %d", data->z);
        _latest_movement.from = _latest_movement.to;
        _latest_movement.to = data.value();
        _current_state = _current_state->update(_latest_movement);
    }
}
