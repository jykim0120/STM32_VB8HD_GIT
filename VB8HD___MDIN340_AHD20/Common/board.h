//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name			:  BOARD.H
// Description			:  This file contains typedefine for the driver files	
// Ref. Docment			: 
// Revision History 	:

#ifndef		__BOARD_H__  
#define		__BOARD_H__  

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------
#ifndef		__STMCORTEX_H__
#include	"..\common\stm32f10x_conf.h"		//by hungry 2012.12.14 for STM32F103xx
#include	"arm\util\stm32_eval_spi_flash.h"	//by hungry 2013.02.01 for STM32F103xx
#endif

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
////////////                   BOARD MODE						/////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
//#define    __NVP1918C_TX__
//#define    __NVP1918C_RX_1CH__
//#define    __NVP1918C_RX__
//#define    __AHD_TX__
#define    __AHD_4CH_RX__
//#define  	 __AHD_2CH_RX__


#define	NVP6114A_FULL_INITIAL_ENABLE			0

#define	NVP6124B_FULL_INITIAL_ENABLE			1

#define	EMBEDED_8BIT							0
#define	SEPERATE_8BIT							1
#define	MUX656_8BIT								2
#define	RGB444_8								3
#define	YUV444_8								4
#define	EMBEDED_10BIT							5
#define	SEPERATE_10BIT							6
#define	MUX656_10BIT							7

#define	MDIN380_INPUT_STYLE_SELECT				MUX656_10BIT


#define	_720p_30								0
#define	_720p_60								1
#define	_1080p_30								2
#define	_1080p_60								3
#define	_1080p_25								4
#define	_1080p_50								5

#define	MDIN380_INPUT_RESOLUTION_SELECT			_1080p_60
#define	MDIN380_OUTPUT_RESOLUTION_SELECT		_1080p_60

#define	_BLUE									0
#define	_WHITE									1
#define	_YELLOW									2
#define	_CYAN									3
#define	_GREEN									4
#define	_MAGENTA								5
#define	_RED									6
#define	_BLUE2									7
#define	_BLACK									8
#define	_GRAY									9

#define	BG_1080P								_BLACK
#define	BG_720P									_BLACK

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

#define 	__I2C_SPEED_100K__		1	// "0" for 357Khz, "1" for 100KHz
#if 1
#define 	__DEBUGPRT_UART1__		1	// "0" for UART0, "1" for UART1	   //for XR
#else
#define 	__DEBUGPRT_UART1__		0	// "0" for UART0, "1" for UART1
#endif
#define     _DEBUG_HDMIRX_			1

#define		__INPUT					0
#define		__OUTPUT				1
#define		KJY_PORTA_1_SELECT		__INPUT


#define		LED_R(x)		(x)? GPIO_SetBits(GPIOA, GPIO_Pin_4): GPIO_ResetBits(GPIOA, GPIO_Pin_4)	//PD2
#define		LED_G(x)		(x)? GPIO_SetBits(GPIOA, GPIO_Pin_5): GPIO_ResetBits(GPIOA, GPIO_Pin_5)	//PC9

#define		MDIN_RST(x)		(x)? GPIO_SetBits(GPIOC, GPIO_Pin_10): GPIO_ResetBits(GPIOC, GPIO_Pin_10)	//define user specific
#define		HDRX_RST(x)		(x)? GPIO_SetBits(GPIOC, GPIO_Pin_11): GPIO_ResetBits(GPIOB, GPIO_Pin_11)	//define user specific
#define     TW_RST(x)		HDRX_RST(x)     
#define		VDEC_RST(x)		(x)? GPIO_SetBits(GPIOC, GPIO_Pin_12): GPIO_ResetBits(GPIOC, GPIO_Pin_12)	//define user specific

#define		KEY_SCAN(x)		x = HIGH	//define user specific

#define		TEST_MODE2(x)	(x)? GPIO_SetBits(GPIOB, GPIO_Pin_9): GPIO_ResetBits(GPIOB, GPIO_Pin_9)	//PB9
#define		TEST_MODE1(x)	(x)? GPIO_SetBits(GPIOB, GPIO_Pin_8): GPIO_ResetBits(GPIOB, GPIO_Pin_8)	//PB8

#define		GET_MDIN_IRQ()	GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4)	//by hungry 2013.05.20 test


#define		GET_MSTAR_STMA()		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

#if 1
#define		GET_MSTAR_STMB()		GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#else
#define 	SET_MSTAR_STMB(x)		(x)? GPIO_SetBits(GPIOA, GPIO_Pin_1): GPIO_ResetBits(GPIOA, GPIO_Pin_1)
#endif


#define		MLD_REQUEST_PIN(x)		(x)? GPIO_SetBits(GPIOA, GPIO_Pin_6): GPIO_ResetBits(GPIOA, GPIO_Pin_6)


#if 1
#define		GET_TRIGGER_1()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)
#define		GET_TRIGGER_2()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)
#define		GET_TRIGGER_3()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
#define		GET_TRIGGER_4()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#else
#define		GET_TRIGGER_4()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0)
#define		GET_TRIGGER_3()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)
#define		GET_TRIGGER_2()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2)
#define		GET_TRIGGER_1()		GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3)
#endif

#define		kjy_boot_speed_enable		1


// -----------------------------------------------------------------------------
// External Variables declaration
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------

#endif	/* __BOARD_H__ */
