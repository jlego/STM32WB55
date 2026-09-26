################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include USB_Device/Target/subdir.mk
-include USB_Device/App/subdir.mk
-include UI/resources/fonts/subdir.mk
-include UI/lvgl/tests/lv_test_widgets/subdir.mk
-include UI/lvgl/tests/lv_test_fonts/subdir.mk
-include UI/lvgl/tests/lv_test_core/subdir.mk
-include UI/lvgl/tests/subdir.mk
-include UI/lvgl/src/lv_widgets/subdir.mk
-include UI/lvgl/src/lv_themes/subdir.mk
-include UI/lvgl/src/lv_misc/subdir.mk
-include UI/lvgl/src/lv_hal/subdir.mk
-include UI/lvgl/src/lv_gpu/subdir.mk
-include UI/lvgl/src/lv_font/subdir.mk
-include UI/lvgl/src/lv_draw/subdir.mk
-include UI/lvgl/src/lv_core/subdir.mk
-include UI/lvgl/examples/porting/subdir.mk
-include UI/drivers/Bma421_C/subdir.mk
-include UI/drivers/subdir.mk
-include UI/displayapp/icons/music/subdir.mk
-include UI/displayapp/icons/infinitime/subdir.mk
-include UI/displayapp/icons/battery/subdir.mk
-include UI/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/subdir.mk
-include Drivers/STM32WBxx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := STM32WB55
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32WB55.elf \

MAP_FILES += \
STM32WB55.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32WB55.list \


# All Target
all: main-build

# Main-build Target
main-build: STM32WB55.elf secondary-outputs

# Tool invocations
STM32WB55.elf STM32WB55.map: $(OBJS) $(USER_OBJS) /Volumes/disk1t/stm32/STM32WB55/STM32WB55CGUX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STM32WB55.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Volumes/disk1t/stm32/STM32WB55/STM32WB55CGUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STM32WB55.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32WB55.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STM32WB55.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32WB55.elf STM32WB55.list STM32WB55.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
