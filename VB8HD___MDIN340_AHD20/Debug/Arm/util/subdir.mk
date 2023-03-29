################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Arm/util/newlib_stubs.c \
../Arm/util/stm32_eval_spi_flash.c 

OBJS += \
./Arm/util/newlib_stubs.o \
./Arm/util/stm32_eval_spi_flash.o 

C_DEPS += \
./Arm/util/newlib_stubs.d \
./Arm/util/stm32_eval_spi_flash.d 


# Each subdirectory must supply rules for building sources it contributes
Arm/util/%.o: ../Arm/util/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


