#pragma once

#include "cirque_pinnacle.h"
#include <memory>
constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;

struct State {
    virtual ~State() = default;

    virtual std::unique_ptr<State> transition(const TouchData &current_touch) = 0;

    TouchData _last_touch;
};

struct Idle final : State {
    std::unique_ptr<State> transition(const TouchData &current_touch) override;
};

struct Tracking final : State {
    std::unique_ptr<State> transition(const TouchData &current_touch) override;
};

struct Glide final : State {
    std::unique_ptr<State> transition(const TouchData &current_touch) override;

    float vx, vy;
};

class MouseController {
public:
    MouseController() = default;

    auto tick() -> void;

private:
    TouchPad _touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};
    std::unique_ptr<State> _state;
};
