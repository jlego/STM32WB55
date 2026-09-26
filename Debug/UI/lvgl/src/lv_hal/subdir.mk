################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/src/lv_hal/lv_hal_disp.c \
../UI/lvgl/src/lv_hal/lv_hal_indev.c \
../UI/lvgl/src/lv_hal/lv_hal_tick.c 

OBJS += \
./UI/lvgl/src/lv_hal/lv_hal_disp.o \
./UI/lvgl/src/lv_hal/lv_hal_indev.o \
./UI/lvgl/src/lv_hal/lv_hal_tick.o 

C_DEPS += \
./UI/lvgl/src/lv_hal/lv_hal_disp.d \
./UI/lvgl/src/lv_hal/lv_hal_indev.d \
./UI/lvgl/src/lv_hal/lv_hal_tick.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/src/lv_hal/%.o UI/lvgl/src/lv_hal/%.su UI/lvgl/src/lv_hal/%.cyclo: ../UI/lvgl/src/lv_hal/%.c UI/lvgl/src/lv_hal/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-src-2f-lv_hal

clean-UI-2f-lvgl-2f-src-2f-lv_hal:
	-$(RM) ./UI/lvgl/src/lv_hal/lv_hal_disp.cyclo ./UI/lvgl/src/lv_hal/lv_hal_disp.d ./UI/lvgl/src/lv_hal/lv_hal_disp.o ./UI/lvgl/src/lv_hal/lv_hal_disp.su ./UI/lvgl/src/lv_hal/lv_hal_indev.cyclo ./UI/lvgl/src/lv_hal/lv_hal_indev.d ./UI/lvgl/src/lv_hal/lv_hal_indev.o ./UI/lvgl/src/lv_hal/lv_hal_indev.su ./UI/lvgl/src/lv_hal/lv_hal_tick.cyclo ./UI/lvgl/src/lv_hal/lv_hal_tick.d ./UI/lvgl/src/lv_hal/lv_hal_tick.o ./UI/lvgl/src/lv_hal/lv_hal_tick.su

.PHONY: clean-UI-2f-lvgl-2f-src-2f-lv_hal

