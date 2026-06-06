# Bootloader Porting Guide

The bootloader core should depend only on these public headers:

- `BL_Interface.h`
- `BL_Platform.h`
- `BL_Transport.h`
- `ASE_Interface.h`

Vendor-specific code belongs behind the platform and transport headers.

## To Port Away From STM32 HAL

1. Keep `1_BootLoader/App/Src/BL.c` unchanged.
2. Remove `1_BootLoader/Port/STM32F1_HAL/Src/BL_Platform.c` from the build.
3. Remove `1_BootLoader/Port/STM32F1_HAL/Src/BL_Transport_CAN.c` from the build.
4. Add your own implementation files, for example:

```text
BL_Platform_STM32F1_LL.c
BL_Transport_CAN_LL.c
```

5. Implement every function declared in `BL_Platform.h` and `BL_Transport.h`.
6. Keep the same flash memory map unless you also update `BL_Interface.h`, linker scripts, and gateway metadata generation.

## Rule

No `HAL_*`, `LL_*`, register access, or vendor handle should be used from `BL.c`. That file is the bootloader policy layer.
