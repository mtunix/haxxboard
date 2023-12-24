/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "class/hid/hid_device.h"
#include "driver/gpio.h"

#define APP_BUTTON (GPIO_NUM_0) // Use BOOT signal by default
static const char *TAG = "example";

/************* TinyUSB descriptors ****************/

#define TUSB_DESC_TOTAL_LEN      (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

/**
 * @brief HID report descriptor
 *
 * In this example we implement Keyboard + Mouse HID device,
 * so we must define both report descriptors
 */
const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_ITF_PROTOCOL_KEYBOARD)),
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(HID_ITF_PROTOCOL_MOUSE))
};

/**
 * @brief String descriptor
 */
const char *hid_string_descriptor[5] = {
    // array of pointer to string descriptors
    (char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    "TinyUSB", // 1: Manufacturer
    "TinyUSB Device", // 2: Product
    "123456", // 3: Serials, should use chip ID
    "Example HID interface", // 4: HID
};

/**
 * @brief Configuration descriptor
 *
 * This is a simple configuration descriptor that defines 1 configuration and 1 HID interface
 */
static const uint8_t hid_configuration_descriptor[] = {
    // Configuration number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, boot protocol, report descriptor len, EP In address, size & polling interval
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};

/********* TinyUSB HID callbacks ***************/

// Invoked when received GET HID REPORT DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance) {
    // We use only one interface and one HID report descriptor, so we can ignore parameter 'instance'
    return hid_report_descriptor;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
}

/********* Application ***************/

typedef enum {
    MOUSE_DIR_RIGHT,
    MOUSE_DIR_DOWN,
    MOUSE_DIR_LEFT,
    MOUSE_DIR_UP,
    MOUSE_DIR_MAX,
} mouse_dir_t;

#define DISTANCE_MAX        125
#define DELTA_SCALAR        5

static void mouse_draw_square_next_delta(int8_t *delta_x_ret, int8_t *delta_y_ret) {
    static mouse_dir_t cur_dir = MOUSE_DIR_RIGHT;
    static uint32_t distance = 0;

    // Calculate next delta
    if (cur_dir == MOUSE_DIR_RIGHT) {
        *delta_x_ret = DELTA_SCALAR;
        *delta_y_ret = 0;
    } else if (cur_dir == MOUSE_DIR_DOWN) {
        *delta_x_ret = 0;
        *delta_y_ret = DELTA_SCALAR;
    } else if (cur_dir == MOUSE_DIR_LEFT) {
        *delta_x_ret = -DELTA_SCALAR;
        *delta_y_ret = 0;
    } else if (cur_dir == MOUSE_DIR_UP) {
        *delta_x_ret = 0;
        *delta_y_ret = -DELTA_SCALAR;
    }

    // Update cumulative distance for current direction
    distance += DELTA_SCALAR;
    // Check if we need to change direction
    if (distance >= DISTANCE_MAX) {
        distance = 0;
        cur_dir++;
        if (cur_dir == MOUSE_DIR_MAX) {
            cur_dir = 0;
        }
    }
}

static void app_send_hid_demo(void) {
    // Keyboard output: Send key 'a/A' pressed and released
    ESP_LOGI(TAG, "Sending Keyboard report");
    uint8_t keycode[6] = {HID_KEY_A};
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, keycode);
    vTaskDelay(pdMS_TO_TICKS(50));
    tud_hid_keyboard_report(HID_ITF_PROTOCOL_KEYBOARD, 0, NULL);

    // Mouse output: Move mouse cursor in square trajectory
    ESP_LOGI(TAG, "Sending Mouse report");
    int8_t delta_x;
    int8_t delta_y;
    for (int i = 0; i < (DISTANCE_MAX / DELTA_SCALAR) * 4; i++) {
        // Get the next x and y delta in the draw square pattern
        mouse_draw_square_next_delta(&delta_x, &delta_y);
        tud_hid_mouse_report(HID_ITF_PROTOCOL_MOUSE, 0x00, delta_x, delta_y, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

uint8_t keycode_map[49] = {
    HID_KEY_A,
    HID_KEY_B,
    HID_KEY_C,
    HID_KEY_D,
    HID_KEY_E,
    HID_KEY_F,
    HID_KEY_G,
    HID_KEY_H,
    HID_KEY_I,
    HID_KEY_J,
    HID_KEY_K,
    HID_KEY_L,
    HID_KEY_M,
    HID_KEY_N,
    HID_KEY_O,
    HID_KEY_P,
    HID_KEY_Q,
    HID_KEY_R,
    HID_KEY_S,
    HID_KEY_T,
    HID_KEY_U,
    HID_KEY_V,
    HID_KEY_W,
    HID_KEY_X,
    HID_KEY_Y,
    HID_KEY_Z,
    HID_KEY_1,
    HID_KEY_2,
    HID_KEY_3,
    HID_KEY_4,
    HID_KEY_5,
    HID_KEY_6,
    HID_KEY_7,
    HID_KEY_8,
    HID_KEY_9,
    HID_KEY_0,
};

uint8_t key_state[49] = {};

uint8_t is_invalid_gpio(uint8_t gpio) {
    return gpio == 19
           || gpio == 20
           || (gpio >= 22 && gpio <= 25)
           || (gpio >= 27 && gpio <= 32)
           || gpio == 43
           || gpio == 44;
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

void send_pressed_keys(void) {
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

void app_main(void) {
    init_gpios();

    ESP_LOGI(TAG, "USB initialization");
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = hid_string_descriptor,
        .string_descriptor_count = sizeof(hid_string_descriptor) / sizeof(hid_string_descriptor[0]),
        .external_phy = false,
        .configuration_descriptor = hid_configuration_descriptor,
    };

    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
    ESP_LOGI(TAG, "USB initialization DONE");

    while (1) {
        if (tud_mounted()) {
            for (int i = 0; i < 49; i++) {
                key_state[i] = !gpio_get_level(i);
            }
            send_pressed_keys();
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}
