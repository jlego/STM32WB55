################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/src/lv_core/lv_disp.c \
../UI/lvgl/src/lv_core/lv_group.c \
../UI/lvgl/src/lv_core/lv_indev.c \
../UI/lvgl/src/lv_core/lv_obj.c \
../UI/lvgl/src/lv_core/lv_refr.c \
../UI/lvgl/src/lv_core/lv_style.c 

OBJS += \
./UI/lvgl/src/lv_core/lv_disp.o \
./UI/lvgl/src/lv_core/lv_group.o \
./UI/lvgl/src/lv_core/lv_indev.o \
./UI/lvgl/src/lv_core/lv_obj.o \
./UI/lvgl/src/lv_core/lv_refr.o \
./UI/lvgl/src/lv_core/lv_style.o 

C_DEPS += \
./UI/lvgl/src/lv_core/lv_disp.d \
./UI/lvgl/src/lv_core/lv_group.d \
./UI/lvgl/src/lv_core/lv_indev.d \
./UI/lvgl/src/lv_core/lv_obj.d \
./UI/lvgl/src/lv_core/lv_refr.d \
./UI/lvgl/src/lv_core/lv_style.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/src/lv_core/%.o UI/lvgl/src/lv_core/%.su UI/lvgl/src/lv_core/%.cyclo: ../UI/lvgl/src/lv_core/%.c UI/lvgl/src/lv_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-src-2f-lv_core

clean-UI-2f-lvgl-2f-src-2f-lv_core:
	-$(RM) ./UI/lvgl/src/lv_core/lv_disp.cyclo ./UI/lvgl/src/lv_core/lv_disp.d ./UI/lvgl/src/lv_core/lv_disp.o ./UI/lvgl/src/lv_core/lv_disp.su ./UI/lvgl/src/lv_core/lv_group.cyclo ./UI/lvgl/src/lv_core/lv_group.d ./UI/lvgl/src/lv_core/lv_group.o ./UI/lvgl/src/lv_core/lv_group.su ./UI/lvgl/src/lv_core/lv_indev.cyclo ./UI/lvgl/src/lv_core/lv_indev.d ./UI/lvgl/src/lv_core/lv_indev.o ./UI/lvgl/src/lv_core/lv_indev.su ./UI/lvgl/src/lv_core/lv_obj.cyclo ./UI/lvgl/src/lv_core/lv_obj.d ./UI/lvgl/src/lv_core/lv_obj.o ./UI/lvgl/src/lv_core/lv_obj.su ./UI/lvgl/src/lv_core/lv_refr.cyclo ./UI/lvgl/src/lv_core/lv_refr.d ./UI/lvgl/src/lv_core/lv_refr.o ./UI/lvgl/src/lv_core/lv_refr.su ./UI/lvgl/src/lv_core/lv_style.cyclo ./UI/lvgl/src/lv_core/lv_style.d ./UI/lvgl/src/lv_core/lv_style.o ./UI/lvgl/src/lv_core/lv_style.su

.PHONY: clean-UI-2f-lvgl-2f-src-2f-lv_core

