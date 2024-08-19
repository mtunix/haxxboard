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


// cirque sample taken from https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Circular_Trackpad/Single_Pad_Sample_Code/I2C_FlatOverlay_USBMouse/I2C_FlatOverlay_USBMouse.ino
#define SDA_PIN   GPIO_NUM_2
#define SCL_PIN   GPIO_NUM_1
#define DR_PIN    GPIO_NUM_3

#define LED_0     21
#define LED_1     20

// Cirque's 7-bit I2C Slave Address
#define SLAVE_ADDR  0x2A

// Masks for Cirque Register Access Protocol (RAP)
#define WRITE_MASK  0x80
#define READ_MASK   0xA0

// Register config values for this demo
#define SYSCONFIG_1_DATA   0x00
#define SYSCONFIG_1_ADDR   0x03
#define FEEDCONFIG_1_DATA  0x81  //0x01 = relative, 0x03 = absolute, 0x80 = invert Y-axis
#define FEEDCONFIG_1_ADDR  0x04
#define FEEDCONFIG_2_DATA  0x1C // disable scroll, disable rtap
#define FEEDCONFIG_2_ADDR  0x05
#define Z_IDLE_COUNT  0x05
#define PACKETBYTE_0_ADDRESS 0x12

// Coordinate scaling values
#define PINNACLE_XMAX     2047    // max value Pinnacle can report for X
#define PINNACLE_YMAX     1535    // max value Pinnacle can report for Y
#define PINNACLE_X_LOWER  127     // min "reachable" X value
#define PINNACLE_X_UPPER  1919    // max "reachable" X value
#define PINNACLE_Y_LOWER  63      // min "reachable" Y value
#define PINNACLE_Y_UPPER  1471    // max "reachable" Y value
#define PINNACLE_X_RANGE  (PINNACLE_X_UPPER-PINNACLE_X_LOWER)
#define PINNACLE_Y_RANGE  (PINNACLE_Y_UPPER-PINNACLE_Y_LOWER)


extern "C" void app_main(void) {
    ESP_LOGI("General", "Hi from the start of main");


    constexpr i2c_master_bus_config_t i2c_mst_config = {
        .i2c_port = -1,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = {.enable_internal_pullup = true}
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    constexpr i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = SLAVE_ADDR,
        .scl_speed_hz = 100000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    constexpr gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(DR_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));

    bool hw_dr = gpio_get_level(DR_PIN);
    ESP_LOGI("General", "hw_dr value %d", hw_dr);

    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, SLAVE_ADDR, -1));

    // This one crashes, meaning that we see the cirque device above successfully 🥳
    //ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x11, -1));

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
