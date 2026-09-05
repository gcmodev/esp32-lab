# ESP32 Lab

AI-assisted electronics lab for Haekal. The current active project is **LoRa Handheld Communicator V1**.

## Current simulated hardware

- ESP32 DevKit C V4
- SSD1306 128x64 OLED
- 6 push buttons:
  - UP
  - DOWN
  - SELECT
  - BACK
  - SEND
  - RX SIM (simulates an incoming LoRa packet)
- Wokwi CI visual screenshots generated automatically after successful tests

The actual LoRa RF chip is intentionally simulated in firmware for Phase 1. This lets us finish the UI, controls, message handling, and automated tests before committing to a physical radio module.

## Controls

| Control | ESP32 pin | Keyboard key in Wokwi |
|---|---:|---|
| UP | GPIO 32 | U |
| DOWN | GPIO 33 | D |
| SELECT | GPIO 25 | E |
| BACK | GPIO 26 | B |
| SEND | GPIO 27 | S |
| RX SIM | GPIO 14 | R |
| OLED SDA | GPIO 21 | - |
| OLED SCL | GPIO 22 | - |

The future LoRa radio SPI pins are reserved and kept free from the UI controls.

## What V1 can do

- Navigate a simple handheld menu
- Choose preset messages
- Simulate transmitting a message
- Simulate receiving a message
- Show an inbox
- Change the configured 920-923 MHz test channel
- Produce screenshots automatically in the `screenshots/` folder

## Local visual test

After cloning/pulling the repository:

1. Run `pio run`
2. Open `diagram.json` using the Wokwi VS Code extension
3. Start the simulator
4. Click the virtual buttons or use the keyboard shortcuts above

## CI visual output

Every successful Wokwi run refreshes the PNG files in `screenshots/`.

See `docs/lora-handheld-v1.md` for the hardware architecture and roadmap.
