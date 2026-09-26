################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../UI/lvgl/src/lv_misc/lv_anim.c \
../UI/lvgl/src/lv_misc/lv_area.c \
../UI/lvgl/src/lv_misc/lv_async.c \
../UI/lvgl/src/lv_misc/lv_bidi.c \
../UI/lvgl/src/lv_misc/lv_color.c \
../UI/lvgl/src/lv_misc/lv_debug.c \
../UI/lvgl/src/lv_misc/lv_fs.c \
../UI/lvgl/src/lv_misc/lv_gc.c \
../UI/lvgl/src/lv_misc/lv_ll.c \
../UI/lvgl/src/lv_misc/lv_log.c \
../UI/lvgl/src/lv_misc/lv_math.c \
../UI/lvgl/src/lv_misc/lv_mem.c \
../UI/lvgl/src/lv_misc/lv_printf.c \
../UI/lvgl/src/lv_misc/lv_task.c \
../UI/lvgl/src/lv_misc/lv_templ.c \
../UI/lvgl/src/lv_misc/lv_txt.c \
../UI/lvgl/src/lv_misc/lv_txt_ap.c \
../UI/lvgl/src/lv_misc/lv_utils.c 

OBJS += \
./UI/lvgl/src/lv_misc/lv_anim.o \
./UI/lvgl/src/lv_misc/lv_area.o \
./UI/lvgl/src/lv_misc/lv_async.o \
./UI/lvgl/src/lv_misc/lv_bidi.o \
./UI/lvgl/src/lv_misc/lv_color.o \
./UI/lvgl/src/lv_misc/lv_debug.o \
./UI/lvgl/src/lv_misc/lv_fs.o \
./UI/lvgl/src/lv_misc/lv_gc.o \
./UI/lvgl/src/lv_misc/lv_ll.o \
./UI/lvgl/src/lv_misc/lv_log.o \
./UI/lvgl/src/lv_misc/lv_math.o \
./UI/lvgl/src/lv_misc/lv_mem.o \
./UI/lvgl/src/lv_misc/lv_printf.o \
./UI/lvgl/src/lv_misc/lv_task.o \
./UI/lvgl/src/lv_misc/lv_templ.o \
./UI/lvgl/src/lv_misc/lv_txt.o \
./UI/lvgl/src/lv_misc/lv_txt_ap.o \
./UI/lvgl/src/lv_misc/lv_utils.o 

C_DEPS += \
./UI/lvgl/src/lv_misc/lv_anim.d \
./UI/lvgl/src/lv_misc/lv_area.d \
./UI/lvgl/src/lv_misc/lv_async.d \
./UI/lvgl/src/lv_misc/lv_bidi.d \
./UI/lvgl/src/lv_misc/lv_color.d \
./UI/lvgl/src/lv_misc/lv_debug.d \
./UI/lvgl/src/lv_misc/lv_fs.d \
./UI/lvgl/src/lv_misc/lv_gc.d \
./UI/lvgl/src/lv_misc/lv_ll.d \
./UI/lvgl/src/lv_misc/lv_log.d \
./UI/lvgl/src/lv_misc/lv_math.d \
./UI/lvgl/src/lv_misc/lv_mem.d \
./UI/lvgl/src/lv_misc/lv_printf.d \
./UI/lvgl/src/lv_misc/lv_task.d \
./UI/lvgl/src/lv_misc/lv_templ.d \
./UI/lvgl/src/lv_misc/lv_txt.d \
./UI/lvgl/src/lv_misc/lv_txt_ap.d \
./UI/lvgl/src/lv_misc/lv_utils.d 


# Each subdirectory must supply rules for building sources it contributes
UI/lvgl/src/lv_misc/%.o UI/lvgl/src/lv_misc/%.su UI/lvgl/src/lv_misc/%.cyclo: ../UI/lvgl/src/lv_misc/%.c UI/lvgl/src/lv_misc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32WB55xx -c -I../Core/Inc -I../UI -I../UI/lvgl -I../UI/lvgl/src -I../Drivers/STM32WBxx_HAL_Driver/Inc -I../Drivers/STM32WBxx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32WBxx/Include -I../Drivers/CMSIS/Include -I../USB_Device/App -I../USB_Device/Target -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-UI-2f-lvgl-2f-src-2f-lv_misc

clean-UI-2f-lvgl-2f-src-2f-lv_misc:
	-$(RM) ./UI/lvgl/src/lv_misc/lv_anim.cyclo ./UI/lvgl/src/lv_misc/lv_anim.d ./UI/lvgl/src/lv_misc/lv_anim.o ./UI/lvgl/src/lv_misc/lv_anim.su ./UI/lvgl/src/lv_misc/lv_area.cyclo ./UI/lvgl/src/lv_misc/lv_area.d ./UI/lvgl/src/lv_misc/lv_area.o ./UI/lvgl/src/lv_misc/lv_area.su ./UI/lvgl/src/lv_misc/lv_async.cyclo ./UI/lvgl/src/lv_misc/lv_async.d ./UI/lvgl/src/lv_misc/lv_async.o ./UI/lvgl/src/lv_misc/lv_async.su ./UI/lvgl/src/lv_misc/lv_bidi.cyclo ./UI/lvgl/src/lv_misc/lv_bidi.d ./UI/lvgl/src/lv_misc/lv_bidi.o ./UI/lvgl/src/lv_misc/lv_bidi.su ./UI/lvgl/src/lv_misc/lv_color.cyclo ./UI/lvgl/src/lv_misc/lv_color.d ./UI/lvgl/src/lv_misc/lv_color.o ./UI/lvgl/src/lv_misc/lv_color.su ./UI/lvgl/src/lv_misc/lv_debug.cyclo ./UI/lvgl/src/lv_misc/lv_debug.d ./UI/lvgl/src/lv_misc/lv_debug.o ./UI/lvgl/src/lv_misc/lv_debug.su ./UI/lvgl/src/lv_misc/lv_fs.cyclo ./UI/lvgl/src/lv_misc/lv_fs.d ./UI/lvgl/src/lv_misc/lv_fs.o ./UI/lvgl/src/lv_misc/lv_fs.su ./UI/lvgl/src/lv_misc/lv_gc.cyclo ./UI/lvgl/src/lv_misc/lv_gc.d ./UI/lvgl/src/lv_misc/lv_gc.o ./UI/lvgl/src/lv_misc/lv_gc.su ./UI/lvgl/src/lv_misc/lv_ll.cyclo ./UI/lvgl/src/lv_misc/lv_ll.d ./UI/lvgl/src/lv_misc/lv_ll.o ./UI/lvgl/src/lv_misc/lv_ll.su ./UI/lvgl/src/lv_misc/lv_log.cyclo ./UI/lvgl/src/lv_misc/lv_log.d ./UI/lvgl/src/lv_misc/lv_log.o ./UI/lvgl/src/lv_misc/lv_log.su ./UI/lvgl/src/lv_misc/lv_math.cyclo ./UI/lvgl/src/lv_misc/lv_math.d ./UI/lvgl/src/lv_misc/lv_math.o ./UI/lvgl/src/lv_misc/lv_math.su ./UI/lvgl/src/lv_misc/lv_mem.cyclo ./UI/lvgl/src/lv_misc/lv_mem.d ./UI/lvgl/src/lv_misc/lv_mem.o ./UI/lvgl/src/lv_misc/lv_mem.su ./UI/lvgl/src/lv_misc/lv_printf.cyclo ./UI/lvgl/src/lv_misc/lv_printf.d ./UI/lvgl/src/lv_misc/lv_printf.o ./UI/lvgl/src/lv_misc/lv_printf.su ./UI/lvgl/src/lv_misc/lv_task.cyclo ./UI/lvgl/src/lv_misc/lv_task.d ./UI/lvgl/src/lv_misc/lv_task.o ./UI/lvgl/src/lv_misc/lv_task.su ./UI/lvgl/src/lv_misc/lv_templ.cyclo ./UI/lvgl/src/lv_misc/lv_templ.d ./UI/lvgl/src/lv_misc/lv_templ.o ./UI/lvgl/src/lv_misc/lv_templ.su ./UI/lvgl/src/lv_misc/lv_txt.cyclo ./UI/lvgl/src/lv_misc/lv_txt.d ./UI/lvgl/src/lv_misc/lv_txt.o ./UI/lvgl/src/lv_misc/lv_txt.su ./UI/lvgl/src/lv_misc/lv_txt_ap.cyclo ./UI/lvgl/src/lv_misc/lv_txt_ap.d ./UI/lvgl/src/lv_misc/lv_txt_ap.o ./UI/lvgl/src/lv_misc/lv_txt_ap.su ./UI/lvgl/src/lv_misc/lv_utils.cyclo ./UI/lvgl/src/lv_misc/lv_utils.d ./UI/lvgl/src/lv_misc/lv_utils.o ./UI/lvgl/src/lv_misc/lv_utils.su

.PHONY: clean-UI-2f-lvgl-2f-src-2f-lv_misc

