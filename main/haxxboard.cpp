#include <led_strip.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "util.h"
#include "key_maps.h"
#include "led.h"
#include "usb.h"

uint8_t key_state[49] = {};

extern "C" void app_main(void) {
    set_color(16, 4, 2);
    init_gpios();
    init_usb();
    set_color(0, 16, 0);

    while (true) {
        if (usb_mounted()) {
            for (int i = 0; i < 49; i++) {
                key_state[i] = !gpio_get_level((gpio_num_t) i);
            }

            send_pressed_keys(keycode_map, key_state);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
