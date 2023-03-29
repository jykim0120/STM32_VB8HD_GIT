################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Arm/CMSIS/core_cm3.c \
../Arm/CMSIS/system_stm32f10x.c 

OBJS += \
./Arm/CMSIS/core_cm3.o \
./Arm/CMSIS/system_stm32f10x.o 

C_DEPS += \
./Arm/CMSIS/core_cm3.d \
./Arm/CMSIS/system_stm32f10x.d 


# Each subdirectory must supply rules for building sources it contributes
Arm/CMSIS/%.o: ../Arm/CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


