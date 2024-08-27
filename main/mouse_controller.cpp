#include "mouse_controller.h"
#include "esp_log.h"

void MouseController::idle() {
    ESP_LOGI("StateMachine", "Hi from idle");
    this->tick = &initial_touch;
}

void MouseController::initial_touch() {
    ESP_LOGI("StateMachine", "Hi from initial touch");
    this->tick = &tracking;
}

void MouseController::tracking() {
    ESP_LOGI("StateMachine", "Hi from tracking");
    this->tick = &idle;
}
