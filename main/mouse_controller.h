#pragma once

#include "cirque_pinnacle.h"
#include <functional>
#include <memory>

constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;

struct Movement {
    TouchData from, to;
};

struct State {
    virtual ~State() = default;

    virtual std::unique_ptr<State> update(Movement movement) = 0;
};

struct Idle final : State {
    std::unique_ptr<State> update(Movement movement) override;
};

struct InitialTouch final : State {
    std::unique_ptr<State> update(Movement movement) override;
};

struct Tracking final : State {
    std::unique_ptr<State> update(Movement movement) override;
};

class MouseController {
public:
    MouseController() = default;

    auto tick() -> void;

private:
    TouchPad _touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};
    std::unique_ptr<State> _current_state{std::make_unique<Idle>()};
    Movement _latest_movement{};
};
