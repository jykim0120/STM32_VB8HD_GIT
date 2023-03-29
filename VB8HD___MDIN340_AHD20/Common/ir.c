
//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	IR.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	<stdio.h>
#include	"..\video\video.h"
#include	"..\graphic\osd.h"

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

#define		IRCODE_NUM_1				0x12ed
#define		IRCODE_NUM_2				0x13ec
#define		IRCODE_NUM_3				0x14eb
#define		IRCODE_NUM_4				0x15ea
#define		IRCODE_NUM_5				0x18e7
#define		IRCODE_NUM_6				0x19e6
#define		IRCODE_NUM_7				0x1ae5
#define		IRCODE_UP					0x02fd
#define		IRCODE_DOWN					0x0ef1
#define		IRCODE_LEFT					0x07f8
#define		IRCODE_RIGHT				0x09f6


// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
BYTE IrPulseCount=0, IrNothingCount;
WORD IrComCommand,OldIrComCommand, IrCustomCode, IrDataCode;
static volatile BOOL fIrCodeCmdParsed, fIrCodeCompleted, fIrContinueCheck;
static volatile BOOL fIrCodeUARTprintf;

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

void IrReceivePulseDecode(void)
{
	WORD count = TIM2->CNT;		// leader: 13.5ms(21600), '0': 1.12ms(1792), '1': 2.24ms(3584)

	TIM_Cmd(TIM2, DISABLE);		TIM2->CNT = 0;
	TIM_Cmd(TIM2, ENABLE);

	if (IrPulseCount==0) 
	{
		if (count>=20100&&count<=22100);		// lead pulse	21100 +- 1000
		else if (fIrContinueCheck==FALSE)
				IrPulseCount = (CHAR)-1;
		else if (count>=16500&&count<=18500)	// continue	17500 +- 1000
				fIrCodeCompleted = 1;			// resolve code
		else	IrPulseCount = (CHAR)-1;
	}
	else if (count>=3010&&count<=4010) 	// "0" data 	3510 +- 500
	{
		if (IrPulseCount<=16) IrCustomCode<<=1;
		else if (IrCustomCode==0xFB04) IrDataCode>>=1;	// for NEC
		else	IrPulseCount = (CHAR)-1;
	}
	else if (count>=1270&&count<=2270) 	// "1" data		1770 +- 500
	{
		if (IrPulseCount<=16) IrCustomCode<<=1;
		else if (IrCustomCode==0xFB04) IrDataCode>>=1;	// for NEC
		else	IrPulseCount = (CHAR)-1;

		if (IrPulseCount<=16) 		   IrCustomCode++;
		else						IrDataCode|=0x8000;	// for NEC
	}
	else	IrPulseCount = (CHAR)-1;

	if (IrPulseCount==32)
	{
		fIrCodeCompleted = 1;	// resolve code
		
	}
	
	if (IrPulseCount<32) IrPulseCount++;
	else				 IrPulseCount=0;
}


//--------------------------------------------------------------------------------------------------------------------------
void IrCodeCommandParse(void)
{
	if (!fIrCodeCompleted) return;
	fIrCodeCompleted = FALSE;	IrNothingCount = 14;	// Time-out is 140ms

	fIrContinueCheck = TRUE;	fIrCodeCmdParsed = TRUE;
	IrComCommand = IrDataCode;	fIrCodeUARTprintf = TRUE;
}

//--------------------------------------------------------------------------------------------------------------------------
void IrCodeTimeOutCounter(void)
{
	if (IrNothingCount) IrNothingCount--;
	if (IrNothingCount) return;

	fIrContinueCheck = FALSE;
}
//--------------------------------------------------------------------------------------------------------------------------
extern BYTE disp_mode;
extern BYTE fMsgExecRepeatKey;
extern BYTE KeyCmdCode;
extern BYTE KeyPushed;
void IrCodeMessageExecution(void)
{
	IrCodeCommandParse();

	if(IrComCommand == 0) return;

	if(OldIrComCommand == IrComCommand) return;
	
	fMsgExecRepeatKey = LOW;
	KeyPushed = HIGH;

	switch (IrComCommand) 
	{
		case IRCODE_NUM_1 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_ALL;
		break;
		case IRCODE_NUM_2 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_CH01;
		break;
		case IRCODE_NUM_3 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_CH02;
		break;
		case IRCODE_NUM_4 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_CH03;
		break;
		case IRCODE_NUM_5 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_CH04;
		break;
		case IRCODE_NUM_6 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_CH12;
		break;
		case IRCODE_NUM_7 :
				KeyCmdCode = BUTTON1;
				disp_mode = MDIN_4CHVIEW_CH34;
		break;
		default :
				KeyPushed = LOW;
		break;
	}
	OldIrComCommand = IrComCommand;
	IrComCommand = 0x00;
}

/*  FILE_END_HERE */
