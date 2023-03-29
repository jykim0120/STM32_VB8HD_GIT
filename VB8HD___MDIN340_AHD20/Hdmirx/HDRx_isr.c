//------------------------------------------------------------------------------
// Copyright �2007, Silicon Image, Inc.  All rights reserved.
//
// No part of this work may be reproduced, modified, distributed, transmitted,
// transcribed, or translated into any language or computer format, in any form
// or by any means without written permission of: Silicon Image, Inc.,
// 1060 East Arques Avenue, Sunnyvale, California 94085
//------------------------------------------------------------------------------
// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include "..\video\video.h"
#include "HDRx_registers.h"

//------------------------------------------------------------------------------
// Choose interrupt polling method
//------------------------------------------------------------------------------

//read interrupt register bit
//#define INTERRUPT_PENDING  ((HDRX_RegRead(REG__INTR_STATE) & (BIT__INTR_GROUP0|BIT__INTR_GROUP1)) != 0)


//------------------------------------------------------------------------------
// Function: SetupInterruptMasks
// Description: Configure interrupt mask registers.
//              Any unmasked interrupt causes the hardware interrupt pin to be set when pendng.
//------------------------------------------------------------------------------
void SetupInterruptMasks(void)
{
#if 0
	BYTE abIntrMasks[7];

	abIntrMasks[0] = 0;
	abIntrMasks[1] = BIT__SCDT_CHG;			  
	abIntrMasks[2] = 0;
	abIntrMasks[3] = 0;
	abIntrMasks[4] = BIT__VRCHG | BIT__HRCHG;
	abIntrMasks[5] = 0;		   
	abIntrMasks[6] = BIT__VIDEO_READY;

	HDRX_MultiWrite(REG__INTR1_UNMASK, &abIntrMasks[0], 4);
	HDRX_MultiWrite(REG__INTR5_UNMASK, &abIntrMasks[4], 2);

	HDRX_MultiWrite(REG__INTR7_UNMASK, &abIntrMasks[6], 1);
#endif
}


//------------------------------------------------------------------------------
// Function: ProcessScdtModeChange
// Description: Process SYNC detect on and off changes
//              This function is only called when the SCDT_CHG interrupt is set.
//------------------------------------------------------------------------------
static void ProcessScdtModeChange(void)
{
#if 0
	BYTE bStateReg;

	bStateReg = HDRX_RegRead(REG__STATE);  //read scdt off or on  	

	if (bStateReg & BIT__SCDT) //SCDT on
	{
		//mute the audio if it is still on, it happens when SCDT off and on in a very short time
		if(CurrentStatus.AudioState == STATE_AUDIO__ON)
		{
			TurnAudioMute(ON);	            
			CurrentStatus.AudioState = STATE_AUDIO__MUTED;
		}

		CurrentStatus.ColorDepth = 0;
		HDRX_RegBitValue(REG__TMDS_CCTRL2, MSK__DC_CTL, VAL__DC_CTL_8BPP_1X);

		CurrentStatus.ResolutionChangeCount = 0;

		CurrentStatus.AudioState = STATE_AUDIO__REQUEST_AUDIO;
	}
	else //SCDT off
	{
		TurnVideoMute(ON);
	}
#endif
}

//------------------------------------------------------------------------------
// Function: ConfigureDeepColor
// Description: Configure the deep color clock based on input video format.
//              Change is made only when input color depth changes.
//------------------------------------------------------------------------------

static void ConfigureDeepColor(void)
{
#if 0
	BYTE abDcCtlVal1x[] = { VAL__DC_CTL_8BPP_1X, VAL__DC_CTL_10BPP_1X, VAL__DC_CTL_12BPP_1X, 0 };
	BYTE abDcCtlVal2x[] = { VAL__DC_CTL_8BPP_2X, VAL__DC_CTL_10BPP_2X, VAL__DC_CTL_12BPP_2X, 0 };
	BYTE bColorDepth;
	BYTE bDcCtlValue;
	BYTE vStatus = OFF;

	bColorDepth = HDRX_RegRead(REG__DC_STAT) & MSK__PIXEL_DEPTH;


	if (bColorDepth != CurrentStatus.ColorDepth)
	{
		if(CurrentStatus.VideoState == STATE_VIDEO__ON)
			vStatus = ON;
		CurrentStatus.ColorDepth = bColorDepth;

		if(vStatus == ON)
			TurnVideoMute(ON);

		//value is 2x if muxYC output is enabled
		if (HDRX_RegRead(REG__VID_AOF) & BIT__MUXYC)
		{
			bDcCtlValue = abDcCtlVal2x[ bColorDepth ];
		}
		else
		{
			bDcCtlValue = abDcCtlVal1x[ bColorDepth ];
		}
		HDRX_RegBitValue(REG__TMDS_CCTRL2, MSK__DC_CTL, bDcCtlValue);
		HDRX_RegBitToggle(REG__SRST2, BIT__DCFIFO_RST);  //reset the deep color FIFO

#ifdef	 _DEBUG_HDMIRX_INTERRUPT
		UARTprintf("DC stat=%02X ctl=%02X\n", bColorDepth, bDcCtlValue);
#endif
		if(vStatus == ON)
			TurnVideoMute(OFF);
	}
#endif
}


//------------------------------------------------------------------------------
// Function: TurnAudioMute
// Description: 
// Note: ON is to mute, OFF is to unmute
//------------------------------------------------------------------------------
void TurnAudioMute (BYTE qOn)
{
	if (qOn) //mute
	{
	}
}






//------------------------------------------------------------------------------
// Function: TurnPowerDown
// Description: Enter or exit powerdown mode
// Note: ON is to powerdown, OFF is to wakeup
//------------------------------------------------------------------------------
void TurnPowerDown(BYTE qOn)
{
#if 0
	if (qOn) //powerDown
	{
		HDRX_RegBitValue(REG__SYS_CTRL1, BIT__PD, CLEAR);
	}
	else  //wakeup
	{
		HDRX_RegBitValue(REG__SYS_CTRL1, BIT__PD, SET); 
#ifdef	 _DEBUG_HDMIRX_INTERRUPT
		UARTprintf("wake up\n");
#endif
	}
#endif
}


//------------------------------------------------------------------------------
// Function: ProcessVCLKChange
// Description: 
// Note: monitor VCLKChange interrupt to configure the zone
//------------------------------------------------------------------------------

#if(PEBBLES_ES1_ZONE_WORKAROUND == ENABLE)

static void ResetZoneControl()
{
#if 0
	if((HDRX_RegRead(REG__FACTORY_A88)&0x0F)>=0x02)	//not 1x zone
	{
		HDRX_RegWrite(REG__FACTORY_A87,0x03);              
		HDRX_RegWrite(REG__FACTORY_A81,0x10);              
		HDRX_RegWrite(REG__FACTORY_A88,0x40);		
		HDRX_RegWrite(REG__FACTORY_A87,0x43);     
		HDRX_RegWrite(REG__FACTORY_A81,0x18); 
#ifdef	 _DEBUG_HDMIRX_INTERRUPT
		UARTprintf("reset zone\n");		  
#endif
	}
#endif
}
	
static BYTE ConfigureZone(void)			
{
#if 0
	if((HDRX_RegRead(REG__STATE))&BIT__PCLK_STABLE) //pclk stable
	{
		return TRUE;
	}
	else
	{
#ifdef	 _DEBUG_HDMIRX_INTERRUPT
		UARTprintf(" **** pclk not stable\n");
#endif
		ResetZoneControl();
	}

	return FALSE;
#endif
	return TRUE;
}

#endif

#if (PEBBLES_ES1_FF_WORKAROUND == ENABLE)
//------------------------------------------------------------------------------
// Function: ProcessVhResolutionChange
// Description: workaround for zone detection issue with FF part
//------------------------------------------------------------------------------

static BYTE ProcessVhResolutionChange(void)
{
#if 0
	if (CurrentStatus.ResolutionChangeCount++ == 10)
	{
		CurrentStatus.ResolutionChangeCount = 0;	
#ifdef	 _DEBUG_HDMIRX_INTERRUPT
		UARTprintf(" **** VHreschng\n");
#endif
		ResetZoneControl();
		
		return TRUE;
	}
#endif
	return FALSE;
}
#endif
//------------------------------------------------------------------------------
// Function: TurnVideoMute
// Description: Enable or disable Video Mute
// Note: ON is to mute, OFF is to unmute
//------------------------------------------------------------------------------
void TurnVideoMute(BYTE qOn)
{
#if 0
	if (qOn) //mute
	{
		HDRX_RegBitValue(REG__HDMI_MUTE, BIT__VIDM, SET);
		CurrentStatus.VideoState = STATE_VIDEO__MUTED;
#ifdef	 _DEBUG_HDMIRX_INTERRUPT
		UARTprintf("Video off\n");
#endif
	}
	else  //unmute
	{
#if(PEBBLES_ES1_ZONE_WORKAROUND == ENABLE)	
//		if((HDRX_RegRead(REG__STATE)& BIT__SCDT)	        //check sync status 
//			&&ConfigureZone())
#endif
		{
			HDRX_RegBitValue(REG__HDMI_MUTE, BIT__VIDM, CLEAR);
			CurrentStatus.VideoState = STATE_VIDEO__ON;
#ifdef	 _DEBUG_HDMIRX_INTERRUPT
			UARTprintf("Video on\n");
#endif
		}
	}
#endif
}
//------------------------------------------------------------------------------
// Function: InterruptHandler
// Description: Main interrupt handler called when HDMI interrupt is pending.
//              All pending interrupts are cleared immediately.
//              Each pending interrupt is then handled by calling the associated handler function.
//------------------------------------------------------------------------------
static void InterruptHandler(void)
{
#if 0
	BYTE abIntrStatus[7];

	//read all interrupt registers
	HDRX_MultiRead(REG__INTR1, &abIntrStatus[0], 4);
	HDRX_MultiRead(REG__INTR5, &abIntrStatus[4], 2);

	HDRX_MultiRead(REG__INTR7, &abIntrStatus[6], 1);


#ifdef	 _DEBUG_HDMIRX_INTERRUPT
	UARTprintf("i: %02X %02X %02X %02X %02X %02X %02X\n",
		abIntrStatus[0],abIntrStatus[1],abIntrStatus[2],
		abIntrStatus[3],abIntrStatus[4],abIntrStatus[5],abIntrStatus[6]);
#endif

	//clear all pending interrupts
	HDRX_MultiWrite(REG__INTR1, &abIntrStatus[0], 4);
	HDRX_MultiWrite(REG__INTR5, &abIntrStatus[4], 2);

	// process pending interrupts
	if (abIntrStatus[1] & BIT__SCDT_CHG)    //if SCDT change
		ProcessScdtModeChange();

	if (abIntrStatus[4] & (BIT__HRCHG|BIT__VRCHG))       //if H res change
	{
#if (PEBBLES_ES1_FF_WORKAROUND==ENABLE)   
		if (!ProcessVhResolutionChange())
#endif
		{
			ConfigureDeepColor();
		}
	}

	if (abIntrStatus[6] & BIT__VIDEO_READY)	//if video ready for unmute
	{
		TurnVideoMute(OFF);
		HDRX_RegWrite(REG__INTR7, BIT__VIDEO_READY);
	}
#endif
}



//------------------------------------------------------------------------------
// Function: PollInterrupt
// Description: Poll for HDMI interrupt.
//              This can be polling the interrupt pin on the processor, 
//              or polling the register over I2C.
//------------------------------------------------------------------------------
void PollInterrupt(void)
{
	//if (INTERRUPT_PENDING)
	//	InterruptHandler();
}
