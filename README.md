# CAN Protocol Bootloader

## Overview
This project implements a Gateway application for microcontrollers. This allows firmware updates over the CAN bus to MCU in CAN network.

## Installation and Setup
1. Clone this repository:
https://github.com/DatChauThanh/FoTA.git


2. Install required tools and dependencies:
- STM32CubeIDE
- STLink Utility
- Vscode

## Bootloader Protocol
- The bootloader's purpose is to update firmware on microcontrollers.
- Safety features prevent self-erasure and ensure correct firmware writes.

## Security method
- AES-128-CBC mode using tinyAES lib.

## Error Detection method
- Application CRC: Checksum for the application firmware.

## Information:
- Node identifier: 0x50 and 0x60.
- Board name: STM32 Blue Pill 
- Model: f103c8t6

## Performance Considerations
- CAN speed affects overall performance: 500kbps
- UART speed 115200 bps.
- STM32 Clock Frequency: 72MHz.

If you have any questions, don't hesitate to ask! 
Contact: chaudat111022@gmail.com
