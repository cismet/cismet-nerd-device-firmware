# cismet-nerd-device-firmware: Firmware for a ICQUANZX T-Display ESP32 device

The cismet-nerd-device is a christmas gift for our staff. (ICQUANZX T-Display ESP32)

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)

# Important Note ⚠️⚠️⚠️⚠️

After first start of platformIO and the libraries are loaded got to this file:

`.pio/libdeps/ttgo-lora32-v1/TFT_eSPI/User_Setup_Select.h`

and uncomment this line

```c++
// #include <User_Setups/Setup23_TTGO_TM.h>           // Setup file for ESP32 and TTGO TM ST7789 SPI bus TFT
// #include <User_Setups/Setup24_ST7789.h>            // Setup file for DSTIKE/ESP32/ESP8266 configured for ST7789 240 x 240
#include <User_Setups/Setup25_TTGO_T_Display.h> // Setup file for ESP32 and TTGO T-Display ST7789V SPI bus TFT
// #include <User_Setups/Setup26_TTGO_T_Wristband.h>  // Setup file for ESP32 and TTGO T-Wristband ST7735 SPI bus TFT

```

![IMG_3077](https://github.com/cismet/cismet-nerd-device-firmware/assets/837211/94873de9-b696-48a9-aeaa-62c102c26328)

## Thanks

Almost everything i know about the device and also the sourcecode from the two integrated minigames comes from https://www.youtube.com/@VolosProjects

Thanks man. You rock. 🤘🏼

## Overview

The software will power the nerd device. I used PlatformIO to manage the process of building and flashing.

## Modes

The software has 4 modes:

- Startup Mode (shows QR code of the ip adress)
- Presence Mode (shows the images of my precious coworkers 😉)
- Tetris like Game (see also https://www.youtube.com/watch?v=a9YPPsduDNg)
- Space Shooter game (see also https://www.youtube.com/watch?v=N6V7ZJkhSbc)

You can switch the modes with the build webpage

<img width="730" alt="Screenshot 2024-01-13 at 13 13 34" src="https://github.com/cismet/cismet-nerd-device-firmware/assets/837211/840e853a-849a-4421-9cd3-588441f01999">

## HowTo for cismet staff

As fellow nerds, we know you'll appreciate the coolness and capabilities of this gadget, but remember, it's as delicate as our love for coding, especially the display.

Upon powering up with the included USB-C cable, the device enters Access Point Mode, creating a Wi-Fi network named `cismetFirstConnect`. It's best to connect to this network using a smartphone, as a Connection Manager should automatically open. Here, you can navigate to 'Configure WiFi' to select your home network (no spying, we promise 😇) and enter your Wi-Fi password. After saving your settings, the device will reboot and start in Startup Mode, displaying its IP address both as text and a QR code. You can scan this QR code with your camera or manually enter the IP address to access the web interface.

Through the web interface, you can switch between different modes. The Presence Mode, the original purpose of this device, visualizes the current presence of cismet colleagues by evaluating the #presence channel. During less busy times, like the holiday season, the device is set to a Demo Mode. You can change the orientation of the display with the left button (portrait/landscape), and yes, we've included Tetris and Breakout because we can 😉. The device is pre-configured, but to switch from Demo to Live Mode, you need to set the topic to `slackpresence`. If everyone is absent, the device enters a screensaver mode after 5 minutes and wakes up automatically when the first person returns. A reboot or pressing the right button will always bring you back to Startup Mode. To delete all WiFi settings (MQTT settings will remain), hold both buttons in Startup Mode for 5 seconds.

Enjoy exploring and using your Device!

## Contribution

Feel free to improve with PRs or fork the project and keep the work for you 😊

## License

This project’s code is available under a MIT license.
