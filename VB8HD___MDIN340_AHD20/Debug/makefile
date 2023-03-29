################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Video/subdir.mk
-include Hdmirx/subdir.mk
-include Graphic/subdir.mk
-include Drivers/subdir.mk
-include Common/subdir.mk
-include Arm/util/hid/subdir.mk
-include Arm/util/subdir.mk
-include Arm/usb/subdir.mk
-include Arm/lib/subdir.mk
-include Arm/CMSIS/startup/subdir.mk
-include Arm/CMSIS/subdir.mk
-include App/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: VB8HD___MDIN340_AHD20.axf

# Tool invocations
VB8HD___MDIN340_AHD20.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-none-eabi-gcc -nostartfiles -T STM32F10x_flash_hd.ld -mcpu=cortex-m3 -mthumb -o "VB8HD___MDIN340_AHD20.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) VB8HD___MDIN340_AHD20.axf
	-@echo ' '

post-build:
	-@echo 'axf to binary'
	-arm-none-eabi-objcopy -O binary VB8HD___MDIN340_AHD20.axf VB8HD___MDIN340_AHD20.bin
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
