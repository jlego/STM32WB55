################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/displayapp/icons/music/disc.c \
../UI/displayapp/icons/music/disc_f_1.c \
../UI/displayapp/icons/music/disc_f_2.c 

OBJS += \
./UI/displayapp/icons/music/disc.o \
./UI/displayapp/icons/music/disc_f_1.o \
./UI/displayapp/icons/music/disc_f_2.o 

C_DEPS += \
./UI/displayapp/icons/music/disc.d \
./UI/displayapp/icons/music/disc_f_1.d \
./UI/displayapp/icons/music/disc_f_2.d 


# Each subdirectory must supply rules for building sources it contributes
UI/displayapp/icons/music/%.o UI/displayapp/icons/music/%.su UI/displayapp/icons/music/%.cyclo: ../UI/displayapp/icons/music/%.c UI/displayapp/icons/music/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-displayapp-2f-icons-2f-music

clean-UI-2f-displayapp-2f-icons-2f-music:
	-$(RM) ./UI/displayapp/icons/music/disc.cyclo ./UI/displayapp/icons/music/disc.d ./UI/displayapp/icons/music/disc.o ./UI/displayapp/icons/music/disc.su ./UI/displayapp/icons/music/disc_f_1.cyclo ./UI/displayapp/icons/music/disc_f_1.d ./UI/displayapp/icons/music/disc_f_1.o ./UI/displayapp/icons/music/disc_f_1.su ./UI/displayapp/icons/music/disc_f_2.cyclo ./UI/displayapp/icons/music/disc_f_2.d ./UI/displayapp/icons/music/disc_f_2.o ./UI/displayapp/icons/music/disc_f_2.su

.PHONY: clean-UI-2f-displayapp-2f-icons-2f-music

