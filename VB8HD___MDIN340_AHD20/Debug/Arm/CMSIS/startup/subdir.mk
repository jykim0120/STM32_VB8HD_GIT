################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Arm/CMSIS/startup/startup_stm32f10x_hd.c 

OBJS += \
./Arm/CMSIS/startup/startup_stm32f10x_hd.o 

C_DEPS += \
./Arm/CMSIS/startup/startup_stm32f10x_hd.d 


# Each subdirectory must supply rules for building sources it contributes
Arm/CMSIS/startup/%.o: ../Arm/CMSIS/startup/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


