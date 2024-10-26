#include <projdefs.h>

#include "esp_log.h"
#include "usb.h"
#include "key_scan.h"
#include "task.h"

#include "mouse_controller.h"

extern "C" void app_main(void) {
    init_usb();

    init_gpios();

    ESP_LOGI("General", "Hi from the start of main");
    MouseController mouse_controller;
    int i = 0;
    while (true) {
        if (usb_mounted())
            mouse_controller.tick();
        send_pressed_keys();
        vTaskDelay(pdMS_TO_TICKS(10));
        if ((++i % 100) == 0) {
            ESP_LOGI("General", "HI");
        }
    }
    ESP_LOGI("General", "End of main");
}


/*
{
    // set_color(16, 4, 2);

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
