################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Arm/lib/misc.c \
../Arm/lib/stm32f10x_adc.c \
../Arm/lib/stm32f10x_bkp.c \
../Arm/lib/stm32f10x_can.c \
../Arm/lib/stm32f10x_cec.c \
../Arm/lib/stm32f10x_crc.c \
../Arm/lib/stm32f10x_dac.c \
../Arm/lib/stm32f10x_dbgmcu.c \
../Arm/lib/stm32f10x_dma.c \
../Arm/lib/stm32f10x_exti.c \
../Arm/lib/stm32f10x_flash.c \
../Arm/lib/stm32f10x_fsmc.c \
../Arm/lib/stm32f10x_gpio.c \
../Arm/lib/stm32f10x_i2c.c \
../Arm/lib/stm32f10x_iwdg.c \
../Arm/lib/stm32f10x_pwr.c \
../Arm/lib/stm32f10x_rcc.c \
../Arm/lib/stm32f10x_rtc.c \
../Arm/lib/stm32f10x_sdio.c \
../Arm/lib/stm32f10x_spi.c \
../Arm/lib/stm32f10x_tim.c \
../Arm/lib/stm32f10x_usart.c \
../Arm/lib/stm32f10x_wwdg.c 

OBJS += \
./Arm/lib/misc.o \
./Arm/lib/stm32f10x_adc.o \
./Arm/lib/stm32f10x_bkp.o \
./Arm/lib/stm32f10x_can.o \
./Arm/lib/stm32f10x_cec.o \
./Arm/lib/stm32f10x_crc.o \
./Arm/lib/stm32f10x_dac.o \
./Arm/lib/stm32f10x_dbgmcu.o \
./Arm/lib/stm32f10x_dma.o \
./Arm/lib/stm32f10x_exti.o \
./Arm/lib/stm32f10x_flash.o \
./Arm/lib/stm32f10x_fsmc.o \
./Arm/lib/stm32f10x_gpio.o \
./Arm/lib/stm32f10x_i2c.o \
./Arm/lib/stm32f10x_iwdg.o \
./Arm/lib/stm32f10x_pwr.o \
./Arm/lib/stm32f10x_rcc.o \
./Arm/lib/stm32f10x_rtc.o \
./Arm/lib/stm32f10x_sdio.o \
./Arm/lib/stm32f10x_spi.o \
./Arm/lib/stm32f10x_tim.o \
./Arm/lib/stm32f10x_usart.o \
./Arm/lib/stm32f10x_wwdg.o 

C_DEPS += \
./Arm/lib/misc.d \
./Arm/lib/stm32f10x_adc.d \
./Arm/lib/stm32f10x_bkp.d \
./Arm/lib/stm32f10x_can.d \
./Arm/lib/stm32f10x_cec.d \
./Arm/lib/stm32f10x_crc.d \
./Arm/lib/stm32f10x_dac.d \
./Arm/lib/stm32f10x_dbgmcu.d \
./Arm/lib/stm32f10x_dma.d \
./Arm/lib/stm32f10x_exti.d \
./Arm/lib/stm32f10x_flash.d \
./Arm/lib/stm32f10x_fsmc.d \
./Arm/lib/stm32f10x_gpio.d \
./Arm/lib/stm32f10x_i2c.d \
./Arm/lib/stm32f10x_iwdg.d \
./Arm/lib/stm32f10x_pwr.d \
./Arm/lib/stm32f10x_rcc.d \
./Arm/lib/stm32f10x_rtc.d \
./Arm/lib/stm32f10x_sdio.d \
./Arm/lib/stm32f10x_spi.d \
./Arm/lib/stm32f10x_tim.d \
./Arm/lib/stm32f10x_usart.d \
./Arm/lib/stm32f10x_wwdg.d 


# Each subdirectory must supply rules for building sources it contributes
Arm/lib/%.o: ../Arm/lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-none-eabi-gcc -I"D:\NVP1918C_DEMO\src\Arm" -I"D:\NVP1918C_DEMO\src\Arm\CMSIS" -I"D:\NVP1918C_DEMO\src\Arm\lib" -I"D:\NVP1918C_DEMO\src" -Os -g3 -Wall -c -mcpu=cortex-m3 -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


