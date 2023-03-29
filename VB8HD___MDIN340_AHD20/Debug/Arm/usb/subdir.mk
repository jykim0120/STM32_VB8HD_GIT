################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Arm/usb/usb_core.c \
../Arm/usb/usb_init.c \
../Arm/usb/usb_int.c \
../Arm/usb/usb_mem.c \
../Arm/usb/usb_regs.c \
../Arm/usb/usb_sil.c 

OBJS += \
./Arm/usb/usb_core.o \
./Arm/usb/usb_init.o \
./Arm/usb/usb_int.o \
./Arm/usb/usb_mem.o \
./Arm/usb/usb_regs.o \
./Arm/usb/usb_sil.o 

C_DEPS += \
./Arm/usb/usb_core.d \
./Arm/usb/usb_init.d \
./Arm/usb/usb_int.d \
./Arm/usb/usb_mem.d \
./Arm/usb/usb_regs.d \
./Arm/usb/usb_sil.d 


# Each subdirectory must supply rules for building sources it contributes
Arm/usb/%.o: ../Arm/usb/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


