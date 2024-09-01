#pragma once

#include "cirque_pinnacle.h"
#include <functional>

constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;

struct Movement {
    TouchData from, to;
};

// https://stackoverflow.com/questions/23737449/recursive-typedef-function-definition-stdfunction-returning-its-own-type
template<typename... T>
struct FunctionReturningOwnTypeHelper {
    typedef std::function<FunctionReturningOwnTypeHelper(T...)> type;

    FunctionReturningOwnTypeHelper(type f) : func(f) {
    }

    operator type() { return func; }
    type func;
};

typedef FunctionReturningOwnTypeHelper<Movement &>::type StateUpdater;

StateUpdater idle(const Movement &movement);

StateUpdater initial_touch(const Movement &movement);

StateUpdater tracking(const Movement &movement);

class MouseController {
public:
    MouseController() = default;

    auto tick() -> void;

private:
    TouchPad _touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};
    StateUpdater _state_updater{&idle};
    Movement _latest_movement{};
};
