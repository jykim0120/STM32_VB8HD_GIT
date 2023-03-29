################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Common/coaxial.c \
../Common/i2c.c \
../Common/init.c \
../Common/ir.c \
../Common/isr.c \
../Common/key.c \
../Common/serial.c \
../Common/stm32f10x_it.c 

OBJS += \
./Common/coaxial.o \
./Common/i2c.o \
./Common/init.o \
./Common/ir.o \
./Common/isr.o \
./Common/key.o \
./Common/serial.o \
./Common/stm32f10x_it.o 

C_DEPS += \
./Common/coaxial.d \
./Common/i2c.d \
./Common/init.d \
./Common/ir.d \
./Common/isr.d \
./Common/key.d \
./Common/serial.d \
./Common/stm32f10x_it.d 


# Each subdirectory must supply rules for building sources it contributes
Common/%.o: ../Common/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


