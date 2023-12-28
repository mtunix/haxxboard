#pragma once
#include <stdint.h>
#include <class/hid/hid.h>

uint8_t keycode_map_left[49] = {
    HID_KEY_GRAVE,
    HID_KEY_1,
    HID_KEY_2,
    HID_KEY_3,
    HID_KEY_4,
    HID_KEY_5,
    HID_KEY_6,
    HID_KEY_Q,
    HID_KEY_W,
    HID_KEY_E,
    HID_KEY_R,
    HID_KEY_T,
    HID_KEY_A,
    HID_KEY_S,
    HID_KEY_D,
    HID_KEY_F,
    HID_KEY_G,
    HID_KEY_Z,
    HID_KEY_X,
    0, // 19
    0, // 20
    HID_KEY_C,
    0, // 22
    0, // 23
    0, // 24
    0, // 25
    HID_KEY_V,
    0, // 27
    0, // 28
    0, // 29
    0, // 30
    0, // 31
    0, // 32
    HID_KEY_B,
    HID_KEY_TAB,
    HID_KEY_ESCAPE,
    HID_KEY_SHIFT_LEFT,
    HID_KEY_CONTROL_LEFT,
    HID_KEY_GUI_LEFT,
    HID_KEY_ALT_LEFT,
    HID_KEY_SPACE,
    HID_KEY_NONE,
    HID_KEY_NONE,
    0, // 43
    0, // 44
    HID_KEY_NONE,
    0, // 46, Joystick
    0, // 47, Joystick
    0, // 48, Joystick
};

uint8_t keycode_map_left_fn[49] = {
    HID_KEY_NONE,
    HID_KEY_F1,
    HID_KEY_F2,
    HID_KEY_F3,
    HID_KEY_F4,
    HID_KEY_F5,
    HID_KEY_F6,
    HID_KEY_NONE, // q
    HID_KEY_ARROW_UP, // w
    HID_KEY_NONE, // e
    HID_KEY_NONE, // r
    HID_KEY_NONE, // t
    HID_KEY_ARROW_LEFT, // a
    HID_KEY_ARROW_DOWN, // s
    HID_KEY_ARROW_RIGHT, // d
    HID_KEY_NONE, // f
    HID_KEY_NONE, // g
    HID_KEY_NONE, // z
    HID_KEY_NONE, // x
    0, // 19
    0, // 20
    HID_KEY_NONE, // c
    0, // 22
    0, // 23
    0, // 24
    0, // 25
    HID_KEY_NONE, // v
    0, // 27
    0, // 28
    0, // 29
    0, // 30
    0, // 31
    0, // 32
    HID_KEY_NONE, // b
    HID_KEY_CAPS_LOCK,
};

uint8_t *keycode_map = keycode_map_left;
uint8_t *keycode_map_fn = keycode_map_left_fn;