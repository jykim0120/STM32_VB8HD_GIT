//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	ISR.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include	"..\usb\usb_lib.h"		//by hungry 2013.06.18 usb
#include	"..\util\hid\usb_pwr.h"	//by hungry 2013.06.18 usb
#include	"..\video\video.h"
#include    "i2c.h"

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
static VBYTE count_10ms;
VBYTE 	Send_Buffer[2];	//by hungry 2013.06.18 usb
NCDEC_MSG pMSG;


extern BYTE Serial2RxCount;
extern BYTE Serial2RxData[50];
extern BYTE Serial2RxLimit;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
extern BOOL fCLR_WKUP;		
extern VBYTE PrevXferComplete;	
extern DWORD ADC_ConvertedValueX;	
extern DWORD ADC_ConvertedValueX_1;	
extern VDWORD bDeviceState; // USB device status 	//by hungry 2013.06.18 usb

extern BYTE	KJY_CTRL_FLAG;
extern WORD	KJY_CTRL_FLAG_COUNT;

//extern BYTE	KJY_AHD_REINITIAL_FLAG;
extern WORD	KJY_AHD_REINITIAL_FLAG_COUNT;


extern BYTE	KJY_UART2_OVERTIME_FLAG;
extern WORD	KJY_UART2_OVERTIME_CNT;


extern WORD kjy_data_retry_count;
extern BYTE kjy_data_retry_flag;


extern BYTE	TRIGGER_FLAG;
extern WORD	TRIGGER_COUNT;
extern WORD	TRIGGER_WAIT_TIME;


extern WORD command_process_cnt;

extern WORD	AUTO_SCAN_COUNT;
extern BYTE	AUTO_SCAN_FLAG;

extern BYTE	MSTAR_LOAD_FLAG;
extern WORD MSTAR_LOAD_COUNT;
// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------
extern void KJY_SET_COMMAND_PROCESS_FLAG(BYTE _dat);
extern BYTE KJY_GET_COMMAND_PROCESS_FLAG(void);


// ----------------------------------------------------------------------
// Global functions
// ----------------------------------------------------------------------
extern void USB_Istr(void);

//--------------------------------------------------------------------------------------------------------------------------

void EXTI9_5_IRQHandler(void)
{
#if 0
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)	
	{
		printf("int\r\n");
		//IrReceivePulseDecode();
	}
	EXTI_ClearITPendingBit(EXTI_Line8);
#endif
}



void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)	
	{
		IrReceivePulseDecode();
	}
	EXTI_ClearITPendingBit(EXTI_Line11);
}


//--------------------------------------------------------------------------------------------------------------------------
static void TimeService_10ms(void)
{
	count_10ms++;
	if ((count_10ms%10)==0)
	{
		IrCodeTimeOutCounter();
	}
	else if ((count_10ms%10)==4) 
	{
	}
}

void TIM2_IRQHandler(void)		// Timer2 - internal timer
{
	TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
}

//--------------------------------------------------------------------------------------------------------------------------
void TIM3_IRQHandler(void)		// Timer3 - internal timer
{
	TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);

	TimeService_10ms();

	if (KJY_CTRL_FLAG_COUNT>0)
			KJY_CTRL_FLAG_COUNT --;
	else
		KJY_CTRL_FLAG = 1;

	if (KJY_AHD_REINITIAL_FLAG_COUNT>0)
			KJY_AHD_REINITIAL_FLAG_COUNT --;

	if (KJY_UART2_OVERTIME_FLAG==1)
	{
		if (KJY_UART2_OVERTIME_CNT>0)
			KJY_UART2_OVERTIME_CNT--;
		else 
			KJY_UART2_OVERTIME_FLAG = 2;
	}
	
	if (kjy_data_retry_flag==1)
	{
		if (kjy_data_retry_count>0)
			kjy_data_retry_count--;
		else
			kjy_data_retry_flag = 2;
	}
	
	if (KJY_GET_COMMAND_PROCESS_FLAG()==1)
	{
		if (command_process_cnt>0)
			command_process_cnt--;
		else
		{
			KJY_SET_COMMAND_PROCESS_FLAG(2);
		}
	}
	
	
	
	#if 0
		if ( (TRIGGER_FLAG>0) && (TRIGGER_FLAG<5) )
		{
				TRIGGER_COUNT--;
		}
	#else
	if ( (TRIGGER_FLAG>0) && (TRIGGER_FLAG<5) )
	{
		if (TRIGGER_COUNT<TRIGGER_WAIT_TIME)
			TRIGGER_COUNT++;
	}
	#endif


	if ( (MLD.MENU_LEVEL == 0) && (TRIGGER_FLAG == 0) )
	{
		if (AUTO_SCAN_FLAG==1)
		{
			if (AUTO_SCAN_COUNT>0)
				AUTO_SCAN_COUNT--;
			else
				AUTO_SCAN_FLAG=2;
		}
	}

	if (KJY_LED_R__FLAG > 0)
	{
		if (KJY_LED_R__COUNT>0)
			KJY_LED_R__COUNT--;
		else
		{
			KJY_LED_R__FLAG++;
			KJY_LED_R_REFRESH();
		}
	}

	if (MSTAR_LOAD_FLAG==2)
	{
		if (MSTAR_LOAD_COUNT>0)
			MSTAR_LOAD_COUNT--;
	}
	

//	TimeService_8ms_TX();		// Added for HDMI Tx.
}

//--------------------------------------------------------------------------------------------------------------------------
void TIM4_IRQHandler(void)		// Timer4 - internal timer
{
	TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);

	if (i2cDelay) i2cDelay--;
	if (usDelay) usDelay--;
	if (msDelay) msDelay--;
}

//--------------------------------------------------------------------------------------------------------------------------
void Serial_Handler(void)		// Serial - UART
{
}

//--------------------------------------------------------------------------------------------------------------------------
void USART1_IRQHandler(void)		
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        SerialRxHandler();
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }

    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
    	SerialTxHandler();
    	USART_ClearITPendingBit(USART1, USART_IT_TXE);
    }
}


//--------------------------------------------------------------------------------------------------------------------------

void SerialPutChar(BYTE c)
{
	while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 
	USART_SendData(USART2, c);
}

void Serial_PutString(BYTE *s)
{
	while (*s != '\0')
	{
		SerialPutChar(*s);
		s++;
	}
}



#if 1			//2 [JYKIM-2023-1-5]	
void USART2_IRQHandler(void)		
{
	BYTE cmd;
	BYTE ack = LOW;
	BYTE i=0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
		if (KJY_UART2_OVERTIME_FLAG == 0)
		{
			KJY_UART2_OVERTIME_FLAG = 1;
		}
		KJY_UART2_OVERTIME_CNT = 250;

		ack = Serial2RxHandler();
//        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        USART_ClearFlag(USART2, USART_IT_RXNE);
    }

	#if 1
	if(ack == HIGH)
	{
//		USART_ClearITPendingBit(USART2, USART_IT_TXE);
		USART_ClearFlag(USART2, USART_IT_TXE);
	}
	#endif

	#if 0
    if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
	#endif

				
}

#else		//2 [JYKIM-2023-1-5]			org
void USART2_IRQHandler(void)		
{
	BYTE cmd;
	BYTE ack = LOW;
	BYTE i=0;

    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        ack = Serial2RxHandler();
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }

	if(ack == HIGH)
	{
		RxCommand(&pMSG);
		TxCommand(&pMSG);

		for(i=0;i<=5;i++)
		{
			Serial2RxData[i]=0;
		}

		for(i=0;i<=Serial2RxCount;i++)
		{
			printf("Serial2RxData[%d]=0x%x\r\n",Serial2RxData[i]);
			pMSG.buf[i] =	Serial2RxData[i];
		}

		USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
				
}
#endif 

//--------------------------------------------------------------------------------------------------------------------------
void USB_LP_CAN1_RX0_IRQHandler(void)	//by hungry 2013.06.18 usb
{
//	UARTprintf("USB_LP_CAN1_RX0_IRQHandler(), run USB_Istr() \r\n");//by hungry 2013.06.18 test

//	printf("USB_LP_CAN1_RX0_IRQHandler()\r\n");
	USB_Istr();
//	if(fCLR_WKUP)	LED_B(OFF);
//	LED_R(1);
}

//--------------------------------------------------------------------------------------------------------------------------
void USBWakeUp_IRQHandler(void)			//by hungry 2013.06.18 usb
{
	//printf("USBWakeUp_IRQHandler() \r\n");//by hungry 2013.06.18 test
	EXTI_ClearITPendingBit(EXTI_Line18);
}


/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  Send_Buffer[0] = 0x07;

  if((ADC_ConvertedValueX >>4) - (ADC_ConvertedValueX_1 >>4) > 4)
  {
    if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
    {
      Send_Buffer[1] = (uint8_t)(ADC_ConvertedValueX >>4);

      // Write the descriptor through the endpoint
      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2);
      SetEPTxValid(ENDP1);
      ADC_ConvertedValueX_1 = ADC_ConvertedValueX;
      PrevXferComplete = 0;
    }
  }

  DMA_ClearFlag(DMA1_FLAG_TC1);

  LED_R(1);	//by hungry 2013.06.18 test
}


/*  FILE_END_HERE */
