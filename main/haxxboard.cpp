#include <cirque_pinnacle.h>
#include <led_strip.h>
#include "esp_log.h"
#include "led.h"
#include "usb.h"
#include <class/hid/hid_device.h>
#include <sys/stat.h>


// cirque sample adapted from https://github.com/cirque-corp/Cirque_Pinnacle_1CA027/blob/master/Circular_Trackpad/Single_Pad_Sample_Code/I2C_FlatOverlay_USBMouse/I2C_FlatOverlay_USBMouse.ino
constexpr gpio_num_t sda_pin = GPIO_NUM_2;
constexpr gpio_num_t scl_pin = GPIO_NUM_1;
constexpr gpio_num_t dr_pin = GPIO_NUM_3;

// Coordinate scaling values
constexpr uint16_t pinnacle_xmax = 2047; // max value Pinnacle can report for X
constexpr uint16_t pinnacle_ymax = 1535; // max value Pinnacle can report for Y
constexpr uint16_t pinnacle_x_lower = 127; // min "reachable" X value
constexpr uint16_t pinnacle_x_upper = 1919; // max "reachable" X value
constexpr uint16_t pinnacle_y_lower = 63; // min "reachable" Y value
constexpr uint16_t pinnacle_y_upper = 1471; // max "reachable" Y value
constexpr uint16_t pinnacle_x_range = (pinnacle_x_upper - pinnacle_x_lower);
constexpr uint16_t pinnacle_y_range = (pinnacle_y_upper - pinnacle_y_lower);


extern "C" [[noreturn]] void app_main(void) {
    ESP_LOGI("General", "Hi from the start of main");

    const TouchPad touch_pad{sda_pin, scl_pin, dr_pin};

    while (true) {
        if (const auto touch_data = touch_pad.get_data()) {
            ESP_LOGI("loop", "got data");
            ESP_LOGI("loop", "x: %d", touch_data->x);
            ESP_LOGI("loop", "y: %d", touch_data->y);
            ESP_LOGI("loop", "z: %d", touch_data->z);
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
