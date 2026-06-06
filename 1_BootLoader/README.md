# Bootloader Middleware Design

This bootloader is being moved toward a small MCUboot-like structure without taking a full MCUboot dependency.

## Layering

```text
App
  Bootloader core plus middleware APIs: image validation, update flow, flash/CRC/reset abstraction, transport abstraction.

Port/[platform]
  Platform-specific implementation of platform and transport operations.

Core
  Minimal application entry point. It calls platform init, then bootloader init, then stays in a dead loop if control returns.

ThirdParty/tinyAES
  AES implementation used by the bootloader update flow.
```

`App/Src/BL.c` must not call STM32 HAL APIs directly. To port to another driver stack, add a new implementation file for `BL_Platform.h` and/or `BL_Transport.h`, then exclude the HAL adapter files from the build.

## STM32CubeIDE Include Paths

Add these include paths for this bootloader project:

```text
1_BootLoader/App/Inc
1_BootLoader/Port/STM32F1_HAL/Inc
1_BootLoader/ThirdParty/tinyAES/Inc
```

Compile these source folders:

```text
1_BootLoader/Core/Src
1_BootLoader/App/Src
1_BootLoader/Port/STM32F1_HAL/Src
1_BootLoader/ThirdParty/tinyAES/Src
```

## Porting Contract

The platform layer must provide:

- Read a 32-bit flash word.
- Erase flash pages.
- Program 32-bit and 64-bit flash words.
- Calculate CRC over image words.
- Jump to an image vector table.
- Request a controlled reset.

The transport layer must provide:

- Send one UDS-like service byte.
- Receive one UDS-like service byte.
- Receive one CAN-sized frame.
- Receive a multi-frame firmware block.

## Current Implementation

The current adapter still uses STM32 HAL because the existing CubeIDE project initializes CAN, CRC, FLASH, IWDG, clock, and GPIO through Cube-generated code. The bootloader policy is now isolated from that choice.
