#pragma once

#include <optional>

#include "driver/i2c_master.h"

struct TouchData {
    uint16_t x, y;
    uint8_t z;
};

class TouchPad {
public:
    TouchPad(gpio_num_t sda_pin, gpio_num_t scl_pin, gpio_num_t data_ready_pin);

    [[nodiscard]] std::optional<TouchData> get_data() const;

private:
    i2c_master_bus_handle_t _bus_handle{};
    i2c_master_dev_handle_t _dev_handle{};
    gpio_num_t _data_ready_pin{};

    void write_rap(uint8_t address, uint8_t data) const;
};
