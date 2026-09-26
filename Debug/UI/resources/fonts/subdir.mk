################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/resources/fonts/jetbrains_mono_42.c \
../UI/resources/fonts/jetbrains_mono_76.c \
../UI/resources/fonts/jetbrains_mono_bold_20.c \
../UI/resources/fonts/jetbrains_mono_extrabold_compressed.c 

OBJS += \
./UI/resources/fonts/jetbrains_mono_42.o \
./UI/resources/fonts/jetbrains_mono_76.o \
./UI/resources/fonts/jetbrains_mono_bold_20.o \
./UI/resources/fonts/jetbrains_mono_extrabold_compressed.o 

C_DEPS += \
./UI/resources/fonts/jetbrains_mono_42.d \
./UI/resources/fonts/jetbrains_mono_76.d \
./UI/resources/fonts/jetbrains_mono_bold_20.d \
./UI/resources/fonts/jetbrains_mono_extrabold_compressed.d 


# Each subdirectory must supply rules for building sources it contributes
UI/resources/fonts/%.o UI/resources/fonts/%.su UI/resources/fonts/%.cyclo: ../UI/resources/fonts/%.c UI/resources/fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-resources-2f-fonts

clean-UI-2f-resources-2f-fonts:
	-$(RM) ./UI/resources/fonts/jetbrains_mono_42.cyclo ./UI/resources/fonts/jetbrains_mono_42.d ./UI/resources/fonts/jetbrains_mono_42.o ./UI/resources/fonts/jetbrains_mono_42.su ./UI/resources/fonts/jetbrains_mono_76.cyclo ./UI/resources/fonts/jetbrains_mono_76.d ./UI/resources/fonts/jetbrains_mono_76.o ./UI/resources/fonts/jetbrains_mono_76.su ./UI/resources/fonts/jetbrains_mono_bold_20.cyclo ./UI/resources/fonts/jetbrains_mono_bold_20.d ./UI/resources/fonts/jetbrains_mono_bold_20.o ./UI/resources/fonts/jetbrains_mono_bold_20.su ./UI/resources/fonts/jetbrains_mono_extrabold_compressed.cyclo ./UI/resources/fonts/jetbrains_mono_extrabold_compressed.d ./UI/resources/fonts/jetbrains_mono_extrabold_compressed.o ./UI/resources/fonts/jetbrains_mono_extrabold_compressed.su

.PHONY: clean-UI-2f-resources-2f-fonts

