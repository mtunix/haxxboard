#ifndef LED_H
#define LED_H

#include <led_strip.h>
#include <led_strip_rmt.h>
#include <led_strip_types.h>
#include <util.h>
#include <driver/gpio.h>
#include <soc/clk_tree_defs.h>

inline led_strip_handle_t led_strip;

inline led_strip_config_t strip_config = {
    .strip_gpio_num = 48,
    .max_leds = 1,
    .led_pixel_format = LED_PIXEL_FORMAT_GRB,
    .led_model = LED_MODEL_SK6812,
    .flags = 0,
};

inline led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 10 * 1000 * 1000,
    .mem_block_symbols = 0,
    .flags = 0,
};

inline void set_color(const uint8_t r, const uint8_t g, const uint8_t b) {
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    led_strip_set_pixel(led_strip, 0, r, g, b);
    led_strip_refresh(led_strip);
}

#endif //LED_H
