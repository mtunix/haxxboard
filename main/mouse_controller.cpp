#include "mouse_controller.h"
#include "esp_log.h"

void MouseController::tick() {
    (this->*_current_state_function)();
    if (const auto data = _touch_pad.get_data()) {
        ESP_LOGI("StateMachine", "got data: %d", data->z);
    }
}

void MouseController::idle() {
    ESP_LOGI("StateMachine", "Hi from idle");
    this->_current_state_function = &MouseController::initial_touch;
}

void MouseController::initial_touch() {
    ESP_LOGI("StateMachine", "Hi from initial touch");
    this->_current_state_function = &MouseController::tracking;
}

void MouseController::tracking() {
    ESP_LOGI("StateMachine", "Hi from tracking");
    this->_current_state_function = &MouseController::idle;
}
