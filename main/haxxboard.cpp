#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "util.h"
#include "key_maps.h"
#include "usb.h"

uint8_t key_state[49] = {};

extern "C" void app_main(void) {
    init_gpios();
    init_usb();

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
