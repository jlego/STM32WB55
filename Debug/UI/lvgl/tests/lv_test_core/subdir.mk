################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/tests/lv_test_core/lv_test_core.c \
../UI/lvgl/tests/lv_test_core/lv_test_font_loader.c \
../UI/lvgl/tests/lv_test_core/lv_test_obj.c \
../UI/lvgl/tests/lv_test_core/lv_test_style.c 

OBJS += \
./UI/lvgl/tests/lv_test_core/lv_test_core.o \
./UI/lvgl/tests/lv_test_core/lv_test_font_loader.o \
./UI/lvgl/tests/lv_test_core/lv_test_obj.o \
./UI/lvgl/tests/lv_test_core/lv_test_style.o 

C_DEPS += \
./UI/lvgl/tests/lv_test_core/lv_test_core.d \
./UI/lvgl/tests/lv_test_core/lv_test_font_loader.d \
./UI/lvgl/tests/lv_test_core/lv_test_obj.d \
./UI/lvgl/tests/lv_test_core/lv_test_style.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/tests/lv_test_core/%.o UI/lvgl/tests/lv_test_core/%.su UI/lvgl/tests/lv_test_core/%.cyclo: ../UI/lvgl/tests/lv_test_core/%.c UI/lvgl/tests/lv_test_core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-tests-2f-lv_test_core

clean-UI-2f-lvgl-2f-tests-2f-lv_test_core:
	-$(RM) ./UI/lvgl/tests/lv_test_core/lv_test_core.cyclo ./UI/lvgl/tests/lv_test_core/lv_test_core.d ./UI/lvgl/tests/lv_test_core/lv_test_core.o ./UI/lvgl/tests/lv_test_core/lv_test_core.su ./UI/lvgl/tests/lv_test_core/lv_test_font_loader.cyclo ./UI/lvgl/tests/lv_test_core/lv_test_font_loader.d ./UI/lvgl/tests/lv_test_core/lv_test_font_loader.o ./UI/lvgl/tests/lv_test_core/lv_test_font_loader.su ./UI/lvgl/tests/lv_test_core/lv_test_obj.cyclo ./UI/lvgl/tests/lv_test_core/lv_test_obj.d ./UI/lvgl/tests/lv_test_core/lv_test_obj.o ./UI/lvgl/tests/lv_test_core/lv_test_obj.su ./UI/lvgl/tests/lv_test_core/lv_test_style.cyclo ./UI/lvgl/tests/lv_test_core/lv_test_style.d ./UI/lvgl/tests/lv_test_core/lv_test_style.o ./UI/lvgl/tests/lv_test_core/lv_test_style.su

.PHONY: clean-UI-2f-lvgl-2f-tests-2f-lv_test_core

