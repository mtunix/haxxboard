#include <class/hid/hid_device.h>
#include <driver/gpio.h>

uint8_t is_invalid_gpio(uint8_t gpio) {
    return gpio == 19 // USB
           || gpio == 20 // USB
           || (gpio >= 22 && gpio <= 25) // Non-existent
           || (gpio >= 27 && gpio <= 32) // Non-existent
           || gpio == 43 // UART
           || gpio == 44; // UART
}

void init_gpios() {
    for (int i = 0; i < 49; i++) {
        if (is_invalid_gpio(i)) continue;

        gpio_config_t gpio_cfg = {
            .pin_bit_mask = BIT64(i),
            .mode = GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_DISABLE,
            .pull_up_en = true,
            .pull_down_en = false,
        };
        ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    }
}

void send_pressed_keys(const uint8_t *keycode_map, const uint8_t *key_state) {
    uint8_t pressed_keys[6] = {};
    uint8_t pressed_keys_count = 0;

    for (int i = 0; i < 49; i++) {
        if (pressed_keys_count >= 6) break;

        if (key_state[i]) {
            pressed_keys[pressed_keys_count] = keycode_map[i];
            ++pressed_keys_count;
        }
    }

    if (pressed_keys_count > 0)
        tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, pressed_keys);
    else
        tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);
}
