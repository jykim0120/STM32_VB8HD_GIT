//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	SERIAL.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	<stdio.h>
#include	<string.h>
#include	<stdarg.h>
#include	"..\video\video.h"

#include 	"..\common\common.h"

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
//#define		UART_COMM_BGN			0xB0
//#define		UART_COMM_END			0xBF

//#define		uCOM_IRAM_ID			0x00

#define		TXBUF(x)				USART_SendData(USART1, x)
#define		RXBUF(x)				x = USART_ReceiveData(USART1)

#define		TX2BUF(x)				USART_SendData(USART2, x)
#define		RX2BUF(x)				x = USART_ReceiveData(USART2)

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
BYTE SerialTxCount, SerialTxData[20], SerialTxLimit = 20;
BYTE SerialRxCount, SerialRxData[20], SerialRxLimit = 20;
BYTE Serial2TxCount, Serial2TxData[50], Serial2TxLimit = 50;
BYTE Serial2RxCount, Serial2RxData[50], Serial2RxLimit = 50;

extern BYTE g_bState,g_bState2;


extern BYTE KJY_UART2_OVERTIME_CNT;
extern BYTE KJY_UART2_OVERTIME_FLAG;


extern WORD kjy_data_retry_count;
extern BYTE kjy_data_retry_flag;

extern BYTE	CAMERA_STATUS;

BYTE KJY_UART2_COMMNAD_BACKUP[2];


#if 1		//2 [JYKIM-2023-1-5]	 uart2

	////////	1	////////////////////
#define	_UART2_START			0x53	// S
	////////	2	////////////////////
#define	_UART2_WRITE			0x00
#define	_UART2_READ				0x01
#define	_UART2_KEY_RECEIVE		0x02
#define	_UART2_IR_RECEIVE		0x03
#define	_UART2_ALL_RECEIVE		0x04
#define	_UART2_O				0x4F
#define	_UART2_R				0x52
#define	_UART2_C				0x43
	////////	3	////////////////////
#define	_UART2_0				0x30
#define	_UART2_1				0x31
#define	_UART2_2				0x32
#define	_UART2_3				0x33
#define	_UART2_4				0x34

#define	_UART2_F				0x46
#define	_UART2_M				0x4D
#define	_UART2_N				0x4E
#define	_UART2_K				0x4B
#define	_UART2_S				0x53
#define	_UART2_T				0x54
#define	_UART2_D				0x44
#define	_UART2_E				0x45
#define	_UART2_F				0x46
#define	_UART2_G				0x47

	////////	4	////////////////////
#define	_UART2_END				0x45	// E
	
	
extern WORD MSTAR_LOAD_COUNT;


	#if 0
	unsigned char vbok_lo[5] = {_UART1_M, _UART1_O,_UART1_K	,_UART1_END,(unsigned char) (_UART1_M+_UART1_O+_UART1_K	+_UART1_END)};
	unsigned char vbok_hi[5] = {_UART1_M, _UART1_O, 0x32	,_UART1_END,(unsigned char)	(_UART1_M+_UART1_O+ 0x32	+_UART1_END)};
	unsigned char vbon[5] = 	{_UART1_M, _UART1_O,_UART1_N,_UART1_END,(unsigned char) (_UART1_M+_UART1_O+_UART1_N	+_UART1_END)};
	unsigned char vbof[5] = 	{_UART1_M, _UART1_O,_UART1_F,_UART1_END,(unsigned char) (_UART1_M+_UART1_O+_UART1_F	+_UART1_END)};
	unsigned char vbtr[5] = 	{_UART1_M, _UART1_T,_UART1_R,_UART1_END,(unsigned char) (_UART1_M+_UART1_T+_UART1_R	+_UART1_END)};

	BYTE mstar_retry[5] =	{_UART1_M, _UART1_R,_UART1_T,_UART1_END,(BYTE) (_UART1_M+ _UART1_R+_UART1_T+_UART1_END)};
	BYTE mstar_ok[5] =		{_UART1_M, _UART1_O,_UART1_K,_UART1_END,(BYTE) (_UART1_M+ _UART1_O+_UART1_K+_UART1_END)};
	#endif

#define		KJY_DEBUG_ENABLE				0
#define		__START__END__CHECKSUM			3
			
#define		KJY_SINGLE_BYTE_LENTH			2 + __START__END__CHECKSUM
#define		KJY_SINGLE_BYTE_CS				KJY_SINGLE_BYTE_LENTH-1

#define		KJY_MULTI_BYTE_LENTH			( (ENUM_MSTAR_LOAD_DATA_MAX) + __START__END__CHECKSUM )
#define		KJY_MULTI_BYTE_CS				KJY_MULTI_BYTE_LENTH-1



/*
	MLD.CAMERA_CH_INFO= 	mstar_load_data[0];
	MLD.AUTO_POWER =		mstar_load_data[1];
	MLD.AUTO_SCAN = 		mstar_load_data[2]; 	
	MLD.TRIGGER_IN =		mstar_load_data[3];
	MLD.TRIGGER_TIME =		mstar_load_data[4];
	MLD.TRIGGER_PRIORITY =	mstar_load_data[5];
	MLD.CAMERA_DETECT = 	mstar_load_data[6];
	MLD.CAMERA1_MIRROR =	mstar_load_data[7];
	MLD.CAMERA2_MIRROR =	mstar_load_data[8];
	MLD.CAMERA3_MIRROR =	mstar_load_data[9];
	MLD.CAMERA4_MIRROR =	mstar_load_data[10];
	MLD.CAMERA1_SCALE = 	mstar_load_data[11];
	MLD.CAMERA2_SCALE = 	mstar_load_data[12];
	MLD.CAMERA3_SCALE = 	mstar_load_data[13];
	MLD.CAMERA4_SCALE = 	mstar_load_data[14];
	MLD.CAMERA_SCANTIME =	mstar_load_data[15];
	MLD.MENU_LEVEL =		mstar_load_data[16];
*/



BYTE process_buf[2];

BYTE mstar_load_data[KJY_MULTI_BYTE_LENTH];

BYTE uart2_rx_all_buf2[KJY_MULTI_BYTE_LENTH];

BYTE DataReceiveFlag;
BYTE DataReceiveFlag2;
BYTE command_process_flag;
WORD command_process_cnt;


extern BYTE	CAMERA_CH_STATUS;
extern BYTE MSTAR_LOAD_FLAG;

extern BYTE MSTAR_TRIGGER_READY_FLAG;

void KJY_UART2_RX_PROCESS(void)
{
	unsigned char i;
	unsigned char _rx_data=0, chksum=0;
	unsigned char _dat1, _dat2;
	unsigned char _err_message=0;
	
	if (DataReceiveFlag==2)						//2 [JYKIM-2023-1-25]			SINGLE-BYTE
	{
		for (i = 0; i<KJY_SINGLE_BYTE_LENTH;i++)
		{
			uart2_rx_all_buf2[i] = Serial2RxData[i];

			if (i<KJY_SINGLE_BYTE_CS)
				chksum = uart2_rx_all_buf2[i] + chksum;
		}
		
		if ((uart2_rx_all_buf2[0] == _UART2_START) && (uart2_rx_all_buf2[KJY_SINGLE_BYTE_CS-1] == _UART2_END) && (uart2_rx_all_buf2[KJY_SINGLE_BYTE_CS] == chksum) )
		{
//			printf("<ok_a\r\n");
			#if 1
			_dat1 = _UART2_O;						//2 [JYKIM-2023-3-10]			OK
			_dat2 = uart2_rx_all_buf2[1];			//2 [JYKIM-2023-3-10]			COMMAND
			#else
			_dat1 = _UART2_O;
			_dat2 = _UART2_K;
			#endif
			process_buf[0] = uart2_rx_all_buf2[1];
			process_buf[1] = uart2_rx_all_buf2[2];
			printf("[%x],[%x]",process_buf[0],process_buf[1]);
			KJY_SET_COMMAND_PROCESS_FLAG(1);
			KJY_UART2_BYTE5_TX(_dat1, _dat2);		//2 [JYKIM-2023-1-20]		RESPONSE :	OK		or	RETRY
		}
		else
		{
			printf("<ds__err:\r\n");
//			_dat1 = _UART2_R;
//			_dat2 = _UART2_T;
			_err_message = 1;
		}
		#if (KJY_DEBUG_ENABLE==1)
			for(i=0;i<KJY_SINGLE_BYTE_LENTH;i++)
			{
				printf("<ds[%x]=%x\r\n",i,Serial2RxData[i]);
			}
		#else
			if (_err_message==1)
			{
				for(i=0;i<KJY_SINGLE_BYTE_LENTH;i++)
				{
					printf("<ds[%x]=%x\r\n",i,Serial2RxData[i]);
				}
			}
		#endif
		
//		KJY_UART2_BYTE5_TX(_dat1, _dat2);		//2 [JYKIM-2023-1-20]		RESPONSE :	OK  	or	RETRY
		DataReceiveFlag = 0;
	}			//2 [JYKIM-2023-1-25]			SINGLE-BYTE
	
//2 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	if (DataReceiveFlag2==2)					//2 [JYKIM-2023-1-25]			MULTI-BYTE
	{
		for (i = 0; i<KJY_MULTI_BYTE_LENTH;i++)
		{
			uart2_rx_all_buf2[i] = Serial2RxData[i];

			if (i<KJY_MULTI_BYTE_CS)
			{
				chksum = uart2_rx_all_buf2[i] + chksum;
			}
 		}
		
		if ((uart2_rx_all_buf2[0] == _UART2_START) && (uart2_rx_all_buf2[KJY_MULTI_BYTE_CS-1] == _UART2_END) && (uart2_rx_all_buf2[KJY_MULTI_BYTE_CS] == chksum) )
		{
			printf("ok_b>\r\n");
			MLD_REQUEST_PIN(0);

			#if 1
				KJY_UART2_BYTE5_TX(_UART2_O, _UART2_M);		//2 [JYKIM-2023-1-20]		RESPONSE :	OK
				for(i=0;i<KJY_MULTI_BYTE_CS;i++)
				{
					mstar_load_data[i] = uart2_rx_all_buf2[i+1];
				}
				MSTAR_LOAD_FLAG = 1;
			#else
			_dat1 = _UART2_O;
			_dat2 = _UART2_M;

			for(i=0;i<KJY_MULTI_BYTE_CS;i++)
			{
				mstar_load_data[i] = uart2_rx_all_buf2[i+1];
			}
			MSTAR_LOAD_FLAG = 1;
			KJY_UART2_BYTE5_TX(_dat1, _dat2);		//2 [JYKIM-2023-1-20]		RESPONSE :	OK		or	RETRY
			#endif
		}
		else
		{
//			printf("<dm__err:\r\n");
//			printf("uart2_rx_all_buf2[KJY_MULTI_BYTE_CS-1]=%x\r\n",uart2_rx_all_buf2[KJY_MULTI_BYTE_CS-1]);
//			printf("uart2_rx_all_buf2[KJY_MULTI_BYTE_CS]=%x\r\n",uart2_rx_all_buf2[KJY_MULTI_BYTE_CS]);
//			printf("chksum=%x\r\n",chksum);
//			_dat1 = _UART2_R;
//			_dat2 = _UART2_T;
			_err_message = 1;
		}
		
		#if (KJY_DEBUG_ENABLE==1)
			for(i=0;i<KJY_MULTI_BYTE_LENTH;i++)
			{
				printf("<dm[%d]=%x\r\n",i,Serial2RxData[i]);
			}
		#else
			if (_err_message==1)
			{
				for(i=0;i<KJY_MULTI_BYTE_LENTH;i++)
				{
					printf("<dm[%d]=%x\r\n",i,Serial2RxData[i]);
				}
			}
		#endif
		
//		KJY_UART2_BYTE5_TX(_dat1, _dat2);		//2 [JYKIM-2023-1-20]		RESPONSE :	OK  	or	RETRY

		#if 0		
		for(i=0;i<KJY_MULTI_BYTE_LENTH;i++)
		{
			printf("load[%d]=%x\r\n",i,load_data[i]);
		}
		#endif
		
		DataReceiveFlag2 = 0;
	}			//2 [JYKIM-2023-1-25]			MULTI-BYTE
}


void KJY_COMMAND_PROCESS_INIT(void)
{	
//	printf("PROCESS_INIT\r\n");
	command_process_flag = 0;
	command_process_cnt = 50;//100;
	process_buf[0] = 0;
	process_buf[1] = 0;
}


void KJY_SET_COMMAND_PROCESS_FLAG(BYTE _dat)
{
	command_process_flag = _dat;
}

BYTE KJY_GET_COMMAND_PROCESS_FLAG(void)
{
	return command_process_flag;
}



void KJY_IR_PROCESS(void)
{
//	printf("KJY_IR_PROCESS02\r\n");
	
	switch (process_buf[1])
	{
//		case IR_SOURCE:
//				KeyDetectTimer = 0;
//				ucIR_Code = IR_SOURCE;
//				remoComplete = 1;
//				uart1_puts("IR_SOURCE_03\r\n");	
//				break;
		case 0x01:				//2 [JYKIM-2023-1-12]	IR_SOURCE:
		case 0x57:				//2 [JYKIM-2023-1-12]	IR_SELECT:
//				uart1_puts("IR_KSELECT_03\r\n");	
//				KeyDetectTimer = 0;
//				ucIR_Code = IR_KSELECT;
//				remoComplete = 1;
				KJY_AUTO_SCAN_CLEAR();
				KJY_NEXT_CHANNEL_CHANGE();
				break;

		case 0x42:			//2 [JYKIM-2023-1-12]	IR_CA1:
//					KeyDetectTimer = 0;
//					ucIR_Code = IR_CA1;
//					remoComplete = 1;
				if(CAMERA_CH_STATUS != _CH1)
				{
					CAMERA_CH_STATUS = _CH1;
					CAMERA_STATUS = _UNKNOWN;

					kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				}
				else
				{
					printf("Already:");
				}
				KJY_AUTO_SCAN_CLEAR();
				KJY_SEND_CAM_STATUS_INFO();
				printf("IR_CA1\r\n");	
				break;

		case 0x41:				//2 [JYKIM-2023-1-12]	IR_CA2:
//				KeyDetectTimer = 0;
//				ucIR_Code = IR_CA2;
//				remoComplete = 1;
				if(CAMERA_CH_STATUS != _CH2)
				{
					CAMERA_CH_STATUS = _CH2;
					CAMERA_STATUS = _UNKNOWN;
					kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				}
				else
				{
					printf("Already:");
				}
				
				KJY_AUTO_SCAN_CLEAR();
				KJY_SEND_CAM_STATUS_INFO();
				printf("IR_CA2\r\n");	
				break;				

		case 0x40:				//2 [JYKIM-2023-1-12]	IR_CA3:
//				KeyDetectTimer = 0;
//				ucIR_Code = IR_CA3;
//				remoComplete = 1;
				if (CAMERA_CH_STATUS != _CH3)
				{
					CAMERA_CH_STATUS = _CH3;
					CAMERA_STATUS = _UNKNOWN;
					kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				}
				else
				{
					printf("Already:");
				}
				KJY_AUTO_SCAN_CLEAR();
				KJY_SEND_CAM_STATUS_INFO();
				printf("IR_CA3\r\n");
				break;

		case 0x03:				//2 [JYKIM-2023-1-12]	IR_CA4:
//				KeyDetectTimer = 0;
//				ucIR_Code = IR_CA4;
//				remoComplete = 1;
				if (CAMERA_CH_STATUS != _CH4)
				{
					CAMERA_CH_STATUS = _CH4;
					CAMERA_STATUS = _UNKNOWN;
					kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				}
				else
				{
					printf("Already:");
				}
				printf("IR_CA4\r\n");
				KJY_AUTO_SCAN_CLEAR();
				KJY_SEND_CAM_STATUS_INFO();
				break;
			#if 0
	//		case IR_QUAD:
	//				KeyDetectTimer = 0;
	//				ucIR_Code = IR_QUAD;
	//				remoComplete = 1;
				
	//				KJY_UART1_BYTE5_TX(0x30, cCurrentMode);
	//				uart1_puts("IR_QUAD_03\r\n");	
	//				break;
				#endif			
		default :
				break;
	}	
//	MDINDLY_mSec(300);
}


void KJY_KEY_PROCESS(void)				//2 [JYKIM-2023-3-10]				OSDKEY
{
//	unsigned char tmp[5];
//	BYTE i;
//	BYTE j, k=0;
//	printf("KJY_KEY_PROCESS___process_buf[1]=0x%x\r\n",process_buf[1]);

	switch (process_buf[1])
	{
		case 0x30:	//uart1_puts("key_power\r\n");
					break;

		case 0x31 :	//uart1_puts("key_menu\r\n");
					break;

		case 0x32 :	//uart1_puts("KJY_KEY_PROCESS__key_select\r\n");
					KJY_AUTO_SCAN_CLEAR();
					KJY_NEXT_CHANNEL_CHANGE();
					break;

		case 0x33 :	//uart1_puts("key_up\r\n");
					break;

		case 0x34 :	//uart1_puts("key_down\r\n");
					break;

		case 0x35 :	//uart1_puts("key_right\r\n");
					break;

		case 0x36 :	//uart1_puts("key_left\r\n");
					break;

		case 0x37 : // mstar -> current cam request
					KJY_SEND_CAM_STATUS_INFO();
					#if 0
					printf("MLD.CAMERA_CH_INFO = %d\r\n",MLD.CAMERA_CH_INFO);
					printf("MLD.AUTO_POWER = %d\r\n",MLD.AUTO_POWER);
					printf("MLD.AUTO_SCAN = %d\r\n",MLD.AUTO_SCAN);
					printf("MLD.TRIGGER_IN = %d\r\n",MLD.TRIGGER_IN);
					printf("MLD.TRIGGER_TIME = %d\r\n",MLD.TRIGGER_TIME);
					printf("MLD.TRIGGER_PRIORITY = %d\r\n",MLD.TRIGGER_PRIORITY);
					printf("MLD.CAMERA_DETECT = %d\r\n",MLD.CAMERA_DETECT);
					printf("MLD.CAMERA1_MIRROR = %d\r\n",MLD.CAMERA1_MIRROR);
					printf("MLD.CAMERA2_MIRROR = %d\r\n",MLD.CAMERA2_MIRROR);
					printf("MLD.CAMERA3_MIRROR = %d\r\n",MLD.CAMERA3_MIRROR);
					printf("MLD.CAMERA4_MIRROR = %d\r\n",MLD.CAMERA4_MIRROR);
					printf("MLD.CAMERA1_SCALE = %d\r\n",MLD.CAMERA1_SCALE);
					printf("MLD.CAMERA2_SCALE = %d\r\n",MLD.CAMERA2_SCALE);
					printf("MLD.CAMERA3_SCALE = %d\r\n",MLD.CAMERA3_SCALE);
					printf("MLD.CAMERA4_SCALE = %d\r\n",MLD.CAMERA4_SCALE);
					printf("MLD.CAMERA_SCANTIME = %d\r\n",MLD.CAMERA_SCANTIME);
					printf("MLD.MENU_LEVEL = %d\r\n",MLD.MENU_LEVEL);
					#endif
					break;

		default :
				break;
	}
//	MDINDLY_mSec(300);
}


	#if 0
	void KJY_MENU_READ_PROCESS(void)
	{

		switch (process_buf[1])
		{
			case 0x03:
			case 0x05:
			case 0x00:	cCurrentQuadData[5]  = cCurrentQuadData[5] &0xFE;		//2 [JYKIM-2023-1-10]	  osd diasble
						break;

			case 0x01:	cCurrentQuadData[5]  = cCurrentQuadData[5] |0x01;		//2 [JYKIM-2023-1-10]	 osd enable
						break;
						
			case 0x46 :	//uart1_protocol_write(vbok_lo,5);
						uart1_protocol_write(vbof, 5);
						KJY_FACTORY_RESET();
						KJY_UART1_BYTE5_TX(0x30, cCurrentMode);
						break;

			default :
					break;
		}
		
	}
	#endif


void KJY_UART2_COMMAND_CTRL(void)
{
	if (KJY_GET_COMMAND_PROCESS_FLAG()	==2)
	{
		#if 1
		switch (process_buf[0])
		{
			case 0x30:	//KJY_WRITE_PROCESS();
						break;
			
			case 0x31 :	//KJY_READ_PROCESS();
						break;
			
			case 0x32 :	KJY_KEY_PROCESS();
						break;
			
			case 0x33 :	KJY_IR_PROCESS();
						break;
			
			case 0x35 :	KJY_MENU_PROCESS(process_buf[1]);
						break;
			
			case 0x40:	KJY_AUTO_POWER(process_buf[1]);
						break;
			//2		[ JYKim 		2018-2-13 ]	FUNCTION 1						
			case 0x50:	KJY_AUTO_SCAN(process_buf[1]);
						break;
			
			case 0x51:	KJY_TRIGGER_IN(process_buf[1]);
						break;
			
			case 0x52:	KJY_TRIGGER_TIME(process_buf[1]);
						break;
			
			case 0x53:	KJY_TRIGGER_PRIORITY(process_buf[1]);
						break;
			
			case 0x55:	KJY_OSD(process_buf[1]);
						break;
			
			case 0x60:	KJY_CAM1_MIRROR(process_buf[1]);
						break;
			
			case 0x61:	KJY_CAM2_MIRROR(process_buf[1]);
						break;
			
			case 0x62:	KJY_CAM3_MIRROR(process_buf[1]);
						break;
			
			case 0x63:	KJY_CAM4_MIRROR(process_buf[1]);
						break;

			case 0x64:	KJY_CAM_SCANTIME(process_buf[1]);
						break;

			case 0x65:	KJY_CAM_STATUS_PROCESS();
						break;
			default	  :
						break;
		}

		#else
		switch (process_buf[0])
		{
			case 0x30:	KJY_WRITE_PROCESS();
						break;

			case 0x31 :	KJY_READ_PROCESS();
						break;

			case 0x32 :	KJY_KEY_PROCESS();
						break;

			case 0x33 :	//KJY_IR_PROCESS();
						break;

			case 0x34 :	KJY_ALL_READ_PROCESS();
						break;

			case 0x35 :	KJY_MENU_READ_PROCESS();
						break;

			//2		[ JYKim 		2018-2-13 ]	PICTURE 2
			case 0x40:	KJY_AUTO_POWER();
						break;

			//2		[ JYKim 		2018-2-13 ]	FUNCTION 1						
			case 0x50:	KJY_AUTO_SCAN();
						break;
						
			case 0x51:	KJY_TRIGGER_IN();
						break;
						
			case 0x52:	KJY_TRIGGER_TIME();
						break;
						
			case 0x53:	KJY_TRIGGER_PRIORITY();
						break;
						
			case 0x54:	KJY_CAM_DETECT();
						break;

			case 0x60:	KJY_CAM1_MIRROR();
						break;

			case 0x61:	KJY_CAM2_MIRROR();
						break;

			case 0x62:	KJY_CAM3_MIRROR();
						break;

			case 0x63:	KJY_CAM4_MIRROR();
						break;

			case 0x64:	KJY_CAM_SCANTIME();
						break;

			
			default :
					break;
		}
		#endif


		KJY_COMMAND_PROCESS_INIT();
	}

}


#endif		//2 [JYKIM-2023-1-5]		uart2

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
//--------------------------------------------------------------------------------------------------------------------------



BYTE SerialRxHandler(void)
{
	RXBUF(SerialRxData[SerialRxCount]);

	if(SerialRxData[SerialRxCount] == '\r')
	{
		g_bState = HIGH;
	}
	else
		g_bState = LOW;

	TXBUF(SerialRxData[SerialRxCount]);

	if (++SerialRxCount>SerialRxLimit)
	{
		SerialRxCount = 0;
		g_bState = LOW;
	}

	return g_bState;
}

void KJY_Tx2_ctrl(void)
{
	BYTE i;
	
	for(i=0;i<=Serial2RxCount;i++)
	{
		TX2BUF(Serial2RxData[i]);
//		printf("Rx2[%x]=0x%x\r\n",i,Serial2RxData[i]);
	}
//	KJY_UART2_VARIABLE_INIT();
//	Serial_PutString(SEND_WOK);
}


void KJY_UART2_VARIABLE_INIT(void)
{
	BYTE i;
	
	USART_Cmd(USART2, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

//	printf("-TIMEOVER:_UART2_STOP\r\n");
	printf(">\r\n");
//	printf("\r\n");
	
	USART_ClearFlag(USART2, USART_IT_RXNE);
	USART_ClearFlag(USART2, USART_IT_TXE);
	
	Serial2RxCount=0;
	KJY_UART2_OVERTIME_FLAG = 0;
	KJY_UART2_OVERTIME_CNT = 250;//250;
	
	for(i=0;i<KJY_MULTI_BYTE_LENTH;i++)
	{
		Serial2RxData[i]= 0;
		uart2_rx_all_buf2[i]= 0;
	}
	DataReceiveFlag = 0;
	DataReceiveFlag2 = 0;
	
	process_buf[0] = 0;
	process_buf[1] = 0;

	#if 0
//	USART_ClearITPendingBit(USART2, USART_IT_TXE);
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	if( USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET)
	{
		printf("clear_rxflag\r\n");
		USART_ClearFlag(USART2, USART_IT_RXNE);
	}

	if( USART_GetFlagStatus(USART2, USART_IT_TXE) == SET)
	{
		printf("clear_txflag\r\n");
		USART_ClearFlag(USART2, USART_IT_TXE);
	}
	#endif

	
//	MDINDLY_mSec(30);
}

#if 1
BYTE Serial2RxHandler(void)
{
	BYTE i;

	g_bState2 = HIGH;

	if( USART_GetFlagStatus(USART2, USART_IT_RXNE) == SET)
	{
		RX2BUF(Serial2RxData[Serial2RxCount]);
		USART_ClearFlag(USART2, USART_IT_RXNE);
	}
	
	Serial2RxCount++;

	if (Serial2RxCount >= Serial2RxLimit)
	{
		Serial2RxCount = 0;
	}

	return g_bState2;
}

#elif 0
BYTE Serial2RxHandler(void)
{
	BYTE i;

	g_bState2 = LOW;

	RX2BUF(Serial2RxData[Serial2RxCount]);
	
	if(Serial2RxData[Serial2RxCount] == '\r')
//	if(Serial2RxData[Serial2RxCount] == '\n')
	{
		g_bState2 = HIGH;
		KJY_UART2_OVERTIME_FLAG = 2;
	}

	Serial2RxCount++;

	if (Serial2RxCount >= Serial2RxLimit)
	{
		Serial2RxCount = 0;
	}

	return g_bState2;
}
#else		//2 [JYKIM-2023-1-5]	 org
BYTE Serial2RxHandler(void)
{
	BYTE i;

	RX2BUF(Serial2RxData[Serial2RxCount]);

	g_bState2 = LOW;
	if(Serial2RxData[Serial2RxCount] == '\r')
	{
		g_bState2 = HIGH;

	}

	if (++Serial2RxCount>Serial2RxLimit)
	{
		Serial2RxCount = 0;
	}

	return g_bState2;
}
#endif


//--------------------------------------------------------------------------------------------------------------------------
void SerialTxHandler(void)
{
	if (++SerialTxCount>=SerialTxLimit)
	{
		SerialTxCount = 0;	
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	
	}
	else
	{
		TXBUF(SerialTxData[SerialTxCount]);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	
	}
}

void Serial2TxHandler(void)
{
	if (++Serial2TxCount>=Serial2TxLimit)
	{
		Serial2TxCount = 0;	
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);	
	}
	else
	{
		TX2BUF(Serial2TxData[Serial2TxCount]);
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);	
	}
}

void kjy__puts_test1(void)
{
	BYTE i;
//	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	

	Serial2TxData[0] = _UART2_M;
	Serial2TxData[1] = 0x31;
	Serial2TxData[2] = CAMERA_CH_STATUS;//kjy_ahd_ch_status();
	Serial2TxData[3] = _UART2_END;
	Serial2TxData[4] = Serial2TxData[0] + Serial2TxData[1] + Serial2TxData[2] + Serial2TxData[3];

	for(i=0;i<5;i++)
	{
		TX2BUF(Serial2TxData[i]);
		MDINDLY_10uSec(20);
	}
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	

//	USART_ClearITPendingBit(USART2, USART_IT_TXE);
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	USART_ClearFlag(USART2, USART_IT_TXE);
	USART_ClearFlag(USART2, USART_IT_RXNE);
}



void KJY_UART2_RETRY_INIT(void)
{
//	puts("RETRY_INIT\r\n");
	kjy_data_retry_count = 250;
	kjy_data_retry_flag = 1;
}

void KJY_UART2_RETRY_CLEAR(void)
{
//	puts("RETRY_CLEAR\r\n");
	kjy_data_retry_flag = 0;
	KJY_UART2_COMMNAD_BACKUP[0] = 0;
	KJY_UART2_COMMNAD_BACKUP[1] = 0;
}

void KJY_UART2_SET_RETRY(void)
{
//	puts("SET_RETRY\r\n");
	kjy_data_retry_flag = 3;
}

BYTE KJY_UART2_GET_RETRY(void)
{
//	printf("GET_RETRY=%d\r\n",kjy_data_retry_flag);
	return kjy_data_retry_flag;
}


void KJY_NEXT_CHANNEL_CHANGE(void)
{
//	BYTE tmp;
//	tmp = kjy_ahd_ch_status();
//	MDINDLY_mSec(10);

	switch (CAMERA_CH_STATUS)
	{
		case 0: CAMERA_CH_STATUS = _CH2;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				break;
	
		case 1: CAMERA_CH_STATUS = _CH3;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				break;
	
		case 2:	CAMERA_CH_STATUS = _CH4;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				break;

		case 3:	CAMERA_CH_STATUS = _CH1;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				break;
	
		default:
				CAMERA_CH_STATUS = _CH1;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				break;
	}
	CAMERA_STATUS = _UNKNOWN;

	KJY_SEND_CAM_STATUS_INFO();
}

void KJY_SEND_CAM_STATUS_INFO(void)
{
//	printf("KJY_SEND_CAM_STATUS_INFO\r\n");
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, ENABLE);
	
	KJY_UART2_BYTE5_TX(0x31,CAMERA_CH_STATUS);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, DISABLE);
}

void KJY_SEND_TRIGGER_ON_INFO(BYTE _data)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, ENABLE);
	
	KJY_UART2_BYTE5_TX(_UART2_T,_data);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, DISABLE);
}

void KJY_SEND_TRIGGER_OFF_INFO(BYTE _data)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, ENABLE);
	
	KJY_UART2_BYTE5_TX(_UART2_F,_data);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, DISABLE);
}

void KJY_SEND_MLD_REQUEST(void)
{
	if (MSTAR_LOAD_FLAG < 2)
		return;

	if (MSTAR_LOAD_COUNT==0)
	{
		MSTAR_LOAD_COUNT = 1000;
		
//		printf("KJY_SEND_MLD_REQUEST\r\n");
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		USART_Cmd(USART2, ENABLE);
		
		KJY_UART2_BYTE5_TX(_UART2_M,_UART2_D);
		
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		USART_Cmd(USART2, DISABLE);
	}
}



void KJY_SEND_CAM_SIGNAL_INFO(BYTE _data)					//2 [JYKIM-2023-3-10]	cam detect
{
//	printf("KJY_SEND_CAM_STATUS_INFO\r\n");
	#if 0
	if (MSTAR_TRIGGER_READY_FLAG == 1)
	{
		MSTAR_TRIGGER_READY_FLAG = 0;

//		printf("MSTAR_TRIGGER_READY_FLAG = 1-->0\r\n");

		if (GET_TRIGGER_1() == LOW)
		{
			printf("MSTAR_READY__TR1\r\n");
			return;
		}
		if (GET_TRIGGER_2() == LOW)
		{
			printf("MSTAR_READY__TR2\r\n");
			return;
		}
		if (GET_TRIGGER_3() == LOW)
		{
			printf("MSTAR_READY__TR3\r\n");
			return;
		}
		if (GET_TRIGGER_4() == LOW)
		{
			printf("MSTAR_READY__TR4\r\n");
			return;
		}
	}
	#endif
	
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, ENABLE);
						
	KJY_UART2_BYTE5_TX(_UART2_C,_data);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, DISABLE);
}



void KJY_UART2_BYTE5_TX(BYTE _data1, BYTE _data2)
{
	BYTE i, tmp[5];
	
	tmp[0] = _UART2_M;
	tmp[1] = _data1;
	tmp[2] = _data2;
	tmp[3] = _UART2_END;
	tmp[4] = (_UART2_M+_data1+_data2+_UART2_END);

	for(i=0;i<5;i++)	
	{
		TX2BUF(tmp[i]);
		#if 0 //(KJY_DEBUG_ENABLE==1)
		printf(">M[%d]=0x%x\r\n",i,tmp[i]);
		if (i==4)
			printf("--------------------------\r\n");
		#else
		MDINDLY_10uSec(20);
		#endif
	}

	KJY_UART2_COMMNAD_BACKUP[0] = _data1;
	KJY_UART2_COMMNAD_BACKUP[1] = _data2;

//	USART_ClearITPendingBit(USART2, USART_IT_TXE);
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE);
}






