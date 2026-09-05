# LoRa Handheld Communicator V1

## Goal

Build a pair of compact ESP32 handheld communicators for short text/status messages over LoRa without relying on cellular service or Wi-Fi.

Phase 1 focuses on the complete user interface and message flow in Wokwi. The radio transport is simulated so that the project can be tested visually before choosing/buying the RF hardware.

## V1 interaction model

The first standalone UI uses six controls:

- UP / DOWN: menu and item navigation
- SELECT: enter the selected menu item
- BACK: return home
- SEND: transmit the selected preset message
- RX SIM: development-only button that injects a fake incoming packet

The first preset messages are:

- OK
- WHERE ARE YOU?
- ON MY WAY
- NEED HELP
- CALL ME

Free-form typing is deliberately left for a later phase. We can add either a miniature keyboard or BLE phone text entry after the core radio link works.

## RF architecture target

Target radio family: **SX1262-class LoRa transceiver** capable of operating in Indonesia's 920-923 MHz LPWAN allocation.

Provisional ESP32 radio pin reservation:

| Radio signal | ESP32 GPIO |
|---|---:|
| SCK | 18 |
| MISO | 19 |
| MOSI | 23 |
| NSS / CS | 5 |
| DIO1 | 16 |
| RESET | 17 |
| BUSY | 4 |

These pins are reserved now so the UI design will not need to be rewired later.

## Indonesia operating envelope

For the eventual physical end-node design, the project will target the Indonesian non-cellular LPWAN 920-923 MHz rules. Keputusan Menteri Kominfo No. 5 Tahun 2024 specifies, for a 920-923 MHz end node/subscriber station, RF output up to 100 mW EIRP, bandwidth up to 250 kHz, and uplink duty cycle up to 1%.

We will use conservative transmit power and airtime settings, and the final physical hardware/module must be suitable for the local technical/certification requirements.

## Proposed protocol V1

A packet will eventually contain:

- protocol version
- sender device ID
- destination device ID
- sequence number
- message type
- payload
- checksum / integrity field

Initial message types:

- TEXT_PRESET
- ACK
- PING
- STATUS

No encryption/key design is being added in this first simulator stage; that can be designed before the physical-radio phase.

## Development phases

### Phase 1 - UI simulator (current)

- OLED interface
- buttons
- preset messages
- inbox
- channel selection
- simulated TX and RX
- screenshot-based CI tests

### Phase 2 - radio model

- choose exact SX1262 module
- add RadioLib transport layer
- create a Wokwi custom-chip model or test shim for SPI/radio behavior
- simulate packet success/failure and RSSI/SNR

### Phase 3 - two-device link

- Device H1 and H2 identities
- acknowledgements
- duplicate protection
- retry logic
- message history

### Phase 4 - physical prototype

- purchase two boards/modules
- verify 920-923 MHz configuration
- conservative RF power
- antenna choice
- battery/power design
- enclosure layout

### Phase 5 - better text input

Choose one:

1. compact physical keyboard
2. BLE phone companion for typing
3. rotary/5-way character entry
4. preset-only ultra-small communicator

## FM receiver

The FM receiver idea is still a good second project. It can live in this same ESP32 lab after the LoRa communicator reaches a stable V1.
