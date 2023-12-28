#pragma once
void init_gpios();
void init_gpio(uint8_t gpio);
void send_pressed_keys(const uint8_t *keycode_map, const uint8_t *key_state);
