# CAN Protocol FOTA Gateway

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

- Driver module using Default HAL Driver of ST
- CAN speed affects overall performance: 500kbps
- UART speed 115200 bps.
- STM32 Clock Frequency: 72MHz.

Thank to

- Mohamed Hafez Mohamed
- Abdelrhman Mosad Abdelhady
- Osama Salah Hijazi

Their project inspire and help me a lot in concept and implementation.
https://github.com/mohamed-hafez-mohamed/Graduation_Project_2021

If you have any questions, don't hesitate to ask!
Contact: chaudat111022@gmail.com
