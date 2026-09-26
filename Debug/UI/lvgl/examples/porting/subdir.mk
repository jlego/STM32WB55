################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/examples/porting/lv_port_disp_template.c \
../UI/lvgl/examples/porting/lv_port_fs_template.c \
../UI/lvgl/examples/porting/lv_port_indev_template.c 

OBJS += \
./UI/lvgl/examples/porting/lv_port_disp_template.o \
./UI/lvgl/examples/porting/lv_port_fs_template.o \
./UI/lvgl/examples/porting/lv_port_indev_template.o 

C_DEPS += \
./UI/lvgl/examples/porting/lv_port_disp_template.d \
./UI/lvgl/examples/porting/lv_port_fs_template.d \
./UI/lvgl/examples/porting/lv_port_indev_template.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/examples/porting/%.o UI/lvgl/examples/porting/%.su UI/lvgl/examples/porting/%.cyclo: ../UI/lvgl/examples/porting/%.c UI/lvgl/examples/porting/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-examples-2f-porting

clean-UI-2f-lvgl-2f-examples-2f-porting:
	-$(RM) ./UI/lvgl/examples/porting/lv_port_disp_template.cyclo ./UI/lvgl/examples/porting/lv_port_disp_template.d ./UI/lvgl/examples/porting/lv_port_disp_template.o ./UI/lvgl/examples/porting/lv_port_disp_template.su ./UI/lvgl/examples/porting/lv_port_fs_template.cyclo ./UI/lvgl/examples/porting/lv_port_fs_template.d ./UI/lvgl/examples/porting/lv_port_fs_template.o ./UI/lvgl/examples/porting/lv_port_fs_template.su ./UI/lvgl/examples/porting/lv_port_indev_template.cyclo ./UI/lvgl/examples/porting/lv_port_indev_template.d ./UI/lvgl/examples/porting/lv_port_indev_template.o ./UI/lvgl/examples/porting/lv_port_indev_template.su

.PHONY: clean-UI-2f-lvgl-2f-examples-2f-porting

