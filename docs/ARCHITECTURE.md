# FOTA System Architecture

## Product Goal

This repository is the firmware workspace for a vehicle-style firmware-over-the-air chain:

```text
Cloud / PC tool
    |
ESP32 Telematic unit
    | UART, 115200 bps
STM32F103 Gateway
    | CAN, 500 kbps
STM32F103 Target bootloader
    |
Target application image
```

The practical goal is to keep the STM32 firmware maintainable inside STM32CubeIDE while preserving the existing HAL peripheral initialization flow.

## Firmware Projects

| Directory | Role | MCU | Main Interfaces |
| --- | --- | --- | --- |
| `1_BootLoader` | Target bootloader, receives encrypted firmware over CAN, writes active application region, verifies CRC, rolls back from backup image | STM32F103C8T6 | CAN, CRC, FLASH, IWDG |
| `2_Gateway` | Receives firmware from ESP32 over UART, stores it in flash, encrypts blocks, sends update to target bootloader over CAN, displays progress | STM32F103C8T6 | UART1, CAN1, I2C1 OLED, CRC, FLASH |
| `3_Lighting_System` | Example target application | STM32F103C8T6 | GPIO |
| `4_Collision_System` | Example target application | STM32F103C8T6 | GPIO, timer/sensor app layer |
| `5_Init_Blink_led` | Prebuilt initial binary images used during bring-up | Binary artifacts | N/A |

## Layering Rule

Keep CubeMX generated initialization in `main.c`, `stm32f1xx_hal_msp.c`, and `stm32f1xx_it.c`. Application logic should live in independent modules under `Src`/`Inc`.

Recommended layering:

```text
Application services
  ReceiveUpdate, Encrypt, Transmit, UserInterface, BootLoader

Runtime exchange
  Rte and Rte_*Port modules

Hardware adapters
  CAN_Interface, Flash_Interface, SSD1306, ASE

STM32Cube HAL
  HAL CAN/UART/I2C/CRC/FLASH/GPIO/IWDG drivers and generated init code
```

## Runtime Gateway Flow

1. ESP32 sends a new-update request on UART.
2. Gateway validates that the current system state is idle.
3. Gateway receives firmware header and stores firmware bytes into the staging flash region.
4. Gateway encrypts 16-byte blocks with AES-128-CBC.
5. Gateway sends header and encrypted packets to the target bootloader over CAN.
6. Target bootloader decrypts packets, programs the active application region, verifies CRC, and resets into the application.

## Runtime Bootloader Flow

1. On reset, bootloader reads the boot flag and image status flags from the flag page.
2. If no update is requested, it verifies active image CRC and jumps to the active application.
3. If active image is corrupt, it restores from backup when available.
4. If update is requested, it negotiates with the gateway, receives header and firmware packets, writes flash, marks the image active, and resets.

## Current Engineering Priorities

- Keep all flash addresses centralized and documented before adding more boards.
- Keep protocol constants consistent between gateway and bootloader.
- Replace blocking waits with timeout-based error handling before using this outside a lab bench.
- Move AES keys out of source before any real deployment.
- Add linker scripts and `.ioc` files for repeatable STM32CubeIDE import/build.
