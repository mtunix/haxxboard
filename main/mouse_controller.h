#pragma once

#include "cirque_pinnacle.h"
#include <memory>
constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;


// inheritance based state machine is slightly ugly syntax wise
// at least it is easy to pass touch history around the states
// glide mode can also capture its past momentum

struct State {
    virtual ~State() = default;

    State() = delete;

    explicit State(const TouchData &last_touch): _last_touch(last_touch) {
    }

    virtual std::unique_ptr<State> transition(const TouchData &current_touch) = 0;

    TouchData _last_touch;
};

struct Idle final : State {
    using State::State;

    std::unique_ptr<State> transition(const TouchData &current_touch) override;
};

struct Tracking final : State {
    explicit Tracking(const TouchData &last_touch, const int dx, const int dy): State(last_touch), _dx(dx), _dy(dy) {
    }

    std::unique_ptr<State> transition(const TouchData &current_touch) override;

    int _dx, _dy;
};

class MouseController {
public:
    MouseController() = default;

    auto tick() -> void;

private:
    TouchPad _touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};
    std::unique_ptr<State> _state{std::make_unique<Idle>(TouchData{0, 0, 0})};
};
