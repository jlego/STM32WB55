################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/src/lv_draw/lv_draw_arc.c \
../UI/lvgl/src/lv_draw/lv_draw_blend.c \
../UI/lvgl/src/lv_draw/lv_draw_img.c \
../UI/lvgl/src/lv_draw/lv_draw_label.c \
../UI/lvgl/src/lv_draw/lv_draw_line.c \
../UI/lvgl/src/lv_draw/lv_draw_mask.c \
../UI/lvgl/src/lv_draw/lv_draw_rect.c \
../UI/lvgl/src/lv_draw/lv_draw_triangle.c \
../UI/lvgl/src/lv_draw/lv_img_buf.c \
../UI/lvgl/src/lv_draw/lv_img_cache.c \
../UI/lvgl/src/lv_draw/lv_img_decoder.c 

OBJS += \
./UI/lvgl/src/lv_draw/lv_draw_arc.o \
./UI/lvgl/src/lv_draw/lv_draw_blend.o \
./UI/lvgl/src/lv_draw/lv_draw_img.o \
./UI/lvgl/src/lv_draw/lv_draw_label.o \
./UI/lvgl/src/lv_draw/lv_draw_line.o \
./UI/lvgl/src/lv_draw/lv_draw_mask.o \
./UI/lvgl/src/lv_draw/lv_draw_rect.o \
./UI/lvgl/src/lv_draw/lv_draw_triangle.o \
./UI/lvgl/src/lv_draw/lv_img_buf.o \
./UI/lvgl/src/lv_draw/lv_img_cache.o \
./UI/lvgl/src/lv_draw/lv_img_decoder.o 

C_DEPS += \
./UI/lvgl/src/lv_draw/lv_draw_arc.d \
./UI/lvgl/src/lv_draw/lv_draw_blend.d \
./UI/lvgl/src/lv_draw/lv_draw_img.d \
./UI/lvgl/src/lv_draw/lv_draw_label.d \
./UI/lvgl/src/lv_draw/lv_draw_line.d \
./UI/lvgl/src/lv_draw/lv_draw_mask.d \
./UI/lvgl/src/lv_draw/lv_draw_rect.d \
./UI/lvgl/src/lv_draw/lv_draw_triangle.d \
./UI/lvgl/src/lv_draw/lv_img_buf.d \
./UI/lvgl/src/lv_draw/lv_img_cache.d \
./UI/lvgl/src/lv_draw/lv_img_decoder.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/src/lv_draw/%.o UI/lvgl/src/lv_draw/%.su UI/lvgl/src/lv_draw/%.cyclo: ../UI/lvgl/src/lv_draw/%.c UI/lvgl/src/lv_draw/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-src-2f-lv_draw

clean-UI-2f-lvgl-2f-src-2f-lv_draw:
	-$(RM) ./UI/lvgl/src/lv_draw/lv_draw_arc.cyclo ./UI/lvgl/src/lv_draw/lv_draw_arc.d ./UI/lvgl/src/lv_draw/lv_draw_arc.o ./UI/lvgl/src/lv_draw/lv_draw_arc.su ./UI/lvgl/src/lv_draw/lv_draw_blend.cyclo ./UI/lvgl/src/lv_draw/lv_draw_blend.d ./UI/lvgl/src/lv_draw/lv_draw_blend.o ./UI/lvgl/src/lv_draw/lv_draw_blend.su ./UI/lvgl/src/lv_draw/lv_draw_img.cyclo ./UI/lvgl/src/lv_draw/lv_draw_img.d ./UI/lvgl/src/lv_draw/lv_draw_img.o ./UI/lvgl/src/lv_draw/lv_draw_img.su ./UI/lvgl/src/lv_draw/lv_draw_label.cyclo ./UI/lvgl/src/lv_draw/lv_draw_label.d ./UI/lvgl/src/lv_draw/lv_draw_label.o ./UI/lvgl/src/lv_draw/lv_draw_label.su ./UI/lvgl/src/lv_draw/lv_draw_line.cyclo ./UI/lvgl/src/lv_draw/lv_draw_line.d ./UI/lvgl/src/lv_draw/lv_draw_line.o ./UI/lvgl/src/lv_draw/lv_draw_line.su ./UI/lvgl/src/lv_draw/lv_draw_mask.cyclo ./UI/lvgl/src/lv_draw/lv_draw_mask.d ./UI/lvgl/src/lv_draw/lv_draw_mask.o ./UI/lvgl/src/lv_draw/lv_draw_mask.su ./UI/lvgl/src/lv_draw/lv_draw_rect.cyclo ./UI/lvgl/src/lv_draw/lv_draw_rect.d ./UI/lvgl/src/lv_draw/lv_draw_rect.o ./UI/lvgl/src/lv_draw/lv_draw_rect.su ./UI/lvgl/src/lv_draw/lv_draw_triangle.cyclo ./UI/lvgl/src/lv_draw/lv_draw_triangle.d ./UI/lvgl/src/lv_draw/lv_draw_triangle.o ./UI/lvgl/src/lv_draw/lv_draw_triangle.su ./UI/lvgl/src/lv_draw/lv_img_buf.cyclo ./UI/lvgl/src/lv_draw/lv_img_buf.d ./UI/lvgl/src/lv_draw/lv_img_buf.o ./UI/lvgl/src/lv_draw/lv_img_buf.su ./UI/lvgl/src/lv_draw/lv_img_cache.cyclo ./UI/lvgl/src/lv_draw/lv_img_cache.d ./UI/lvgl/src/lv_draw/lv_img_cache.o ./UI/lvgl/src/lv_draw/lv_img_cache.su ./UI/lvgl/src/lv_draw/lv_img_decoder.cyclo ./UI/lvgl/src/lv_draw/lv_img_decoder.d ./UI/lvgl/src/lv_draw/lv_img_decoder.o ./UI/lvgl/src/lv_draw/lv_img_decoder.su

.PHONY: clean-UI-2f-lvgl-2f-src-2f-lv_draw

