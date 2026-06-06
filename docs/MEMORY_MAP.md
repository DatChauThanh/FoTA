# STM32F103C8T6 Memory Map

The current design assumes a 128 KB flash layout on STM32F103C8T6-compatible boards.

| Region | Start | Size | Purpose |
| --- | ---: | ---: | --- |
| Bootloader | `0x08000000` | 20 KB | Reset entry, image validation, CAN update receiver |
| Active application | `0x08005000` | 22 KB | Application image executed by bootloader |
| Backup / gateway staging | `0x0800A800` | 22 KB | Backup image on target bootloader, staging image on gateway |
| Reserved / future use | `0x08010000` | 63 KB | Diagnostics, metadata, larger app split, or unused |
| Flag page | `0x0801FC00` | 1 KB page | Boot flag, image status, image size, image CRC |

## Bootloader Flags

| Flag | Address | Meaning |
| --- | ---: | --- |
| Bootloader branch flag | `0x0801FC00` | `0xFFFFFFFF` = boot app, `0x00000000` = enter update mode |
| Active image status | `0x0801FC14` | active, backup, corrupt, or not existing |
| Active image size | `0x0801FC18` | image size in bytes |
| Active image CRC | `0x0801FC1C` | STM32 hardware CRC over active image words |
| Backup image status | `0x0801FC34` | backup image state |
| Backup image size | `0x0801FC38` | backup image size in bytes |
| Backup image CRC | `0x0801FC3C` | STM32 hardware CRC over backup image words |

## Linker Requirements

Bootloader linker script:

```text
FLASH ORIGIN = 0x08000000, LENGTH = 20K
RAM   ORIGIN = 0x20000000, LENGTH = 20K
```

Target application linker script:

```text
FLASH ORIGIN = 0x08005000, LENGTH = 22K
RAM   ORIGIN = 0x20000000, LENGTH = 20K
```

Every target app must also relocate the vector table to `0x08005000`, either through linker/startup configuration or early system initialization.
