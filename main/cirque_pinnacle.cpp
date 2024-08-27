#include "cirque_pinnacle.h"

#include <esp_log.h>

#include "driver/gpio.h"

namespace {
    // Masks for Cirque Register Access Protocol (RAP)
    constexpr uint8_t write_mask = 0x80;
    constexpr uint8_t read_mask = 0xA0;

    // Register config values required for absolute value reading
    constexpr uint8_t status_1_addr = 0x02;
    constexpr uint8_t feedconfig_1_addr = 0x04;
    constexpr uint8_t absolute_data_start_addr = 0x14;

    void setup_data_ready_pin(const gpio_num_t data_ready_pin) {
        const gpio_config_t gpio_cfg = {
            .pin_bit_mask = BIT64(data_ready_pin),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    }

    void setup_i2c_master(i2c_master_bus_handle_t &bus_handle, const gpio_num_t sda_pin, const gpio_num_t scl_pin) {
        // set up esp as i2c master and pinnacle as device
        const i2c_master_bus_config_t i2c_mst_config = {
            .i2c_port = -1,
            .sda_io_num = sda_pin,
            .scl_io_num = scl_pin,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .flags = {.enable_internal_pullup = true}
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    }

    void setup_i2c_cirque_device(const i2c_master_bus_handle_t &bus_handle, i2c_master_dev_handle_t &dev_handle) {
        constexpr i2c_device_config_t dev_cfg = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = 0x2A, // Cirque's 7-bit I2C Slave Address, always 0x2A
            .scl_speed_hz = 100000,
        };

        ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
        ESP_ERROR_CHECK(i2c_master_probe(bus_handle, dev_cfg.device_address, -1));
    }
}

TouchPad::TouchPad(const gpio_num_t sda_pin, const gpio_num_t scl_pin,
                   const gpio_num_t data_ready_pin): _data_ready_pin(
    data_ready_pin) {
    setup_data_ready_pin(data_ready_pin);
    setup_i2c_master(_bus_handle, sda_pin, scl_pin);
    setup_i2c_cirque_device(_bus_handle, _dev_handle);

    // startup sequence of the pinnacle chip:
    // 1. Power on Reset should be triggered (pull voltage jumper!)
    if (!gpio_get_level(_data_ready_pin)) {
        ESP_LOGE("GPIO", "Error: DR pin is low after PoR");
    }

    // 2. Host clears SW_CC (writes value 0x00 Status1 register), which clears HW_DR.
    write_rap(status_1_addr, 0x00);

    if (!gpio_get_level(_data_ready_pin)) {
        ESP_LOGE("GPIO", "Error: DR pin is high after SW_CC clear");
    }
    // enable feed and absolute value reporting
    write_rap(0x05, 0x00);
    write_rap(feedconfig_1_addr, 0x03);
}

std::optional<TouchData> TouchPad::get_data() const {
    if (!gpio_get_level(_data_ready_pin)) {
        return {};
    }

    constexpr uint8_t read_cmd = absolute_data_start_addr | read_mask;
    ESP_ERROR_CHECK(i2c_master_transmit(_dev_handle, &read_cmd, 1, -1));

    std::array<uint8_t, 4> data{};
    ESP_ERROR_CHECK(i2c_master_receive(_dev_handle, data.data(), 4, -1));

    // clear status register to be able to get the next value
    write_rap(status_1_addr, 0x00);

    return {
        {
            .x = static_cast<uint16_t>(data[0] | ((data[2] & 0x0F) << 8)),
            .y = static_cast<uint16_t>(data[1] | ((data[2] & 0xF0) << 4)),
            .z = static_cast<uint8_t>(data[3] & 0x3F)
        }
    };
}

void TouchPad::write_rap(const uint8_t address, const uint8_t data) const {
    const uint8_t write_cmd[2] = {static_cast<uint8_t>(address | write_mask), data};
    ESP_ERROR_CHECK(i2c_master_transmit(_dev_handle, write_cmd, 2, -1));
}
