# ESP32 Lab

AI-assisted ESP32 hardware lab for Haekal.

## First integration target

This repository currently contains a minimal Wokwi/PlatformIO project:

- ESP32 DevKit C V4
- SSD1306 128x64 OLED over I2C
- Push button on GPIO 27
- Serial status output for automated testing
- Wokwi automation scenario that presses/releases the button
- GitHub Actions workflow that builds the firmware and runs the Wokwi simulation

## Pin map

| Function | ESP32 pin |
|---|---:|
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| Button | GPIO 27 |
| OLED power | 3.3V |
| Ground | GND |

## Required GitHub secret

The Wokwi simulation job needs one repository secret:

`WOKWI_CLI_TOKEN`

Create a token from the Wokwi CI dashboard, then add it in:

**GitHub repository → Settings → Secrets and variables → Actions → New repository secret**

Do not commit the token into this repository.

## Expected automated test

The firmware prints:

- `BOOT:ESP32_LAB`
- `OLED_OK`
- `LAB_READY`

The Wokwi scenario then presses and releases the virtual button and expects:

- `BUTTON:PRESSED`
- `BUTTON:RELEASED`

Once this integration passes, this repository becomes the base lab for the larger ESP32 radio project.
