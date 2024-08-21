#include <led_strip.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "key_scan.h"
#include "key_maps.h"
#include "led.h"
#include "usb.h"
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

// Cirque RAP read and write commands
constexpr uint8_t write_cmd(slave_addr | write_mask);
constexpr uint8_t read_cmd(slave_addr | read_mask);

// Register config values for this demo
constexpr uint8_t sysconfig_1_data = 0x00;
constexpr uint8_t sysconfig_1_addr = 0x03;
constexpr uint8_t feedconfig_1_data = 0x81; //0x01 = relative, 0x03 = absolute, 0x80 = invert Y-axis
constexpr uint8_t feedconfig_1_addr = 0x04;
constexpr uint8_t feedconfig_2_data = 0x1C; // disable scroll, disable rtap
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


extern "C" void app_main(void) {
    ESP_LOGI("General", "Hi from the start of main");

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

    uint8_t data;
    i2c_master_transmit(dev_handle, &read_cmd, 1, -1);
    i2c_master_receive(dev_handle, &data, 1, -1);


    ESP_LOGI("General", "returning");
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
