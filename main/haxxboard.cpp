#include <led_strip.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "key_scan.h"
#include "key_maps.h"
#include "led.h"
#include "usb.h"
#include <vector>
#include <iostream>
#include <driver/gpio.h>
#include <sys/stat.h>

#include "driver/i2c_master.h"


// cirque sample adapted from https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Circular_Trackpad/Single_Pad_Sample_Code/I2C_FlatOverlay_USBMouse/I2C_FlatOverlay_USBMouse.ino
constexpr gpio_num_t sda_pin = GPIO_NUM_2;
constexpr gpio_num_t scl_pin = GPIO_NUM_1;
constexpr gpio_num_t dr_pin = GPIO_NUM_3;

// Cirque's 7-bit I2C Slave Address
constexpr uint8_t slave_addr = 0x2A;

// Masks for Cirque Register Access Protocol (RAP)
constexpr uint8_t write_mask = 0x80;
constexpr uint8_t read_mask = 0xA0;

// Register config values for this demo
constexpr uint8_t sysconfig_1_addr = 0x03;
constexpr uint8_t feedconfig_1_addr = 0x04;
constexpr uint8_t feedconfig_2_addr = 0x05;
constexpr uint8_t z_idle_count = 0x05;
constexpr uint8_t packetbyte_0_address = 0x12;

// Coordinate scaling values
constexpr uint16_t pinnacle_xmax = 2047; // max value Pinnacle can report for X
constexpr uint16_t pinnacle_ymax = 1535; // max value Pinnacle can report for Y
constexpr uint16_t pinnacle_x_lower = 127; // min "reachable" X value
constexpr uint16_t pinnacle_x_upper = 1919; // max "reachable" X value
constexpr uint16_t pinnacle_y_lower = 63; // min "reachable" Y value
constexpr uint16_t pinnacle_y_upper = 1471; // max "reachable" Y value
constexpr uint16_t pinnacle_x_range = (pinnacle_x_upper - pinnacle_x_lower);
constexpr uint16_t pinnacle_y_range = (pinnacle_y_upper - pinnacle_y_lower);

struct dummy {
    uint8_t bit0: 1 = 0;
    uint8_t bit1: 1 = 0;
    uint8_t bit2: 1 = 0;
    uint8_t bit3: 1 = 0;
    uint8_t bit4: 1 = 0;
    uint8_t bit5: 1 = 0;
    uint8_t bit6: 1 = 0;
    uint8_t bit7: 1 = 0;
};

union Status1 {
    struct {
        bool bit0: 1 = false;
        bool bit1: 1 = false;
        bool software_data_ready: 1 = false;
        bool commmand_complete: 1 = false;
        bool bit4: 1 = false;
        bool bit5: 1 = false;
        bool bit6: 1 = false;
        bool bit7: 1 = false;
    } bits{};

    uint8_t value;
};

static_assert(sizeof(Status1) == 1, "Size of Status1 should be 1 byte.");

union SysConfig1 {
    struct {
        bool reset: 1 = false; // 1 = reset
        bool shutdown: 1 = false; // 1 = shutdown
        bool sleep_enable: 1 = false; // 1 = low power mode
        bool bit3: 1 = false;
        bool bit4: 1 = false;
        bool bit5: 1 = false;
        bool bit6: 1 = false;
        bool bit7: 1 = false;
    } bits{};

    uint8_t value;
};

static_assert(sizeof(SysConfig1) == 1, "Size of SysConfig1 should be 1 byte.");

union FeedConfig1 {
    struct {
        bool feed_enable: 1 = false; // 1 = feed, 0 = no feed
        bool data_mode: 1 = false; // 1 = absolute, 0 = relative
        bool filter_disable: 1 = false; // 1 = no filter, 0 = filter
        bool x_disable: 1 = false; // 1 = no x data, 0 = x data
        bool y_disable: 1 = false; // 1 = no y data, 0 = y data
        bool bit5: 1 = false;
        bool x_data_invert: 1 = false; // 1 = y max to 0, 0 = 0 to y max
        bool y_data_invert: 1 = false; // 1 = x max to 0, 0 = 0 to x max
    } bits{};

    uint8_t value;
};

static_assert(sizeof(FeedConfig1) == 1, "Size of FeedConfig1 should be 1 byte.");

union FeedConfig2 {
    struct {
        bool intellimouse_enable: 1 = false;
        bool all_taps_disable: 1 = false;
        bool secondary_tap_disable: 1 = false;
        bool scroll_disable: 1 = false;
        bool glide_extend_disable: 1 = false;
        bool bit5: 1 = false;
        bool bit6: 1 = false;
        bool swap_x_y: 1 = false;
    } bits{};

    uint8_t value;
};

union position {
    struct {
        uint8_t base_value = 0;
        uint8_t sign = 0;
    } bits{};

    int16_t value;
};


static_assert(sizeof(FeedConfig2) == 1, "Size of FeedConfig2 should be 1 byte.");


std::vector<uint8_t> read_rap(i2c_master_dev_handle_t dev_handle, uint8_t address, uint8_t numBytes) {
    const uint8_t read_cmd = address | read_mask;
    esp_err_t ret = i2c_master_transmit(dev_handle, &read_cmd, 1, -1);
    std::vector<uint8_t> data(numBytes);

    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to send read command with address: 0x%.2x", address);
        return {};
    }

    ret = i2c_master_receive(dev_handle, data.data(), numBytes, -1);

    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to receive data from address: 0x%.2x", address);
        return {};
    }

    return data;
}

void write_rap(i2c_master_dev_handle_t dev_handle, uint8_t address, uint8_t data) {
    const uint8_t write_cmd[2] = {static_cast<uint8_t>(address | write_mask), data};
    const auto ret = i2c_master_transmit(dev_handle, write_cmd, 2, -1);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Failed to write RAP with address: 0x%.2x, data: 0x%.2x", address, data);
    }
}


template<typename T>
char *to_binary(T num) {
    static char bin[sizeof(T) * 8 + 1] = {0};
    int n = sizeof(T) * 8 - 1;
    for (int i = n; i >= 0; --i) {
        bin[i] = (num & 1) ? '1' : '0';
        num >>= 1;
    }
    return bin;
}

extern "C" void app_main(void) {
    ESP_LOGI("General", "Hi from the start of main");

    /*position a{.bits = {.base_value = 3}};
    position b{.bits = {.base_value = 255}};
    position c{.bits = {.base_value = 255, .sign = 255}};
    position d{.bits = {.base_value = 0}};
    position e{.bits = {.base_value = 0, .sign = 255}};

    ESP_LOGI("General", "value 3: %d", a.value);
    ESP_LOGI("General", "value 255: %d", b.value);
    ESP_LOGI("General", "value -1: %d", c.value);
    ESP_LOGI("General", "value 0: %d", d.value);
    ESP_LOGI("General", "value -255: %d", e.value);

    return;*/

    // set up hardware data ready probe pin
    constexpr gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(dr_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));

    bool hw_dr = gpio_get_level(dr_pin);
    ESP_LOGI("General", "hw_dr value %d", hw_dr);


    // set up esp as i2c master and pinnacle as device
    constexpr i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = -1,
        .sda_io_num = sda_pin,
        .scl_io_num = scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {.enable_internal_pullup = true}
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    constexpr i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = slave_addr,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, slave_addr, -1));

    // Using another random address (0x11) crashes, meaning that we see the cirque device above successfully 🥳
    // ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x11, -1));


    // startup sequence of the pinnacle chip:
    // 1. Power on Reset should be triggered (pull voltage jumper!)
    // Now, hw data ready should be high?
    hw_dr = gpio_get_level(dr_pin);
    if (hw_dr == 0) {
        ESP_LOGE("GPIO", "Error: DR pin is low after PoR");
        return;
    }
    // 2. Host clears SW_CC (writes value 0x00 to Register 0x02, Status1), which clears HW_DR.
    write_rap(dev_handle, 0x02, 0x00);
    // now hd wr should be low??
    hw_dr = gpio_get_level(dr_pin);
    if (hw_dr == 1) {
        ESP_LOGE("GPIO", "Error: DR pin is high after SW_CC clear");
        return;
    }

    // Read back the same register
    const uint8_t read_back_data = read_rap(dev_handle, 0x02, 1)[0];

    // Check if the value is set
    if (read_back_data != 0x00) {
        ESP_LOGE("I2C", "Error: Register value not set correctly");
        return;
    }

    write_rap(dev_handle, sysconfig_1_addr, SysConfig1().value);
    write_rap(dev_handle, feedconfig_2_addr, FeedConfig2().value);
    write_rap(dev_handle, feedconfig_1_addr, FeedConfig1{
                  .bits = {
                      .feed_enable = true
                  }
              }.value);

    while (true) {
        if (gpio_get_level(dr_pin)) {
            ESP_LOGI("I2C", "got data");
            const auto data = read_rap(dev_handle, 0x12, 3);
            for (int i = 0; i < data.size(); i++) {
                ESP_LOGI("I2C", "data[%d]: %s", i, to_binary(data[i]));
            }
            write_rap(dev_handle, 0x02, 0x00);
        }
    }

    ESP_LOGI("General", "End of main");
}


/*
{
    // set_color(16, 4, 2);
    init_gpios();
    init_usb();
    // set_color(0, 16, 0);

    int i = 0;
    while (true) {
        if (usb_mounted()) {
            send_pressed_keys();
            vTaskDelay(pdMS_TO_TICKS(10));
            if ((++i % 100) == 0) {
                std::cout << "Hi! " << i << std::endl;
            }
        }
    }
}
*/
