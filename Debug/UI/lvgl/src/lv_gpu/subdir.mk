################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.c \
../UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.c \
../UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.c \
../UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.c 

OBJS += \
./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.o \
./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.o \
./UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.o \
./UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.o 

C_DEPS += \
./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.d \
./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.d \
./UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.d \
./UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/src/lv_gpu/%.o UI/lvgl/src/lv_gpu/%.su UI/lvgl/src/lv_gpu/%.cyclo: ../UI/lvgl/src/lv_gpu/%.c UI/lvgl/src/lv_gpu/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-src-2f-lv_gpu

clean-UI-2f-lvgl-2f-src-2f-lv_gpu:
	-$(RM) ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.cyclo ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.d ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.o ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp.su ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.cyclo ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.d ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.o ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_pxp_osa.su ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.cyclo ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.d ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.o ./UI/lvgl/src/lv_gpu/lv_gpu_nxp_vglite.su ./UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.cyclo ./UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.d ./UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.o ./UI/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.su

.PHONY: clean-UI-2f-lvgl-2f-src-2f-lv_gpu

