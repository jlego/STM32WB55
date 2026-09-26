################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/drivers/usb/Lib/Core/usbd_core.c \
../UI/drivers/usb/Lib/Core/usbd_ctlreq.c \
../UI/drivers/usb/Lib/Core/usbd_ioreq.c 

OBJS += \
./UI/drivers/usb/Lib/Core/usbd_core.o \
./UI/drivers/usb/Lib/Core/usbd_ctlreq.o \
./UI/drivers/usb/Lib/Core/usbd_ioreq.o 

C_DEPS += \
./UI/drivers/usb/Lib/Core/usbd_core.d \
./UI/drivers/usb/Lib/Core/usbd_ctlreq.d \
./UI/drivers/usb/Lib/Core/usbd_ioreq.d 


# Each subdirectory must supply rules for building sources it contributes
UI/drivers/usb/Lib/Core/%.o UI/drivers/usb/Lib/Core/%.su UI/drivers/usb/Lib/Core/%.cyclo: ../UI/drivers/usb/Lib/Core/%.c UI/drivers/usb/Lib/Core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-drivers-2f-usb-2f-Lib-2f-Core

clean-UI-2f-drivers-2f-usb-2f-Lib-2f-Core:
	-$(RM) ./UI/drivers/usb/Lib/Core/usbd_core.cyclo ./UI/drivers/usb/Lib/Core/usbd_core.d ./UI/drivers/usb/Lib/Core/usbd_core.o ./UI/drivers/usb/Lib/Core/usbd_core.su ./UI/drivers/usb/Lib/Core/usbd_ctlreq.cyclo ./UI/drivers/usb/Lib/Core/usbd_ctlreq.d ./UI/drivers/usb/Lib/Core/usbd_ctlreq.o ./UI/drivers/usb/Lib/Core/usbd_ctlreq.su ./UI/drivers/usb/Lib/Core/usbd_ioreq.cyclo ./UI/drivers/usb/Lib/Core/usbd_ioreq.d ./UI/drivers/usb/Lib/Core/usbd_ioreq.o ./UI/drivers/usb/Lib/Core/usbd_ioreq.su

.PHONY: clean-UI-2f-drivers-2f-usb-2f-Lib-2f-Core

