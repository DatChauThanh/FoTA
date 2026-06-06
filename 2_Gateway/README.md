# Gateway Layout

The gateway follows the same simple structure as the bootloader.

```text
Core
  Minimal entry point. It calls platform init, app init, then repeatedly runs the app main function.

App
  Gateway state machine and application services: receive update, encrypt, transmit, UI, RTE.

Port/[platform]
  Platform-specific implementation and generated support code.

ThirdParty/tinyAES
  AES implementation used before forwarding firmware to the target bootloader.

ThirdParty/SSD1306
  OLED display driver and fonts.
```

## STM32CubeIDE Include Paths

```text
2_Gateway/App/Inc
2_Gateway/Port/STM32F1_HAL/Inc
2_Gateway/ThirdParty/tinyAES/Inc
2_Gateway/ThirdParty/SSD1306/Inc
```

## Source Folders

```text
2_Gateway/Core/Src
2_Gateway/App/Src
2_Gateway/Port/STM32F1_HAL/Src
2_Gateway/ThirdParty/tinyAES/Src
2_Gateway/ThirdParty/SSD1306/Src
```

`Core/Src/main.c` should stay platform-agnostic. STM32 HAL initialization belongs in `Port/STM32F1_HAL`.
