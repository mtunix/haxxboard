#include <esp_log.h>
#include <class/hid/hid_device.h>
#include <driver/gpio.h>
#include <key_maps.h>

uint8_t is_invalid_gpio(uint8_t gpio) {
    return gpio == 19 // USB
           || gpio == 20 // USB
           || (gpio >= 22 && gpio <= 25) // Non-existent
           || (gpio >= 27 && gpio <= 32) // Non-existent
           || gpio == 43 // UART
           || gpio == 44; // UART
}

void init_gpio(const uint8_t gpio) {
    if (is_invalid_gpio(gpio)) return;

    const gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(gpio),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
}

void init_gpios() {
    for (int i = 0; i < 49; i++) {
        init_gpio(i);
    }
}

void send_pressed_keys() {
    std::array<uint8_t, 6> pressed_keys{};
    uint8_t pressed_keys_count = 0;

    int fn_key_state = !gpio_get_level(FN_KEY_GPIO_NUM);

    for (auto&& [gpio_num, hid_key]: key_map) {
        if (pressed_keys_count >= 6) break;

        if (!gpio_get_level(gpio_num)) {
            pressed_keys[pressed_keys_count] = hid_key[fn_key_state];
            ++pressed_keys_count;
        }
    }

    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, pressed_keys.data());
}