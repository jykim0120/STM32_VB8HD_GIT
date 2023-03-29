################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Video/NVP6124B.c \
../Video/nvp1918c_rx.c \
../Video/nvp1918c_tx.c \
../Video/nvp6011.c \
../Video/nvp6114.c \
../Video/tw9960.c \
../Video/video380.c 

OBJS += \
./Video/NVP6124B.o \
./Video/nvp1918c_rx.o \
./Video/nvp1918c_tx.o \
./Video/nvp6011.o \
./Video/nvp6114.o \
./Video/tw9960.o \
./Video/video380.o 

C_DEPS += \
./Video/NVP6124B.d \
./Video/nvp1918c_rx.d \
./Video/nvp1918c_tx.d \
./Video/nvp6011.d \
./Video/nvp6114.d \
./Video/tw9960.d \
./Video/video380.d 


# Each subdirectory must supply rules for building sources it contributes
Video/%.o: ../Video/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


