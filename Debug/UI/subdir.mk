################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/ft3168_touch.c \
../UI/infinitime_adapter.c \
../UI/lvgl_stm32.c \
../UI/soft_i2c.c \
../UI/touch_driver.c \
../UI/ui_demo.c 

OBJS += \
./UI/ft3168_touch.o \
./UI/infinitime_adapter.o \
./UI/lvgl_stm32.o \
./UI/soft_i2c.o \
./UI/touch_driver.o \
./UI/ui_demo.o 

C_DEPS += \
./UI/ft3168_touch.d \
./UI/infinitime_adapter.d \
./UI/lvgl_stm32.d \
./UI/soft_i2c.d \
./UI/touch_driver.d \
./UI/ui_demo.d 


# Each subdirectory must supply rules for building sources it contributes
UI/%.o UI/%.su UI/%.cyclo: ../UI/%.c UI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI

clean-UI:
	-$(RM) ./UI/ft3168_touch.cyclo ./UI/ft3168_touch.d ./UI/ft3168_touch.o ./UI/ft3168_touch.su ./UI/infinitime_adapter.cyclo ./UI/infinitime_adapter.d ./UI/infinitime_adapter.o ./UI/infinitime_adapter.su ./UI/lvgl_stm32.cyclo ./UI/lvgl_stm32.d ./UI/lvgl_stm32.o ./UI/lvgl_stm32.su ./UI/soft_i2c.cyclo ./UI/soft_i2c.d ./UI/soft_i2c.o ./UI/soft_i2c.su ./UI/touch_driver.cyclo ./UI/touch_driver.d ./UI/touch_driver.o ./UI/touch_driver.su ./UI/ui_demo.cyclo ./UI/ui_demo.d ./UI/ui_demo.o ./UI/ui_demo.su

.PHONY: clean-UI

