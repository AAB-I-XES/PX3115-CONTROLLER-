<div align="center">

# 🎮 ESP32-S3 Xbox-Style Game Controller

### A fully simulated wireless + wired game controller, built for Wokwi

[![Platform](https://img.shields.io/badge/platform-ESP32--S3-blue?style=for-the-badge&logo=espressif&logoColor=white)](https://www.espressif.com/en/products/socs/esp32-s3)
[![Simulator](https://img.shields.io/badge/simulator-Wokwi-orange?style=for-the-badge)](https://wokwi.com)
[![Connectivity](https://img.shields.io/badge/connectivity-USB--C%20%2B%20BLE-brightgreen?style=for-the-badge)](#connectivity)
[![License](https://img.shields.io/badge/license-MIT-lightgrey?style=for-the-badge)](#license)

No physical hardware required — the entire circuit and firmware run in-browser via Wokwi.

</div>

---

## Table of contents

- [Overview](#overview)
- [Features](#features)
- [Architecture](#architecture)
- [Pin mapping](#pin-mapping)
- [Project structure](#project-structure)
- [Getting started](#getting-started)
- [How the simulation works](#how-the-simulation-works)
- [Moving to real hardware](#moving-to-real-hardware)
- [Roadmap](#roadmap)
- [License](#license)

---

## Overview

<table>
<tr>
<td width="60%">

This project simulates a complete **Xbox-style game controller** — two analog sticks, a D-pad, ABXY buttons, shoulder buttons, analog triggers, menu buttons, rumble motors, and a status LED — entirely inside [Wokwi](https://wokwi.com), no soldering iron required.

It's built around the **ESP32-S3**, chosen because it has native USB and built-in Bluetooth LE, so the same chip can act as a wired *or* wireless HID gamepad.

</td>
<td width="40%" align="center">

```
┌─────────────────────────┐
│   ┌───┐         ┌───┐   │
│   │ ⊕ │  ⌒ ⌒    │ ⊕ │   │
│   └───┘  ⌒ ⌒    └───┘   │
│                          │
│   ▲                     │
│  ◄ ►      ≡  ≡     Y    │
│   ▼                X  B │
│                     A    │
└─────────────────────────┘
```

</td>
</tr>
</table>

## Features

| | |
|---|---|
| 🕹️ **Dual analog sticks** | X/Y analog read + click (L3/R3) |
| 🎯 **Analog triggers** | LT/RT via potentiometer simulation |
| 🔘 **Full button set** | D-pad, ABXY, LB/RB, Start/Back/Guide/Share |
| 📳 **Rumble feedback** | PWM-driven, LED stand-in in simulation |
| 💡 **Status indicator** | Addressable RGB LED reacts to stick input |
| 🔌 **Dual connectivity** | Native USB-C wired mode + BLE wireless mode |
| 🧪 **Zero hardware needed** | Runs entirely in Wokwi's browser simulator |

## Architecture

```
┌──────────────────────────────────────────────┐
│                Controller PCB                 │
│                                                │
│  ┌─────────────┐        ┌──────────────────┐  │
│  │Power system │──────▶│    MCU core       │  │
│  │USB-C+battery│        │ ESP32-S3, USB+BLE │  │
│  └─────────────┘        └────────┬─────────┘  │
│                                   ▲             │
│                                   │             │
│                        ┌──────────────────┐    │
│                        │   Input matrix    │    │
│                        │ sticks · buttons  │    │
│                        │     · triggers    │    │
│                        └──────────────────┘    │
└──────────────────────────────────────────────┘
```

The MCU reads the input matrix continuously, and — on real hardware — encodes state into a USB or BLE HID gamepad report. In simulation, the same state is printed to Serial instead, so the logic is identical either way.

## Pin mapping

<details>
<summary><b>Click to expand full GPIO table</b></summary>

| Function | GPIO | Type |
|---|---|---|
| Left stick X | 1 | ADC |
| Left stick Y | 2 | ADC |
| Right stick X | 4 | ADC |
| Right stick Y | 5 | ADC |
| LT trigger | 6 | ADC |
| RT trigger | 7 | ADC |
| L3 (left stick click) | 8 | Digital, pull-up |
| R3 (right stick click) | 9 | Digital, pull-up |
| D-pad Up / Down / Left / Right | 10 – 13 | Digital, pull-up |
| A / B / X / Y | 14 – 17 | Digital, pull-up |
| LB / RB | 18 / 21 | Digital, pull-up |
| Start / Back / Guide / Share | 33 – 36 | Digital, pull-up |
| Rumble L / Rumble R | 37 / 38 | PWM out |
| Status LED (NeoPixel data) | 39 | Digital out |
| USB D+ / D− | 19 / 20 | Reserved — native USB |

</details>

## Project structure

```
.
├── sketch.ino        # Firmware — reads inputs, drives outputs, prints state
├── diagram.json       # Wokwi virtual circuit — sticks, buttons, LEDs, board
└── README.md          # You are here
```

## Getting started

> **Prerequisite:** a free [wokwi.com](https://wokwi.com) account. No local install needed.

1. Go to **wokwi.com** → **New Project** → **ESP32-S3** (starter template).
2. Open the `sketch.ino` tab and paste in this repo's `sketch.ino`.
3. Open the `diagram.json` tab and paste in this repo's `diagram.json`.
4. If prompted for a missing library, add **Adafruit NeoPixel**.
5. Press ▶ **Run**, then open the **Serial Monitor**.
6. Drag the joysticks, turn the potentiometers, and click the buttons in the diagram view — watch the live state print in Serial.

```cpp
LX: 2048 LY: 2048 RX: 2048 RY: 2048 LT:    0 RT:    0 |
Dpad U:0 D:0 L:0 R:0 | ABXY:0000 | LB:0 RB:0 L3:0 R3:0 |
Start:0 Back:0 Guide:0 Share:0
```

## How the simulation works

| Real hardware | Wokwi simulation |
|---|---|
| Analog joystick module | `wokwi-joystick` virtual part |
| Hall-effect trigger | `wokwi-potentiometer` |
| Tactile switches | `wokwi-pushbutton` |
| ERM rumble motor + driver | PWM signal → `wokwi-led` (visual stand-in) |
| WS2812B status LED | `wokwi-neopixel` |
| USB/BLE HID report | Printed to Serial (no virtual host to pair with) |

Every line of input-reading logic is identical between simulation and real hardware — only the final output step (`driveOutputs()` in `sketch.ino`) changes.

## Moving to real hardware

When you're ready to build this for real:

- Swap the rumble LEDs for a real motor driver (e.g. `DRV8833`) feeding two ERM motors.
- Add a USB-C receptacle with 5.1 kΩ CC resistors, a LiPo charge IC, and a 3.3V regulator.
- Replace `printState()` with an actual HID library call — for example [`ESP32-BLE-Gamepad`](https://github.com/lemmingDev/ESP32-BLE-Gamepad) for wireless, or the Arduino-ESP32 core's native `USBHIDGamepad` for wired.
- Everything upstream of that — reading sticks, triggers, and buttons — needs no changes.

## Roadmap

- [ ] Add deadzone + calibration logic for analog sticks
- [ ] Add BLE HID output for real-device testing
- [ ] Add USB HID output for real-device testing
- [ ] Battery level reporting via ADC voltage divider
- [ ] Enclosure / physical layout design

## License

MIT — use it, fork it, break it, build a real one.

<div align="center">

Made for tinkering in a browser tab. No solder required.

</div>
