################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/dev_debug.c \
../App/flash_config.c \
../App/jig.c \
../App/kjy_ctrl.c \
../App/main.c 

OBJS += \
./App/dev_debug.o \
./App/flash_config.o \
./App/jig.o \
./App/kjy_ctrl.o \
./App/main.o 

C_DEPS += \
./App/dev_debug.d \
./App/flash_config.d \
./App/jig.d \
./App/kjy_ctrl.d \
./App/main.d 


# Each subdirectory must supply rules for building sources it contributes
App/%.o: ../App/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


