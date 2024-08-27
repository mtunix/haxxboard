#pragma once

#include "cirque_pinnacle.h"
#include <functional>

constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;

using StateFunction = std::function<void(class MouseController &)>;

class MouseController {
public:
    MouseController();

    StateFunction tick{idle};

private:
    TouchPad _touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};

    void idle();

    void initial_touch();

    void tracking();
};
