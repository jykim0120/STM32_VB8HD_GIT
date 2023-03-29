################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/mdin3xx.c \
../Drivers/mdinaux.c \
../Drivers/mdinbus.c \
../Drivers/mdincoef.c \
../Drivers/mdindly.c \
../Drivers/mdinfrmt.c \
../Drivers/mdingac.c \
../Drivers/mdinhtx.c \
../Drivers/mdini2c.c \
../Drivers/mdinipc.c \
../Drivers/mdinosd.c 

OBJS += \
./Drivers/mdin3xx.o \
./Drivers/mdinaux.o \
./Drivers/mdinbus.o \
./Drivers/mdincoef.o \
./Drivers/mdindly.o \
./Drivers/mdinfrmt.o \
./Drivers/mdingac.o \
./Drivers/mdinhtx.o \
./Drivers/mdini2c.o \
./Drivers/mdinipc.o \
./Drivers/mdinosd.o 

C_DEPS += \
./Drivers/mdin3xx.d \
./Drivers/mdinaux.d \
./Drivers/mdinbus.d \
./Drivers/mdincoef.d \
./Drivers/mdindly.d \
./Drivers/mdinfrmt.d \
./Drivers/mdingac.d \
./Drivers/mdinhtx.d \
./Drivers/mdini2c.d \
./Drivers/mdinipc.d \
./Drivers/mdinosd.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/%.o: ../Drivers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


