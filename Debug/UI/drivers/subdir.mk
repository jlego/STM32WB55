################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/drivers/UartLogger.c \
../UI/drivers/UsbCdcLogger.c 

OBJS += \
./UI/drivers/UartLogger.o \
./UI/drivers/UsbCdcLogger.o 

C_DEPS += \
./UI/drivers/UartLogger.d \
./UI/drivers/UsbCdcLogger.d 


# Each subdirectory must supply rules for building sources it contributes
UI/drivers/%.o UI/drivers/%.su UI/drivers/%.cyclo: ../UI/drivers/%.c UI/drivers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-drivers

clean-UI-2f-drivers:
	-$(RM) ./UI/drivers/UartLogger.cyclo ./UI/drivers/UartLogger.d ./UI/drivers/UartLogger.o ./UI/drivers/UartLogger.su ./UI/drivers/UsbCdcLogger.cyclo ./UI/drivers/UsbCdcLogger.d ./UI/drivers/UsbCdcLogger.o ./UI/drivers/UsbCdcLogger.su

.PHONY: clean-UI-2f-drivers

