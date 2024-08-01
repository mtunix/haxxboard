# Haxxboard

Keyboard and mouse firmware from hackers for hackers with <3

| Supported Targets |  ESP32-S2  |  ESP32-S3  |
|-------------------|:----------:|:----------:|
|                   |            |     x      |  

## References

![Pinout DevKit M1](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/_images/ESP32-S3_DevKitM-1_pinlayout.jpg)

[Schematics DevKit M1](https://dl.espressif.com/dl/schematics/SCH_ESP32-S3-DEVKITM-1_V1_20210310A.pdf)

[Technical Reference DevKit M1](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf#bootctrl)

## Troubleshooting

### UART BROKEN

If you accidentally programmed the UART pins use the following key combination to enter the flash mode via USB:

- Hold down the BOOT button
- Press and release the RST button
- Release the BOOT button

Then flash.

# TinyUSB Human Interface Device Example

Human interface devices (HID) are one of the most common USB devices, it is implemented in various devices such as keyboards, mice, game controllers, sensors and alphanumeric display devices.
In this example, we implement USB keyboard and mouse.
Upon connection to USB host (PC), the example application will send 'key a/A pressed & released' events and move mouse in a square trajectory. To send these HID reports again, press the BOOT button, that is present on most ESP development boards (GPIO0).

As a USB stack, a TinyUSB component is used.

## How to use example

### Hardware Required

Any ESP board that have USB-OTG supported.

#### Pin Assignment

_Note:_ In case your board doesn't have micro-USB connector connected to USB-OTG peripheral, you may have to DIY a cable and connect **D+** and **D-** to the pins listed below.

See common pin assignments for USB Device examples from [upper level](../../README.md#common-pin-assignments).

Boot signal (GPIO0) is used to send HID reports to USB host.

## Project setup
1. Have ESP32
2. Install `esp-idf`, follow the [official setup guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html), in short:
   ```bash
   sudo apt-get install git wget flex bison gperf python3 python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0
   git clone --recursive https://github.com/espressif/esp-idf.git
   cd esp-idf
   ./install.sh esp32
   . ./export.sh
   cd ..
   ```
3. The IDF requires certain paths, set by sourcing the `export.sh` script. Keep this in mind when opening a new terminal. An IDE like CLion can use that environment file as well.
4. Add the current user to the `dialout` linux group (to access the serial port):
    1. `sudo usermod -a -G dialout $USER`
    2. reboot

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```bash
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

After the flashing you should see the output at idf monitor:

```
I (290) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (310) example: USB initialization
I (310) tusb_desc:
┌─────────────────────────────────┐
│  USB Device Descriptor Summary  │
├───────────────────┬─────────────┤
│bDeviceClass       │ 0           │
├───────────────────┼─────────────┤
│bDeviceSubClass    │ 0           │
├───────────────────┼─────────────┤
│bDeviceProtocol    │ 0           │
├───────────────────┼─────────────┤
│bMaxPacketSize0    │ 64          │
├───────────────────┼─────────────┤
│idVendor           │ 0x303a      │
├───────────────────┼─────────────┤
│idProduct          │ 0x4004      │
├───────────────────┼─────────────┤
│bcdDevice          │ 0x100       │
├───────────────────┼─────────────┤
│iManufacturer      │ 0x1         │
├───────────────────┼─────────────┤
│iProduct           │ 0x2         │
├───────────────────┼─────────────┤
│iSerialNumber      │ 0x3         │
├───────────────────┼─────────────┤
│bNumConfigurations │ 0x1         │
└───────────────────┴─────────────┘
I (480) TinyUSB: TinyUSB Driver installed
I (480) example: USB initialization DONE
I (2490) example: Sending Keyboard report
I (3040) example: Sending Mouse report
```
