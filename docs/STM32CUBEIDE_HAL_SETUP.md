# STM32CubeIDE / HAL Setup

## Toolchain

- STM32CubeIDE
- STM32CubeMX bundled with STM32CubeIDE
- STM32F1 HAL package
- ST-LINK or compatible SWD probe

## Board Assumption

- MCU: STM32F103C8T6 / Blue Pill class board
- HSE: 8 MHz external crystal
- SYSCLK: 72 MHz
- CAN: 500 kbps
- UART: USART1, 115200 bps

## Gateway Peripherals

Enable these peripherals in CubeMX for `2_Gateway`:

| Peripheral | Configuration |
| --- | --- |
| RCC | HSE on, LSI on |
| SYS | Serial Wire debug |
| CAN1 | Normal mode, 500 kbps timing matching existing `MX_CAN_Init` |
| USART1 | 115200, 8N1, TX/RX |
| I2C1 | 400 kHz for SSD1306 OLED |
| CRC | Enabled |
| GPIO | LED output, OK/SWITCH buttons with pull-up |

## Target Bootloader Peripherals

Enable these peripherals for `1_BootLoader`:

| Peripheral | Configuration |
| --- | --- |
| RCC | HSE on, LSI on |
| SYS | Serial Wire debug |
| CAN1 | Normal mode, 500 kbps |
| CRC | Enabled |
| IWDG | Used for controlled software reset path |
| FLASH | HAL flash programming APIs |

## Target Application Requirements

Every target application must:

1. Use linker `FLASH` origin `0x08005000`.
2. Keep image size within 22 KB unless the memory map is changed consistently.
3. Relocate vector table to `0x08005000`.
4. Avoid writing the bootloader, backup, and flag regions.

## Import Strategy

The current repository stores source files but does not yet include complete `.ioc`, `.project`, `.cproject`, or linker script files. Until those are added:

1. Create a fresh STM32CubeIDE project for each firmware directory.
2. Configure peripherals as listed above.
3. Copy or link the corresponding source folders into the CubeIDE project. For bootloader and gateway, compile `Core/Src/main.c`, `App/Src`, `Port/STM32F1_HAL/Src`, the needed `ThirdParty/*/Src` folders, and `Startup`.
4. Add tiny-AES sources to the include/source path if AES is used.
5. Replace the generated linker script memory regions with the values in `docs/MEMORY_MAP.md`.
6. Build each project independently and export `.bin` images for application update testing.

## Definition of Done for the Next Cleanup Pass

- Add `.ioc` files for bootloader, gateway, and target apps.
- Add linker scripts for bootloader and app image regions.
- Add a repeatable build command or CI job.
- Add a host-side firmware packer that outputs size, CRC, node id, and binary payload.
