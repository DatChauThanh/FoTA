# CAN Protocol Bootloader

## Overview
This project implements a CAN protocol bootloader for microcontrollers. The bootloader allows firmware updates over the CAN bus without disassembly or additional connections.

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

## Information:
- Node identifier: Unique ID for the microcontroller.
- Board name: STM32 Blue Pill 
- Model: f103c8t6
- Application CRC: Checksum for the application firmware.

## Performance Considerations
- CAN speed affects overall performance: 500kbps
- Flash write chunk size impacts efficiency.

If you have any questions, don't hesitate to ask! 
Contact: chaudat111022@gmail.com
