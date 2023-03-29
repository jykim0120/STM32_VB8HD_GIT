//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	KEY.C
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
// Static Global Data section variables
// ----------------------------------------------------------------------
#if defined(__AHD_4CH_RX__)
#define MAX_TOGGLE_CH	4
#elif defined(__AHD_2CH_RX__)
#define MAX_TOGGLE_CH	2
#endif


extern __IO uint16_t ADC2ConvertedValue;
extern NCDEC_INFO			ncDec;

BYTE Key1stCode, Key2ndCode, KeyScanCnt;
BYTE KeyCmdCode, OldKeyCmdCode;

//static volatile BOOL fButtonUARTprintf;		// jykim

BYTE KeyPushed;
BYTE fMsgExecRepeatKey;
BYTE disp_mode, toggle_scale=0,toggle_ch=0,toggle_mode=0;
BYTE ptncnt;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------
void vformat_indicator(BYTE vformat)
{
	LED_G(OFF);
	LED_R(OFF);

	if(vformat == NTSC)
	{
		LED_G(ON);
	}
	else
		LED_R(ON);
}

void MessageCtrlHandler(void)
{
	ButtonMessageClassify();
	IrCodeMessageExecution();	
	if((fMsgExecRepeatKey == LOW) && (KeyPushed == HIGH))
	{
		ButtonMessageExecution();
	}	
}

//--------------------------------------------------------------------------------------------------------------------------

void ButtonMessageClassify(void)
{
	WORD keyval;
	
	keyval = ADC2ConvertedValue;

	if(keyval < 200)
	{
		KeyPushed = LOW;
		KeyCmdCode = 0;
		OldKeyCmdCode = 0xFF;
		fMsgExecRepeatKey = LOW;
		return;
	}
	else
	{
		KeyPushed = HIGH;
	}

	if(keyval <1500) 		KeyCmdCode = BUTTON1;
	else if(keyval <2100) 	KeyCmdCode = BUTTON2;
	else if(keyval <2600) 	KeyCmdCode = BUTTON3;
	else if(keyval <3200) 	KeyCmdCode = BUTTON4;
	else if(keyval <4500) 	KeyCmdCode = BUTTON5;
	
	if(KeyCmdCode == OldKeyCmdCode)
		fMsgExecRepeatKey = HIGH;
	else
		fMsgExecRepeatKey = LOW;

	OldKeyCmdCode = KeyCmdCode;
}

#if defined(__NVP1918C_RX__)
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void ButtonMessageExecution(void)
{
	WORD val=0,ch=0;
	
	switch (KeyCmdCode)
	{
		case BUTTON1:  // Change display mode
			if(disp_mode < MDIN_4CHVIEW_CH34_V ) 
				disp_mode++;
			else
				disp_mode = MDIN_4CHVIEW_ALL;
			
			if((ncDec.rx.resolution >= NCRESOL_1280X480) && (ncDec.rx.resolution <= NCRESOL_1440X576))
			{
				if(disp_mode == MDIN_4CHVIEW_ALL) 			disp_mode = MDIN_4CHVIEW_CH01;
				else if(disp_mode == MDIN_4CHVIEW_CH12)		disp_mode = MDIN_4CHVIEW_CH12_V;

				if(disp_mode == MDIN_4CHVIEW_CH12_V)
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0x3);	// enable ch1, ch2, ch3, ch4 // added on 10Mar2014
				else if(disp_mode == MDIN_4CHVIEW_CH34_V)
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0xc);	// enable ch1, ch2, ch3, ch4 // added on 10Mar2014
			}
			else if((ncDec.rx.resolution >= NCRESOL_960X480) && (ncDec.rx.resolution <= NCRESOL_960X576))
			{
			//	if(disp_mode == MDIN_4CHVIEW_CH12) 		disp_mode = MDIN_4CHVIEW_ALL;
				if(disp_mode == MDIN_4CHVIEW_CH12_V) 		disp_mode = MDIN_4CHVIEW_ALL;

				if(disp_mode == MDIN_4CHVIEW_ALL)
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0xF);	// enable ch1, ch2, ch3, ch4 // added on 10Mar2014
				else if(disp_mode == MDIN_4CHVIEW_CH12)
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0x3);	// enable ch1, ch2, ch3, ch4 // added on 10Mar2014
				else if(disp_mode == MDIN_4CHVIEW_CH34)
					MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0xC);	// enable ch1, ch2, ch3, ch4 // added on 10Mar2014
			}
			else 
			{
				if(disp_mode == MDIN_4CHVIEW_CH12_V)
					disp_mode = MDIN_4CHVIEW_ALL;
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0xf);	// enable ch1, ch2, ch3, ch4 // added on 10Mar2014
			}
			
			MDIN3xx_SetDisplay4CH(&stVideo, disp_mode);

			
//			if(disp_mode==MDIN_4CHVIEW_ALL)
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x249, 12,  2, 2);   
//			else
//				MDINHIF_RegField(MDIN_LOCAL_ID, 0x249, 12,  2, 1); 

			if((disp_mode==MDIN_4CHVIEW_CH12) || (disp_mode == MDIN_4CHVIEW_CH34))	// 720x480, 960x480
			{
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 12, 4, 1); //vertically 1ch	// added on 06Mar2014
				if(ncDec.rx.resolution == NCRESOL_960X480)
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x169, 0x1E0);    //vertical screen size
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x16B, 0x12C);    //vertical screen start position
				}
				else if (ncDec.rx.resolution == NCRESOL_960X576)
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x169, 0x240);    //vertical screen size
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x16B, 0x0FC);    //vertical screen start position
				}
			}
			else if((disp_mode == MDIN_4CHVIEW_CH12_V) || (disp_mode == MDIN_4CHVIEW_CH34_V))  //1280 source
			{
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 12, 4, 4); //horizontally 1ch // added on 06Mar2014
				if((ncDec.rx.resolution == NCRESOL_1280X480) || (ncDec.rx.resolution == NCRESOL_1280X576))
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x168, 0x500); //horizontal size
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x16A, 0x140); //horizontal start point
				}
				else if((ncDec.rx.resolution == NCRESOL_1440X480) || (ncDec.rx.resolution == NCRESOL_1440X576))
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x168, 0x5A0); //horizontal size
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x16A, 0x0F0); //horizontal start point
				}
			}
			else if((disp_mode >= MDIN_4CHVIEW_CH01) && (disp_mode <= MDIN_4CHVIEW_CH04))  //1280 source
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 12, 4, 4); //horizontally 1ch // added on 06Mar2014
			else
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 12, 4, 5); //vertically 1ch	// added on 06Mar2014

			if((disp_mode >= MDIN_4CHVIEW_CH01) && (disp_mode <= MDIN_4CHVIEW_CH04))  //3DNR ON/OFF
				MDIN3xx_EnableDeint3DNR(&stVideo,ON);
			else
				MDIN3xx_EnableDeint3DNR(&stVideo,OFF);

			if((disp_mode >= MDIN_4CHVIEW_CH01) && (disp_mode <= MDIN_4CHVIEW_CH04))
			{	
				MDINHIF_RegRead(MDIN_LOCAL_ID, 0x0FD,&val);
				switch(disp_mode)
				{
					case MDIN_4CHVIEW_CH01 : ch = (val>>12)&0x000F;	break;
					case MDIN_4CHVIEW_CH02 : ch = (val>>8)&0x000F;	break;
					case MDIN_4CHVIEW_CH03 : ch = (val>>4)&0x000F;	break;
					case MDIN_4CHVIEW_CH04 : ch = val&0x000F;		break;
				}
				ch = 1<<ch; 					
				MDINHIF_RegField(MDIN_LOCAL_ID, 0x01E, 8,  4, ch); 

				MDINHIF_RegRead(MDIN_LOCAL_ID, 0x01E,&val);  
			}
			DrawCamName(disp_mode, ncDec.rx.resolution);
			break;
		case BUTTON2: 
			break;
		case BUTTON3:
			break;
		case BUTTON4:
			break;
		case BUTTON5:
			if(ncDec.vformat == NTSC)
			{
				ncDec.vformat = PAL;
				printf("PAL set\r\n");
			}
			else
			{
				ncDec.vformat = NTSC;
				printf("NTSC set\r\n");
			}
			vformat_indicator(ncDec.vformat);
			if(ncDec.vformat == NTSC) 
				ncDec.rx.resolution = NCRESOL_960X480;
			else
				ncDec.rx.resolution = NCRESOL_960X576;
			ncDec.rx.pllclk		= PLL_36M;
			ncDec.rx.vclk			= VCLK_72M;
			Write_Config(&ncDec);
			TW9960_DecoderInitial(&ncDec, SVIDEO_TW);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			CreateSBoxInstance();			
			break;
		default:
			break;
	}
}
#elif defined(__NVP1918C_RX_1CH__)
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void ButtonMessageExecution(void)
{
	WORD val=0,ch=0;
	
	switch (KeyCmdCode)
	{
		case BUTTON1:  // Change display mode
			break;
		case BUTTON2: 
			break;
		case BUTTON3:
			if(toggle_scale == 0)
			{
				toggle_scale = 1;

				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x036, 0x3C0);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10A, 0x3C0);

				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x038, 0x1E0);
			
				if(ncDec.vformat == NTSC)
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x1E0);
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x12C);
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x1E0);
				}
				else
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x240);
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x0FC);
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x240);
				}			
			}
			else
			{
				toggle_scale = 0;
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x036, 0x780);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x438);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10A, 0x780);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x438);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x038, 0x000);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x000);
			}
			break;
		case BUTTON4:
			break;
		case BUTTON5:
			if(ncDec.vformat == NTSC)
			{
				ncDec.vformat = PAL;
				printf("PAL set\r\n");
			}
			else
			{
				ncDec.vformat = NTSC;
				printf("NTSC set\r\n");
			}
			vformat_indicator(ncDec.vformat);
			if(ncDec.vformat == NTSC) 
				ncDec.rx.resolution = NCRESOL_960X480;
			else
				ncDec.rx.resolution = NCRESOL_960X576;
			ncDec.rx.pllclk		= PLL_36M;
			ncDec.rx.vclk 			= VCLK_36M;
			Write_Config(&ncDec);
			TW9960_DecoderInitial(&ncDec, SVIDEO_TW);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			CreateSBoxInstance();			
			break;
		default:
			break;
	}
}


#elif defined(__AHD_4CH_RX__) || defined(__AHD_2CH_RX__)
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void ButtonMessageExecution(void)
{
	WORD val=0,ch=0;

	switch (KeyCmdCode)
	{
		case BUTTON1:
			if(ncDec.rx.resolution == NCRESOL_1280X720P)
			{
				ncDec.rx.resolution = (ncDec.vformat == NTSC)?NCRESOL_960X480:NCRESOL_960X576;
				printf("960H set\r\n");
			}
			else
			{
				ncDec.rx.resolution = NCRESOL_1280X720P;
				printf("720P set\r\n");
			}
			Write_Config(&ncDec);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			CreateSBoxInstance();	
			break;
		case BUTTON2: 
			#if defined(__AHD_4CH_RX__) || defined(__AHD_2CH_RX__)
			if(toggle_ch < MAX_TOGGLE_CH)
				toggle_ch++;
			else 
				toggle_ch=0;
			printf("CH%d\r\n",toggle_ch);
//			NVP6114_ChSwitching(toggle_ch);
			#endif
			break;
		case BUTTON3:
			if(toggle_scale == 0)
				toggle_scale = 1;
			else
				toggle_scale = 0;


			if(toggle_scale == 1)
			{
				if(ncDec.rx.resolution == NCRESOL_1280X720P)
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x036, 0x500);  //1280
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x2D0);  //720				
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x038, 0x140);  
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x0B4);  //start vertical position
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10A, 0x500);
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x2D0);
				}
				else // 960H
				{
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x036, 0x3C0);
					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10A, 0x3C0);

					MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x038, 0x1E0);
				
					if(ncDec.vformat == NTSC)
					{
						MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x1E0);
						MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x12C);
						MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x1E0);
					}
					else
					{
						MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x240);
						MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x0FC);
						MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x240);
					}						
				}
			}
			else
			{
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x036, 0x780);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x037, 0x438);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10A, 0x780);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x10B, 0x438);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x038, 0x000);
				MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x039, 0x000);
			}
			break;
		case BUTTON4:
			break;
		case BUTTON5:
			if(ncDec.vformat == NTSC)
			{
				ncDec.vformat = PAL;
				printf("PAL set\r\n");
			}
			else
			{
				ncDec.vformat = NTSC;
				printf("NTSC set\r\n");
			}
			vformat_indicator(ncDec.vformat);
			ncDec.rx.resolution = NCRESOL_1280X720P;
			Write_Config(&ncDec);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			CreateSBoxInstance();			
			break;
		default:
			break;
	}
}

#elif defined(__AHD_TX__)
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void ButtonMessageExecution(void)
{
	WORD val=0,ch=0;
	
	switch (KeyCmdCode)
	{
		case BUTTON1:  // Change display mode
			printf("vformat   = %s\r\n",ncDec.vformat == NTSC?"NTSC":"PAL");
			printf("enc_resol = %s\r\n","1280x720p");
			printf("enc_inputclk = %s\r\n",ncDec.tx.enc_inputclk == ENC_CLK_74M?"74Mhz":"37Mhz");
			printf("bus  = %s\r\n",ncDec.tx.enc_input_bus == ENC_INBUS_16B?"16bit":"8bit");
			break;
		case BUTTON2: 
			ncDec.vformat		= NTSC;
			ncDec.tx.enc_resol 	= NC_ENCSRC_1280X720P;
			ncDec.tx.enc_inputclk	= ENC_CLK_74M;
			ncDec.tx.comet 		= ON;
			ncDec.tx.enc_input_bus = ENC_INBUS_16B;
			printf("vformat   = %s\r\n",ncDec.vformat == NTSC?"NTSC":"PAL");
			printf("enc_resol = %s\r\n","1280x720p");
			printf("enc_inputclk = %s\r\n",ncDec.tx.enc_inputclk == ENC_CLK_74M?"74Mhz":"37Mhz");
			printf("bus  = %s\r\n",ncDec.tx.enc_input_bus == ENC_INBUS_16B?"16bit":"8bit");
			Write_Config(&ncDec);
			DecoderInitial(&ncDec);
			break;
		case BUTTON3:
			if(ncDec.tx.enc_inputclk== ENC_CLK_37M)
				ncDec.tx.enc_inputclk = ENC_CLK_74M;
			else
				ncDec.tx.enc_inputclk = ENC_CLK_37M;
			printf("enc_inputclk = %s\r\n",ncDec.tx.enc_inputclk == ENC_CLK_74M?"74Mhz":"37Mhz");
			Write_Config(&ncDec);
			DecoderInitial(&ncDec);
			break;
		case BUTTON4:
			if(ncDec.tx.enc_input_bus== ENC_INBUS_16B)
				ncDec.tx.enc_input_bus = ENC_INBUS_8B;
			else
				ncDec.tx.enc_input_bus = ENC_INBUS_16B;
			printf("bus  = %s\r\n",ncDec.tx.enc_input_bus == ENC_INBUS_16B?"16bit":"8bit");
			Write_Config(&ncDec);
			DecoderInitial(&ncDec);
			break;
		case BUTTON5:
			if(ncDec.vformat == NTSC)
			{
				ncDec.vformat = PAL;
				printf("PAL set\r\n");
			}
			else
			{
				ncDec.vformat = NTSC;
				printf("NTSC set\r\n");
			}
			vformat_indicator(ncDec.vformat);
			ncDec.tx.enc_resol= NC_ENCSRC_1280X720P;
			Write_Config(&ncDec);
			DecoderInitial(&ncDec);
			break;
		default:
			break;
	}
}
#elif defined(__NVP1918C_TX__)
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
void ButtonMessageExecution(void)
{
	WORD val=0,ch=0;
	
	switch (KeyCmdCode)
	{
		case BUTTON1:  // Change display mode
			break;
		case BUTTON2: 
			break;
		case BUTTON3:
			break;
		case BUTTON4:
			break;
		case BUTTON5:
			if(ncDec.vformat == NTSC)
			{
				ncDec.vformat = PAL;
				ncDec.tx.enc_resol= NC_ENCSRC_720X576;
				printf("PAL set\r\n");
			}
			else
			{
				ncDec.vformat = NTSC;
				ncDec.tx.enc_resol= NC_ENCSRC_720X480;
				printf("NTSC set\r\n");
			}
			vformat_indicator(ncDec.vformat);
			TW9960_DecoderInitial(&ncDec, SVIDEO_TW);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			CreateSBoxInstance();	
			break;
		default:
			break;
	}
}

#endif

/*  FILE_END_HERE */
