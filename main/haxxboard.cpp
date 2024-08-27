#include <algorithm>
#include <cirque_pinnacle.h>
#include <led_strip.h>
#include <limits>

#include "esp_log.h"
#include "led.h"
#include "usb.h"
#include <class/hid/hid_device.h>
#include <sys/stat.h>



constexpr gpio_num_t touchpad_sda_pin = GPIO_NUM_2;
constexpr gpio_num_t touchpad_scl_pin = GPIO_NUM_1;
constexpr gpio_num_t touchpad_data_ready_pin = GPIO_NUM_3;

extern "C" void app_main(void) {
    ESP_LOGI("General", "Hi from the start of main");

    const TouchPad touch_pad{touchpad_sda_pin, touchpad_scl_pin, touchpad_data_ready_pin};

    uint16_t xmin = std::numeric_limits<uint16_t>::max(), xmax = 0;
    uint16_t ymin = std::numeric_limits<uint16_t>::max(), ymax = 0;

    int samples_left = 10000;
    while (samples_left > 0) {
        if (const auto touch_data = touch_pad.get_data()) {
            if (touch_data->z == 0) {
                continue;
            }

            xmin = std::min(xmin, touch_data->x);
            xmax = std::max(xmax, touch_data->x);
            ymin = std::min(ymin, touch_data->y);
            ymax = std::max(ymax, touch_data->y);
            ESP_LOGI("loop", "samples left: %d", samples_left);
            --samples_left;
        }
    }

    ESP_LOGI("General", "xmin: %d", xmin);
    ESP_LOGI("General", "xmax: %d", xmax);
    ESP_LOGI("General", "ymin: %d", ymin);
    ESP_LOGI("General", "ymax: %d", ymax);

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
