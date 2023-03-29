################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Arm/util/hid/hw_config.c \
../Arm/util/hid/usb_desc.c \
../Arm/util/hid/usb_endp.c \
../Arm/util/hid/usb_istr.c \
../Arm/util/hid/usb_prop.c \
../Arm/util/hid/usb_pwr.c 

OBJS += \
./Arm/util/hid/hw_config.o \
./Arm/util/hid/usb_desc.o \
./Arm/util/hid/usb_endp.o \
./Arm/util/hid/usb_istr.o \
./Arm/util/hid/usb_prop.o \
./Arm/util/hid/usb_pwr.o 

C_DEPS += \
./Arm/util/hid/hw_config.d \
./Arm/util/hid/usb_desc.d \
./Arm/util/hid/usb_endp.d \
./Arm/util/hid/usb_istr.d \
./Arm/util/hid/usb_prop.d \
./Arm/util/hid/usb_pwr.d 


# Each subdirectory must supply rules for building sources it contributes
Arm/util/hid/%.o: ../Arm/util/hid/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


