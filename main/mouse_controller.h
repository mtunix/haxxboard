#pragma once

#include "cirque_pinnacle.h"

constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;

// alternative implementation:
// mouse controller is a lambda that returns a lambda
// and passes around the touch pad and history as arguments

class MouseController {
public:
    MouseController() = default;

    void tick();

private:
    TouchPad _touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};

    void idle();

    void initial_touch();

    void tracking();

    void (MouseController::*_current_state_function)(){&MouseController::idle};
};
