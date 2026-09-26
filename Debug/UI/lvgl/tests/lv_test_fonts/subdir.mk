################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/tests/lv_test_fonts/font_1.c \
../UI/lvgl/tests/lv_test_fonts/font_2.c \
../UI/lvgl/tests/lv_test_fonts/font_3.c 

OBJS += \
./UI/lvgl/tests/lv_test_fonts/font_1.o \
./UI/lvgl/tests/lv_test_fonts/font_2.o \
./UI/lvgl/tests/lv_test_fonts/font_3.o 

C_DEPS += \
./UI/lvgl/tests/lv_test_fonts/font_1.d \
./UI/lvgl/tests/lv_test_fonts/font_2.d \
./UI/lvgl/tests/lv_test_fonts/font_3.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/tests/lv_test_fonts/%.o UI/lvgl/tests/lv_test_fonts/%.su UI/lvgl/tests/lv_test_fonts/%.cyclo: ../UI/lvgl/tests/lv_test_fonts/%.c UI/lvgl/tests/lv_test_fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-tests-2f-lv_test_fonts

clean-UI-2f-lvgl-2f-tests-2f-lv_test_fonts:
	-$(RM) ./UI/lvgl/tests/lv_test_fonts/font_1.cyclo ./UI/lvgl/tests/lv_test_fonts/font_1.d ./UI/lvgl/tests/lv_test_fonts/font_1.o ./UI/lvgl/tests/lv_test_fonts/font_1.su ./UI/lvgl/tests/lv_test_fonts/font_2.cyclo ./UI/lvgl/tests/lv_test_fonts/font_2.d ./UI/lvgl/tests/lv_test_fonts/font_2.o ./UI/lvgl/tests/lv_test_fonts/font_2.su ./UI/lvgl/tests/lv_test_fonts/font_3.cyclo ./UI/lvgl/tests/lv_test_fonts/font_3.d ./UI/lvgl/tests/lv_test_fonts/font_3.o ./UI/lvgl/tests/lv_test_fonts/font_3.su

.PHONY: clean-UI-2f-lvgl-2f-tests-2f-lv_test_fonts

