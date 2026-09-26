################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/drivers/usb/App/usb_device.c \
../UI/drivers/usb/App/usbd_cdc_if.c \
../UI/drivers/usb/App/usbd_desc.c 

OBJS += \
./UI/drivers/usb/App/usb_device.o \
./UI/drivers/usb/App/usbd_cdc_if.o \
./UI/drivers/usb/App/usbd_desc.o 

C_DEPS += \
./UI/drivers/usb/App/usb_device.d \
./UI/drivers/usb/App/usbd_cdc_if.d \
./UI/drivers/usb/App/usbd_desc.d 


# Each subdirectory must supply rules for building sources it contributes
UI/drivers/usb/App/%.o UI/drivers/usb/App/%.su UI/drivers/usb/App/%.cyclo: ../UI/drivers/usb/App/%.c UI/drivers/usb/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-drivers-2f-usb-2f-App

clean-UI-2f-drivers-2f-usb-2f-App:
	-$(RM) ./UI/drivers/usb/App/usb_device.cyclo ./UI/drivers/usb/App/usb_device.d ./UI/drivers/usb/App/usb_device.o ./UI/drivers/usb/App/usb_device.su ./UI/drivers/usb/App/usbd_cdc_if.cyclo ./UI/drivers/usb/App/usbd_cdc_if.d ./UI/drivers/usb/App/usbd_cdc_if.o ./UI/drivers/usb/App/usbd_cdc_if.su ./UI/drivers/usb/App/usbd_desc.cyclo ./UI/drivers/usb/App/usbd_desc.d ./UI/drivers/usb/App/usbd_desc.o ./UI/drivers/usb/App/usbd_desc.su

.PHONY: clean-UI-2f-drivers-2f-usb-2f-App

