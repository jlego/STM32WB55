################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/drivers/Bma421_C/bma4.c \
../UI/drivers/Bma421_C/bma423.c 

OBJS += \
./UI/drivers/Bma421_C/bma4.o \
./UI/drivers/Bma421_C/bma423.o 

C_DEPS += \
./UI/drivers/Bma421_C/bma4.d \
./UI/drivers/Bma421_C/bma423.d 


# Each subdirectory must supply rules for building sources it contributes
UI/drivers/Bma421_C/%.o UI/drivers/Bma421_C/%.su UI/drivers/Bma421_C/%.cyclo: ../UI/drivers/Bma421_C/%.c UI/drivers/Bma421_C/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-drivers-2f-Bma421_C

clean-UI-2f-drivers-2f-Bma421_C:
	-$(RM) ./UI/drivers/Bma421_C/bma4.cyclo ./UI/drivers/Bma421_C/bma4.d ./UI/drivers/Bma421_C/bma4.o ./UI/drivers/Bma421_C/bma4.su ./UI/drivers/Bma421_C/bma423.cyclo ./UI/drivers/Bma421_C/bma423.d ./UI/drivers/Bma421_C/bma423.o ./UI/drivers/Bma421_C/bma423.su

.PHONY: clean-UI-2f-drivers-2f-Bma421_C

