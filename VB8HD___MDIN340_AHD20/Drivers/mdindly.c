//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2010  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	MDINDLY.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"mdin3xx.h"

#include	"..\common\stm32f10x_conf.h"	// cpu dependent for making delay	//by hungry 2012.12.14 for stm32f103

#ifndef		__BOARD_H__
#include	"..\common\board.h"
#endif

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
VWORD usDelay, msDelay,i2cDelay;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Drive Function for delay (usec and msec)
// You must make functions which is defined below.
//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t I2CDLY_5uSec(WORD delay)
{
	int cnt;

	for(cnt = 0; cnt < delay ; cnt++)  // uart is conflict with timer.
		;
	
/*
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

	TIM_TimeBaseInitStructure.TIM_Period = 4;				// Set period to 5us
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;			// 1us resolution(72M/1M-1)
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	i2cDelay = delay;	TIM_Cmd(TIM4, ENABLE);
	while(i2cDelay);		TIM_Cmd(TIM4, DISABLE);
*/
	return MDIN_NO_ERROR;
}

MDIN_ERROR_t MDINDLY_5uSec(WORD delay)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

	TIM_TimeBaseInitStructure.TIM_Period = 4;				// Set period to 5us
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;			// 1us resolution(72M/1M-1)
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	i2cDelay = delay;	TIM_Cmd(TIM4, ENABLE);
	while(i2cDelay);		TIM_Cmd(TIM4, DISABLE);

	return MDIN_NO_ERROR;
}



MDIN_ERROR_t MDINDLY_10uSec(WORD delay)
{

	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

	TIM_TimeBaseInitStructure.TIM_Period = 9;				// Set period to 10us
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;			// 1us resolution(72M/1M-1)
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	usDelay = delay;	TIM_Cmd(TIM4, ENABLE);
	while(usDelay);		TIM_Cmd(TIM4, DISABLE);

	return MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINDLY_mSec(WORD delay)
{

	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;

	TIM_TimeBaseInitStructure.TIM_Period = 999;				// Set period to 1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 71;			// 1us resolution(72M/1M-1)
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);

	msDelay = delay;	TIM_Cmd(TIM4, ENABLE);
	while(msDelay);		TIM_Cmd(TIM4, DISABLE);

	return MDIN_NO_ERROR;
}

/*  FILE_END_HERE */
