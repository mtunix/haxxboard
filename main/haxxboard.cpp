#include <led_strip.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "key_scan.h"
#include "key_maps.h"
#include "led.h"
#include "usb.h"

extern "C" void app_main(void) {
    // set_color(16, 4, 2);
    init_gpios();
    init_usb();
    // set_color(0, 16, 0);

    while (true) {

        ESP_LOGI("se big wile", "loohohooop");
        if (usb_mounted()) {
            ESP_LOGI("se big wile", "inside ifatr");
            send_pressed_keys();
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
