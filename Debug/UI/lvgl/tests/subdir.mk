################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/tests/lv_test_assert.c \
../UI/lvgl/tests/lv_test_main.c 

OBJS += \
./UI/lvgl/tests/lv_test_assert.o \
./UI/lvgl/tests/lv_test_main.o 

C_DEPS += \
./UI/lvgl/tests/lv_test_assert.d \
./UI/lvgl/tests/lv_test_main.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/tests/%.o UI/lvgl/tests/%.su UI/lvgl/tests/%.cyclo: ../UI/lvgl/tests/%.c UI/lvgl/tests/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-tests

clean-UI-2f-lvgl-2f-tests:
	-$(RM) ./UI/lvgl/tests/lv_test_assert.cyclo ./UI/lvgl/tests/lv_test_assert.d ./UI/lvgl/tests/lv_test_assert.o ./UI/lvgl/tests/lv_test_assert.su ./UI/lvgl/tests/lv_test_main.cyclo ./UI/lvgl/tests/lv_test_main.d ./UI/lvgl/tests/lv_test_main.o ./UI/lvgl/tests/lv_test_main.su

.PHONY: clean-UI-2f-lvgl-2f-tests

