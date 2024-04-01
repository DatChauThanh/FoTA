################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Can_Interface.c \
../Core/Src/Encrypt.c \
../Core/Src/FlashReprogram_Interface.c \
../Core/Src/FontsSSD1306.c \
../Core/Src/ReceiveUpdate.c \
../Core/Src/Rte.c \
../Core/Src/SSD1306.c \
../Core/Src/Transmit.c \
../Core/Src/UserInterface.c \
../Core/Src/aes.c \
../Core/Src/main.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/Can_Interface.o \
./Core/Src/Encrypt.o \
./Core/Src/FlashReprogram_Interface.o \
./Core/Src/FontsSSD1306.o \
./Core/Src/ReceiveUpdate.o \
./Core/Src/Rte.o \
./Core/Src/SSD1306.o \
./Core/Src/Transmit.o \
./Core/Src/UserInterface.o \
./Core/Src/aes.o \
./Core/Src/main.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/Can_Interface.d \
./Core/Src/Encrypt.d \
./Core/Src/FlashReprogram_Interface.d \
./Core/Src/FontsSSD1306.d \
./Core/Src/ReceiveUpdate.d \
./Core/Src/Rte.d \
./Core/Src/SSD1306.d \
./Core/Src/Transmit.d \
./Core/Src/UserInterface.d \
./Core/Src/aes.d \
./Core/Src/main.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/ACER/STM32CubeIDE/workspace_1.13.1" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/Can_Interface.cyclo ./Core/Src/Can_Interface.d ./Core/Src/Can_Interface.o ./Core/Src/Can_Interface.su ./Core/Src/Encrypt.cyclo ./Core/Src/Encrypt.d ./Core/Src/Encrypt.o ./Core/Src/Encrypt.su ./Core/Src/FlashReprogram_Interface.cyclo ./Core/Src/FlashReprogram_Interface.d ./Core/Src/FlashReprogram_Interface.o ./Core/Src/FlashReprogram_Interface.su ./Core/Src/FontsSSD1306.cyclo ./Core/Src/FontsSSD1306.d ./Core/Src/FontsSSD1306.o ./Core/Src/FontsSSD1306.su ./Core/Src/ReceiveUpdate.cyclo ./Core/Src/ReceiveUpdate.d ./Core/Src/ReceiveUpdate.o ./Core/Src/ReceiveUpdate.su ./Core/Src/Rte.cyclo ./Core/Src/Rte.d ./Core/Src/Rte.o ./Core/Src/Rte.su ./Core/Src/SSD1306.cyclo ./Core/Src/SSD1306.d ./Core/Src/SSD1306.o ./Core/Src/SSD1306.su ./Core/Src/Transmit.cyclo ./Core/Src/Transmit.d ./Core/Src/Transmit.o ./Core/Src/Transmit.su ./Core/Src/UserInterface.cyclo ./Core/Src/UserInterface.d ./Core/Src/UserInterface.o ./Core/Src/UserInterface.su ./Core/Src/aes.cyclo ./Core/Src/aes.d ./Core/Src/aes.o ./Core/Src/aes.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

