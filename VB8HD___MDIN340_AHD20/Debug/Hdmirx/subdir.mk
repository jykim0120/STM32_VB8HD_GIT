################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Hdmirx/HDRx_isr.c \
../Hdmirx/HDRx_main.c 

OBJS += \
./Hdmirx/HDRx_isr.o \
./Hdmirx/HDRx_main.o 

C_DEPS += \
./Hdmirx/HDRx_isr.d \
./Hdmirx/HDRx_main.d 


# Each subdirectory must supply rules for building sources it contributes
Hdmirx/%.o: ../Hdmirx/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


