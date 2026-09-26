################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/src/lv_themes/lv_theme.c \
../UI/lvgl/src/lv_themes/lv_theme_empty.c \
../UI/lvgl/src/lv_themes/lv_theme_material.c \
../UI/lvgl/src/lv_themes/lv_theme_mono.c \
../UI/lvgl/src/lv_themes/lv_theme_template.c 

OBJS += \
./UI/lvgl/src/lv_themes/lv_theme.o \
./UI/lvgl/src/lv_themes/lv_theme_empty.o \
./UI/lvgl/src/lv_themes/lv_theme_material.o \
./UI/lvgl/src/lv_themes/lv_theme_mono.o \
./UI/lvgl/src/lv_themes/lv_theme_template.o 

C_DEPS += \
./UI/lvgl/src/lv_themes/lv_theme.d \
./UI/lvgl/src/lv_themes/lv_theme_empty.d \
./UI/lvgl/src/lv_themes/lv_theme_material.d \
./UI/lvgl/src/lv_themes/lv_theme_mono.d \
./UI/lvgl/src/lv_themes/lv_theme_template.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/src/lv_themes/%.o UI/lvgl/src/lv_themes/%.su UI/lvgl/src/lv_themes/%.cyclo: ../UI/lvgl/src/lv_themes/%.c UI/lvgl/src/lv_themes/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-src-2f-lv_themes

clean-UI-2f-lvgl-2f-src-2f-lv_themes:
	-$(RM) ./UI/lvgl/src/lv_themes/lv_theme.cyclo ./UI/lvgl/src/lv_themes/lv_theme.d ./UI/lvgl/src/lv_themes/lv_theme.o ./UI/lvgl/src/lv_themes/lv_theme.su ./UI/lvgl/src/lv_themes/lv_theme_empty.cyclo ./UI/lvgl/src/lv_themes/lv_theme_empty.d ./UI/lvgl/src/lv_themes/lv_theme_empty.o ./UI/lvgl/src/lv_themes/lv_theme_empty.su ./UI/lvgl/src/lv_themes/lv_theme_material.cyclo ./UI/lvgl/src/lv_themes/lv_theme_material.d ./UI/lvgl/src/lv_themes/lv_theme_material.o ./UI/lvgl/src/lv_themes/lv_theme_material.su ./UI/lvgl/src/lv_themes/lv_theme_mono.cyclo ./UI/lvgl/src/lv_themes/lv_theme_mono.d ./UI/lvgl/src/lv_themes/lv_theme_mono.o ./UI/lvgl/src/lv_themes/lv_theme_mono.su ./UI/lvgl/src/lv_themes/lv_theme_template.cyclo ./UI/lvgl/src/lv_themes/lv_theme_template.d ./UI/lvgl/src/lv_themes/lv_theme_template.o ./UI/lvgl/src/lv_themes/lv_theme_template.su

.PHONY: clean-UI-2f-lvgl-2f-src-2f-lv_themes

