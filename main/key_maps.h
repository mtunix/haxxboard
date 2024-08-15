#pragma once
#include <class/hid/hid.h>

#include <unordered_map>
#include <array>

constexpr gpio_num_t FN_KEY_GPIO_NUM = GPIO_NUM_48;

static const std::unordered_map<gpio_num_t, std::array<int, 2>> key_map = {
    {GPIO_NUM_0, {HID_KEY_X, HID_KEY_NONE}}, // gpio 0 is also the boot strapping key, wired to the boot button on the esp32 s3 devkit m
    // 0 - 2 Joystick (VCC / X / Y ADC pins)
    {GPIO_NUM_3, {HID_KEY_GRAVE, HID_KEY_NONE}},
    {GPIO_NUM_4, {HID_KEY_1, HID_KEY_F1}},
    {GPIO_NUM_5, {HID_KEY_2, HID_KEY_F2}},
    {GPIO_NUM_6, {HID_KEY_3, HID_KEY_F3}},
    {GPIO_NUM_7, {HID_KEY_4, HID_KEY_F4}},
    {GPIO_NUM_8, {HID_KEY_5, HID_KEY_F5}},
    {GPIO_NUM_9, {HID_KEY_6, HID_KEY_F6}},
    {GPIO_NUM_10, {HID_KEY_Q, HID_KEY_NONE}},
    {GPIO_NUM_11, {HID_KEY_W, HID_KEY_ARROW_UP}},
    {GPIO_NUM_12, {HID_KEY_E, HID_KEY_NONE}},
    {GPIO_NUM_13, {HID_KEY_R, HID_KEY_NONE}},
    {GPIO_NUM_14, {HID_KEY_T, HID_KEY_NONE}},
    {GPIO_NUM_15, {HID_KEY_A, HID_KEY_ALT_LEFT}},
    {GPIO_NUM_16, {HID_KEY_S, HID_KEY_ARROW_DOWN}},
    {GPIO_NUM_17, {HID_KEY_D, HID_KEY_ARROW_RIGHT}},
    {GPIO_NUM_18, {HID_KEY_F, HID_KEY_NONE}},
    // 19 - 20 USB
    {GPIO_NUM_21, {HID_KEY_G, HID_KEY_NONE}},
    // 22 - 25 Non-existent
    {GPIO_NUM_26, {HID_KEY_Z, HID_KEY_NONE}},
    // 27 - 32 Non-existent
    {GPIO_NUM_33, {HID_KEY_X, HID_KEY_NONE}},
    {GPIO_NUM_34, {HID_KEY_C, HID_KEY_NONE}},
    {GPIO_NUM_35, {HID_KEY_V, HID_KEY_NONE}},
    {GPIO_NUM_36, {HID_KEY_B, HID_KEY_NONE}},
    {GPIO_NUM_37, {HID_KEY_TAB, HID_KEY_NONE}},
    {GPIO_NUM_38, {HID_KEY_ESCAPE, HID_KEY_CAPS_LOCK}},
    {GPIO_NUM_39, {HID_KEY_SHIFT_LEFT, HID_KEY_NONE}},
    {GPIO_NUM_40, {HID_KEY_CONTROL_LEFT, HID_KEY_NONE}},
    {GPIO_NUM_41, {HID_KEY_GUI_LEFT, HID_KEY_NONE}},
    {GPIO_NUM_42, {HID_KEY_ALT_LEFT, HID_KEY_NONE}},
    // 43 - 44 UART
    {GPIO_NUM_45, {HID_KEY_SPACE, HID_KEY_NONE}},
    {GPIO_NUM_46, {HID_KEY_NONE, HID_KEY_NONE}},
    {GPIO_NUM_47, {HID_KEY_NONE, HID_KEY_NONE}},
    // 48 is for Fn
};