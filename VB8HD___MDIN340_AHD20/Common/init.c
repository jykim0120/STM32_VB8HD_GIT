//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2005  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	INIT.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "..\video\video.h"

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
BYTE DeviceState;		//by hungry 2013.06.13 usb
BYTE DeviceStatus[6];	//by hungry 2013.06.13 usb
LONG ADCConvertedValue[10];

#define ApplicationAddress 0x08003000	//hw_config.h //by hungry 2013.06.13 test
typedef  void (*pFunction)(void);	//main.c
pFunction Jump_To_Application;		//main.c

#define	DAC_37KHZ				27	//26.7us -> sine(780Hz) //ding
#define	DAC_30KHZ				33	//33.3us -> sine(625Hz)	//dong
#define	DAC_48KHZ				21	//20.8us -- sample rate 48Khz
#define	DAC_44KHZ				23	//22.675us -- sample rate 44.1Khz
#define	DAC_22KHZ				45	//45.41us -- sample rate 22.05Khz
#define	DAC_11KHZ				91	//90.7us -- sample rate 11.025Khz

#define DAC_DHR8R1_Address      0x40007410	//by hungry 2013.07.02 dac
//#define	DAC_DHR12R1_Address		0x40007408	//by hungry 2013.07.03 dac

BYTE g_strcnt;
BYTE g_bState,g_bState2;

BYTE outbuf[48] = {
	242, 248, 252, 254, 254, 251, 247, 240, 231, 221,
	209, 196, 181, 166, 150, 133, 117, 100, 84, 69,
	55, 42, 30, 20, 12, 6, 2, 0, 0, 3,
	7, 14, 23, 33, 45, 58, 73, 88, 104, 121,
	137, 154, 170, 185, 199, 212, 224, 234
};

BYTE Sine_1k_8b[48] = {
	242, 248, 252, 254, 254, 251, 247, 240, 231, 221,
	209, 196, 181, 166, 150, 133, 117, 100, 84, 69,
	55, 42, 30, 20, 12, 6, 2, 0, 0, 3,
	7, 14, 23, 33, 45, 58, 73, 88, 104, 121,
	137, 154, 170, 185, 199, 212, 224, 234
};

/*
ROMDATA WORD Sine12bit[10] = {
	2048, 3251, 3995, 3996, 3253, 2051, 847, 101, 98, 839
};

ROMDATA WORD Sine12bit[32] = {
	2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
	3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
	599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647
};

ROMDATA WORD Sine12bit[48] = {
	3899, 3997, 4062, 4092, 4087, 4047, 3973, 3867, 3729, 3562,
	3370, 3154, 2920, 2671, 2412, 2146, 1878, 1613, 1356, 1111,
	881, 672, 486, 326, 196, 98, 33, 2, 7, 46,
	120, 226, 363, 530, 722, 937, 1171, 1420, 1679, 1945,
	2213, 2477, 2735, 2980, 3210, 3420, 3606, 3766
};

ROMDATA WORD Sine8bit[48] = {
	242, 248, 252, 254, 254, 251, 247, 240, 231, 221,
	209, 196, 181, 166, 150, 133, 117, 100, 84, 69,
	55, 42, 30, 20, 12, 6, 2, 0, 0, 3,
	7, 14, 23, 33, 45, 58, 73, 88, 104, 121,
	137, 154, 170, 185, 199, 212, 224, 234
};
*/
// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Global functions
// ----------------------------------------------------------------------
extern void USB_Init(void);	//arm\usb\usb_init.c //by hungry 2013.06.14 usb

//--------------------------------------------------------------------------------------------------------------------------
static void SetPortDirection(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Periph clock enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
							RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
////////////////////////////////////////////////////////////////////////////////////////////
	//2 [JYKIM-2023-1-18]	// Configure output mode(GPIO_OUTPUT)
	#if 1
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2 |GPIO_Pin_4 | GPIO_Pin_5 |GPIO_Pin_6| GPIO_Pin_9;	
	#else
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9;	
	#endif
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;	//PB0,1,8,9,12
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

/////////////////////////////////////////////////////////////////////////////////////////////////
	//2 [JYKIM-2023-1-18]	// Configure input mode(GPIO_INPUT)
	#if 1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1 | GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11;	
	#else
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_10 | GPIO_Pin_11;	
	#endif
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_5 ;	//PC0,4,5,6,7,8	//by hungry 2013.06.07
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 |GPIO_Pin_2|GPIO_Pin_3;	//PC0 : ananlog input
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	MDIN_RST(LOW); 
	VDEC_RST(LOW); 
	HDRX_RST(LOW);
}

//--------------------------------------------------------------------------------------------------------------------------
static void SetJumpBootLoader(void)
{

}

//--------------------------------------------------------------------------------------------------------------------------
static void SetExtIrqFunction(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

#if 1
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11);	  // IR external interrupt

	// Configure EXTI11 line
	EXTI_InitStructure.EXTI_Line = EXTI_Line11;	
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#else
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);	  // IR external interrupt

	// Configure EXTI11 line
	EXTI_InitStructure.EXTI_Line = EXTI_Line8; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
#endif
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// Enable and set EXTI0 Interrupt to the lowest priority
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//	// NVIC Configuration
//	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);	//Set Vector Table base location at 0x08000000
	
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;         // ADC Enable
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//--------------------------------------------------------------------------------------------------------------------------
static void SetTimerFunction(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

	//Timer3,4 for delay & etc
	// *HSI/PLLCLK/HSE -> SYSCLK -> HCLK -> APB1(PCLK1, TIMxCLK), APB2(PCLK2, TIM1CLK, ADCCLK)
	RCC_PCLK1Config(RCC_HCLK_Div2);							// PCLK1 = HCLK/2 = 72M/2 = 36M
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 |
							RCC_APB1Periph_TIM4, ENABLE);	// TIM3,4 clock enable

	//NVIC Configuration
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
	NVIC_Init(&NVIC_InitStructure);

	//update_event = TIM_CLK / ( (PSC+1)*(ARR+1)*(RCR+1) )
	//PSC:Prescaler, ARR:Auto reload, RCR:Repetition counter
	//PSC=7, ARR=1000, RCR=0, TIM_CLK = 18Mhz
	//update_event = 8M / ((7+1)*(999+1)*(0+1)) = 8M / 8000 = 1KHz, 1ms
	//pre : (SYSCLK / ??M)-1 = (8M/1M)-1 = 7 //1us resolution
	//update_event = 72M / ((71+1)*(999+1)*(0+1)) = 72M / 72000 = 1Khz, 1ms
	TIM_TimeBaseInitStructure.TIM_Period = 999;				// Set period to 1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;			// 1us resolution
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);		// Clear update interrupt bit
	TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);		// Clear update interrupt bit
	TIM_ITConfig(TIM3, TIM_FLAG_Update, ENABLE);	// TIM6,7 interrupt enable
	TIM_ITConfig(TIM4, TIM_FLAG_Update, ENABLE);	// TIM6,7 interrupt enable
	TIM_Cmd(TIM3, ENABLE);
//	TIM_Cmd(TIM4, ENABLE);	// TIM6,7 enable counter

	//Timer2 for IR
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	// TIM2 clock enable

	// Time base configuration
	TIM_TimeBaseInitStructure.TIM_Period = 65535;		// set period to max
	TIM_TimeBaseInitStructure.TIM_Prescaler = 44;		// 72M/(45+1) = 1.6Mhz -> 0.625us resolution
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ITConfig(TIM2, TIM_FLAG_Update, ENABLE);		// TIM2 interrupt enable
	TIM_Cmd(TIM2, ENABLE);
}

//--------------------------------------------------------------------------------------------------------------------------
static void SetUARTFunction(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;	//by hungry 2013.05.29

	//USART Peripheral clock enable.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	//USART1 clk enable //by hungry 2013.01.21
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//USART1 clk enable //by hungry 2013.01.21

	//GPIO Configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_9;	//USART2(Pin2), USART1(Pin9)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_10;	//USART2(Pin3), USART1(Pin10)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Enable the USART1 Interrupt			//by hungry 2013.05.29
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


	//USART Configuration
//	USART_InitStructure.USART_BaudRate = 38400;
//	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_BaudRate = 115200;

	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Init(USART2, &USART_InitStructure);


	// Enable USART1 Receive and Transmit interrupts
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//USART Enable
	USART_Cmd(USART1, ENABLE);

	#if 1		//2 [JYKIM-2022-12-20]	 test
	USART_Cmd(USART2, DISABLE);
	#else		//2 [JYKIM-2022-12-20]			org
	USART_Cmd(USART2, ENABLE);
	#endif

	// Output a message on Hyperterminal using printf function
//	printf("UART(%d0kbps) init completed.. \r\n", (BYTE)(USART_InitStructure.USART_BaudRate/10000));	//by hungry 2013.06.28 test
}

//--------------------------------------------------------------------------------------------------------------------------
static void SetI2CFunction(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef   I2C_InitStructure;

	//I2C_SCL_GPIO_CLK and I2C_SDA_GPIO_CLK Peripheral clock enable
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOB, ENABLE);	//done..

	//I2C Peripheral clock enable.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	//I2C1,2 clk enable.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);	//I2C2 clk enable.

	//GPIO Configuration.
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11;	//I2C1,2(SCL, SDA)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//I2C2 Reset	//from 'AN2824(OptimizedI2Cexamples)'
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	//I2C Configuration.
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;	//I2C_Mode_SMBusHost;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;	//by hungry 2013.04.16 test
	//I2C_InitStructure.I2C_OwnAddress1 = I2C_MDIN3xx_ADDR;	//0xDC	//by hungry 2013.04.16
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

#if (__I2C_SPEED_100K__ == 1)
	I2C_InitStructure.I2C_ClockSpeed = 100000;	//100kbps
#else	// '0'
	I2C_InitStructure.I2C_ClockSpeed = 200000;	//400kbps
#endif

	I2C_Cmd(I2C1, ENABLE);	//h	I2C_Cmd(I2C1, ENABLE);		//I2C enable
	I2C_Cmd(I2C2, ENABLE);

	I2C_Init(I2C1, &I2C_InitStructure);	//h	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Init(I2C2, &I2C_InitStructure);	//h	I2C_Init(I2C1, &I2C_InitStructure);
	//Apply I2C configuration after enabling it 	//by hungry 2013.04.16


}

//--------------------------------------------------------------------------------------------------------------------------
static void SetHDRXEnable(void)
{
	HDRX_RST(HIGH);
}

static void SetADCFunction(void)		   //by hungry 2012.03.07
{
	ADC_InitTypeDef	ADC_InitStructure;
	WORD	AD_value;					//for test

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOC, ENABLE);	//enable ADC1 clock

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//Independent conversion mode(single)
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;		//convert single channel only
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//convert one time
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//no ext. triggering
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//right 12bit data alignment
	ADC_InitStructure.ADC_NbrOfChannel = 1;				//single channel conversion
	ADC_Init(ADC1, &ADC_InitStructure);					//load structure values to registers

	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_41Cycles5);	//ch10, 41.5cycle


	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	ADC_Cmd(ADC1, ENABLE);								//enable ADC1

	ADC_ResetCalibration(ADC1);	// Important, ResetCalibration
	while(ADC_GetResetCalibrationStatus(ADC1));	//enable ADC1 reset calibration
	ADC_StartCalibration(ADC1);	// Import, StartCalibration
	while(ADC_GetCalibrationStatus(ADC1));	//ADC1 calibration
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	//while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));	//start ADC1 conversion
	//AD_value = ADC_GetConversionValue(ADC1);
	//ADC_ClearFlag(ADC1, ADC_FLAG_EOC);					//clear EOC Flag
}

//---------------------------------------------------------------------------------------------
static void SetUSBFunction(void)		   //by hungry 2013.06.13 usb
{
	EXTI_InitTypeDef	EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;	//by hungry 2013.06.18 usb

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);		// DMA1 clock enable

	// Configure the EXTI line 18 connected internally to the USB IP
	EXTI_ClearITPendingBit(EXTI_Line18);
	EXTI_InitStructure.EXTI_Line = EXTI_Line18;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable the USB interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the USB Wake-up interrupt
	NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	// Enable the DMA1 Channel1 Interrupt
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//	NVIC_Init(&NVIC_InitStructure);

	//Set_USBClock()
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);	// Select USBCLK source
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);	// Enable the USB clock

	USB_Init();	//usb_init.c //void USB_Init(void)	//by hungry 2013.06.14 test

}

//---------------------------------------------------------------------------------------------
void SetDACFunction(void)	//by hungry 2013.07.02 dac
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	DAC_InitTypeDef			DAC_InitStructure;
	DMA_InitTypeDef			DMA_InitStructure;
	NVIC_InitTypeDef		NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);		// DMA2 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);		// DAC Peripheral clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	// TIM6 Peripheral clock enable

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//to avoid parasitic consumption set analog
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// TIM6 configuration
	TIM_PrescalerConfig(TIM6, 72, TIM_PSCReloadMode_Update);	// 72M/(71+1) = 1Mhz -> 1us resolution
	//TIM_SetAutoreload(TIM6, DAC_48KHZ);						// Trigger : 48Khz(21us)
	//TIM_SetAutoreload(TIM6, DAC_30KHZ);						// Trigger : 30Khz(33us) - 625Hz
	//TIM_SetAutoreload(TIM6, DAC_37KHZ);						// Trigger : 37Khz(26.7us) - 780Hz
	TIM_SetAutoreload(TIM6, DAC_11KHZ);							// Trigger : 11Khz
	//TIM_SetAutoreload(TIM6, DAC_22KHZ);						// Trigger : 22Khz
	//TIM_SetAutoreload(TIM6, DAC_44KHZ);						// Trigger : 44Khz
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);		// TIM6 TRGO selection

	// DAC channel1 Configuration
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	//DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);

	// DMA configuration
	DMA_DeInit(DMA2_Channel3);
	//DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;	//12bit right align
	DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR8R1_Address;	//8bit right align
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&outbuf;		//DataStartAddr in rom
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 48;//sizeof(outbuf);
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	//DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);

	DMA_ITConfig(DMA2_Channel3, DMA_IT_TC | DMA_IT_HT, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel3_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	DMA_Cmd(DMA2_Channel3, ENABLE);	// Enable DMA2 Channel3
	DAC_Cmd(DAC_Channel_1, ENABLE);	// Enable DAC Channel1: DAC channel1 is enabled, PA.04 is connected to the DAC converter.
	DAC_DMACmd(DAC_Channel_1, ENABLE);	// Enable DMA for DAC Channel1
	TIM_Cmd(TIM6, ENABLE);				// TIM6 enable counter
}

//--------------------------------------------------------------------------------------------------------------------------
void uComOnChipInitial(void)
{
//	__disable_irq();		//by hungry 2013.04.25 test
	SetInitValue();
	SetPortDirection(); 	// set port direction
//	SetI2CFunction();		// set i2c operation
	SetExtIrqFunction();	// set external interrupt
	SetTimerFunction();		// set timer operation
	SetUARTFunction();		// set uart operation
	//SetHDRXEnable();		// set HDRX-VCC enable
	SetADCFunction();		// set ADC operation 
//	SetUSBFunction();		// set USB operation 
//	SetDACFunction();		// set DAC operation 

	MDIN_RST(LOW);
	VDEC_RST(LOW);
	HDRX_RST(LOW);
	LED_G(OFF);
	LED_R(OFF);
}

void SetInitValue(void)
{
	g_bState = 0;
	g_strcnt = 0;
}

/*  FILE_END_HERE */
