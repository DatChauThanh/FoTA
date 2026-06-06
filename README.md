# ESP32 to STM32 CAN FOTA

Firmware-over-the-air workspace for this update chain:

```text
ESP32 Telematic -> STM32F103 Gateway -> STM32F103 Target Bootloader -> Target App
```

The STM32 side is intended to be developed with STM32CubeIDE and the STM32Cube HAL drivers. Cube-generated peripheral initialization stays in the normal HAL files, while the application logic is split into gateway, bootloader, transport, flash, encryption, RTE, and UI modules.

## Repository Layout

| Path | Purpose |
| --- | --- |
| `1_BootLoader` | Target bootloader that receives firmware over CAN, programs flash, validates CRC, and jumps to the application |
| `2_Gateway` | STM32 gateway that receives firmware from ESP32 over UART, stages it in flash, encrypts it, and forwards it over CAN |
| `3_Lighting_System` | Example target application image |
| `4_Collision_System` | Example target application image |
| `5_Init_Blink_led` | Initial prebuilt binary images used during bring-up |
| `docs` | Architecture, protocol, memory map, and STM32CubeIDE setup notes |
| `Ref` | Datasheets, reports, and reference PDFs |

## Hardware Baseline

- STM32 board: STM32F103C8T6 / Blue Pill class board
- STM32 clock: 72 MHz from 8 MHz HSE
- Gateway UART: USART1, 115200 bps, 8N1
- CAN speed: 500 kbps
- Gateway display: SSD1306 over I2C1
- Integrity: STM32 hardware CRC
- Encryption: AES-128-CBC using tiny-AES style API

## Key Documentation

- [System architecture](docs/ARCHITECTURE.md)
- [Memory map](docs/MEMORY_MAP.md)
- [FOTA protocol](docs/FOTA_PROTOCOL.md)
- [STM32CubeIDE and HAL setup](docs/STM32CUBEIDE_HAL_SETUP.md)

## Development Workflow

1. Open or create the relevant STM32CubeIDE project.
2. Configure peripherals according to `docs/STM32CUBEIDE_HAL_SETUP.md`.
3. Keep generated HAL initialization in CubeMX `USER CODE` regions.
4. Keep application behavior inside the existing modules under each firmware directory.
5. Build the target application with flash origin `0x08005000`.
6. Generate the binary, compute/store size and CRC, then send it through ESP32 -> gateway -> bootloader.

## Current Status

This repo has working firmware modules from the original graduation project, plus project documentation and cleanup for continued HAL-based development. It is still missing repeatable CubeIDE metadata (`.ioc`, `.project`, linker scripts) and a host-side firmware packer, so those should be the next engineering tasks before treating this as a production-ready workspace.

## Credits

Inspired by concepts and implementation ideas from:

- Mohamed Hafez Mohamed
- Abdelrhman Mosad Abdelhady
- Osama Salah Hijazi
- https://github.com/mohamed-hafez-mohamed/Graduation_Project_2021
