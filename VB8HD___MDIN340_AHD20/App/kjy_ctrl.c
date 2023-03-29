/*
 * kjy_ctrl.c
 *
 *  Created on: 2023. 1. 30.
 *      Author: Administrator
 */

#define KJY_CTRL_C

#include	<stdio.h>
#include	<string.h>
#include	<stdarg.h>
#include	"..\video\video.h"
#include 	"..\common\common.h"

#include	"kjy_ctrl.h"


#define		_TRIGGER_CH1			1
#define		_TRIGGER_CH2			2
#define		_TRIGGER_CH3			3
#define		_TRIGGER_CH4			4


#define		_MIRROR_OFF				0
#define		_MIRROR_H				1
#define		_MIRROR_V				2
#define		_MIRROR_H_V				3



BYTE		CAMERA_CH_PREV_STATUS;
BYTE		CAMERA_CH_STATUS;

BYTE		TRIGGER_FLAG;
WORD		TRIGGER_COUNT;
WORD		TRIGGER_WAIT_TIME;


WORD		SCANTIME_COUNT;

WORD		AUTO_SCAN_COUNT;
BYTE		AUTO_SCAN_FLAG;
BYTE		AUTO_SCAN_FIRST_FLAG;

BYTE		MSTAR_LOAD_FLAG;
WORD		MSTAR_LOAD_COUNT;

BYTE		MSTAR_TRIGGER_READY_FLAG;



//MSTAR_LOAD_DATA MLD;


extern BYTE DataReceiveFlag;
extern BYTE DataReceiveFlag2;
extern BYTE	CAMERA_STATUS;

extern BYTE KJY_CTRL_FLAG;

extern BYTE mstar_load_data[];


WORD TRIGGER_TIME_ARRAY[] = {800,1200, 2000, 2500, 3000};

WORD AUTOSCAN_SCANTIME_ARRAY[] = {1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000};


void KJY_DATA_SEND_GPIO_CHECK(void)
{
	if (GET_MSTAR_STMA()==0)
	{
		if (DataReceiveFlag== 0)
		{
			printf("A<");
			DataReceiveFlag = 1;
			USART_Cmd(USART2, ENABLE);
		}
	}
	else
	{
		if (DataReceiveFlag == 1)
		{
//			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//			USART_ClearFlag(USART2, USART_IT_RXNE);
			DataReceiveFlag = 2;
		}
	}
	
	if (GET_MSTAR_STMB()==0)
	{
		if (DataReceiveFlag2== 0)
		{
			printf("B<");
			DataReceiveFlag2 = 1;
			USART_Cmd(USART2, ENABLE);
		}
	}
	else
	{
		if (DataReceiveFlag2 == 1)
		{
//			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
//			USART_ClearFlag(USART2, USART_IT_RXNE);
			DataReceiveFlag2 =2;
		}
	}
}



void KJY_LED_R_INIT(void)
{
//	printf("KJY_LED_R_INIT\r\n");
	KJY_LED_R__FLAG = 1;
}

void KJY_LED_R_CLEAR(void)
{
//	printf("KJY_LED_R_CLEAR\r\n");
	KJY_LED_R__FLAG = 0;
	KJY_LED_R_REFRESH();
	LED_R(OFF);
}

void KJY_LED_R_REFRESH(void)
{
//	printf("KJY_LED_R_REFRESH\r\n");
	KJY_LED_R__COUNT = 500;
}

void KJY_LED_R_CTRL(void)
{
//	printf("_flag=%d\r\n",KJY_LED_R__FLAG);
//	printf("_COUNT=%d\r\n",KJY_LED_R__COUNT);
	if (KJY_LED_R__FLAG == 0)
	{
		return;
	}
	else if (KJY_LED_R__FLAG == 2)
	{
		LED_R(ON);
		KJY_LED_R__FLAG=3;
//		printf("LED_R_ON\r\n");
	}
	else if (KJY_LED_R__FLAG == 4)
	{
		LED_R(OFF);
		KJY_LED_R__FLAG=5;
//		printf("LED_R_OFF\r\n");
	}
	else if (KJY_LED_R__FLAG >= 6)
	{
		KJY_LED_R_INIT();
	}
	else
	{
	}
}



BYTE KJY_TRIGGER_1_PROCESS(void)		//2 [JYKIM-2023-1-18]			CHANNEL 1
{
	BYTE ret=0;
	
	if (GET_TRIGGER_1() == LOW)
	{
		ret = 1;
		printf("DETECT:TR1_LOW\r\n");
		
		if (TRIGGER_FLAG== 0)
		{
			printf("SAVE:CURRENT_CH=%d\r\n",CAMERA_CH_STATUS);
			CAMERA_CH_PREV_STATUS = CAMERA_CH_STATUS;
		}
		

		if (CAMERA_CH_STATUS != _CH1)
		{
			#if 1
			CAMERA_CH_STATUS = _CH1;
			CAMERA_STATUS = _UNKNOWN;
			KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			#else
			CAMERA_CH_STATUS = _CH1;
			CAMERA_STATUS = _UNKNOWN;
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			MDINDLY_mSec(50);
			KJY_SEND_CAM_STATUS_INFO();
			#endif
		}
		else
		{
			if (TRIGGER_FLAG == 0)
			{
				KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			}
		}
		KJY_TRIGGER_FLAG_INIT(_TRIGGER_CH1);		//2 [JYKIM-2023-1-18]		CH 1
	}
	else
	{
		if (TRIGGER_FLAG==_TRIGGER_CH1)
		{
			printf("TR1_CNT=%d\r\n",TRIGGER_COUNT);
			if (TRIGGER_COUNT>= TRIGGER_WAIT_TIME)
			{
				#if 1
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				KJY_SEND_TRIGGER_OFF_INFO(CAMERA_CH_STATUS);//(_UART2_F);
//				MDINDLY_mSec(70);
//				KJY_SEND_CAM_STATUS_INFO();
				#else
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				MDINDLY_mSec(10);
				KJY_SEND_CAM_STATUS_INFO();
				#endif
			}
		}
	}	
	return ret;
}

BYTE KJY_TRIGGER_2_PROCESS(void)		//2 [JYKIM-2023-1-18]			CHANNEL 2
{
	BYTE ret=0;

	if (GET_TRIGGER_2() == LOW)
	{
		printf("DETECT:TR2_LOW\r\n");
		ret = 1;
		if (TRIGGER_FLAG== 0)
		{
			printf("SAVE:CURRENT_CH=%d\r\n",CAMERA_CH_STATUS);
			CAMERA_CH_PREV_STATUS = CAMERA_CH_STATUS;
		}

		if (CAMERA_CH_STATUS != _CH2)
		{
			#if 1
			CAMERA_CH_STATUS = _CH2;
			CAMERA_STATUS = _UNKNOWN;
			KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			#else
			CAMERA_CH_STATUS = _CH2;
			CAMERA_STATUS = _UNKNOWN;
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			MDINDLY_mSec(10);
			KJY_SEND_CAM_STATUS_INFO();
			#endif
		}
		else
		{
			if (TRIGGER_FLAG == 0)
			{
				KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			}
		}
		KJY_TRIGGER_FLAG_INIT(_TRIGGER_CH2);

	}
	else
	{
		if (TRIGGER_FLAG==_TRIGGER_CH2)
		{
			printf("TR2_CNT=%d\r\n",TRIGGER_COUNT);
			if (TRIGGER_COUNT>= TRIGGER_WAIT_TIME)
			{
				#if 1
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				KJY_SEND_TRIGGER_OFF_INFO(CAMERA_CH_STATUS);//(_UART2_F);
				#else
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				MDINDLY_mSec(10);
				KJY_SEND_CAM_STATUS_INFO();
				#endif
			}
		}
	}	
	return ret;	
}
BYTE KJY_TRIGGER_3_PROCESS(void)		//2 [JYKIM-2023-1-18]			CHANNEL 3
{
	BYTE ret=0;

	if (GET_TRIGGER_3() == LOW)
	{
		printf("DETECT:TR3_LOW\r\n");
		ret = 1;
		if (TRIGGER_FLAG== 0)
		{
			printf("SAVE:CURRENT_CH=%d\r\n",CAMERA_CH_STATUS);
			CAMERA_CH_PREV_STATUS = CAMERA_CH_STATUS;
		}


		if (CAMERA_CH_STATUS != _CH3)
		{
			#if 1
			CAMERA_CH_STATUS = _CH3;
			CAMERA_STATUS = _UNKNOWN;
			KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			#else			
			CAMERA_CH_STATUS = _CH3;
			CAMERA_STATUS = _UNKNOWN;
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			MDINDLY_mSec(10);
			KJY_SEND_CAM_STATUS_INFO();
			#endif
		}
		else
		{
			if (TRIGGER_FLAG == 0)
			{
				KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			}
		}
		KJY_TRIGGER_FLAG_INIT(_TRIGGER_CH3);

	}
	else
	{
		if (TRIGGER_FLAG==_TRIGGER_CH3)
		{
			printf("TR3_CNT=%d\r\n",TRIGGER_COUNT);
			if (TRIGGER_COUNT>= TRIGGER_WAIT_TIME)
			{
				#if 1
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				KJY_SEND_TRIGGER_OFF_INFO(CAMERA_CH_STATUS);//(_UART2_F);
				#else
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				MDINDLY_mSec(10);
				KJY_SEND_CAM_STATUS_INFO();
				#endif
			}
		}
	}	
	return ret;	
}

BYTE KJY_TRIGGER_4_PROCESS(void)		//2 [JYKIM-2023-1-18]			CHANNEL 4
{
	BYTE ret=0;

	if (GET_TRIGGER_4() == LOW)
	{
		printf("DETECT:TR4_LOW\r\n");
		ret = 1;
		if (TRIGGER_FLAG== 0)
		{
			printf("SAVE:CURRENT_CH=%d\r\n",CAMERA_CH_STATUS);
			CAMERA_CH_PREV_STATUS = CAMERA_CH_STATUS;
		}

		
		if (CAMERA_CH_STATUS != _CH4)
		{
			#if 1
			CAMERA_CH_STATUS = _CH4;
			CAMERA_STATUS = _UNKNOWN;
			KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			#else
			CAMERA_CH_STATUS = _CH4;
			CAMERA_STATUS = _UNKNOWN;
			kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
			MDINDLY_mSec(10);
			KJY_SEND_CAM_STATUS_INFO();
			#endif
		}
		else
		{
			if (TRIGGER_FLAG == 0)
			{
				KJY_SEND_TRIGGER_ON_INFO(CAMERA_CH_STATUS);
			}
		}
		KJY_TRIGGER_FLAG_INIT(_TRIGGER_CH4);

	}
	else
	{
		if (TRIGGER_FLAG==_TRIGGER_CH4)
		{
			printf("TR4_CNT=%d\r\n",TRIGGER_COUNT);
			if (TRIGGER_COUNT>= TRIGGER_WAIT_TIME)
			{
				#if 1
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				KJY_SEND_TRIGGER_OFF_INFO(CAMERA_CH_STATUS);//(_UART2_F);
				#else
				printf("RETRUN:CURRENT_CH=%d\r\n",CAMERA_CH_PREV_STATUS);
				KJY_TRIGGER_FLAG_CLEAR();
				CAMERA_CH_STATUS = CAMERA_CH_PREV_STATUS;
				CAMERA_STATUS = _UNKNOWN;
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				MDINDLY_mSec(10);
				KJY_SEND_CAM_STATUS_INFO();
				#endif
			}
		}
	}
	return ret;
}


void KJY_TRIGGER_FLAG_INIT(BYTE _ch_data)
{
//	printf("KJY_TRIGGER_FLAG_INIT\r\n");
	TRIGGER_FLAG = _ch_data;
	TRIGGER_COUNT = 0;
	TRIGGER_WAIT_TIME = TRIGGER_TIME_ARRAY[MLD.TRIGGER_TIME];
}

void KJY_TRIGGER_FLAG_CLEAR(void)
{
	TRIGGER_FLAG = 0;
	TRIGGER_COUNT = 0;
	TRIGGER_WAIT_TIME = 0;
}

void KJY_AUTO_SCAN(BYTE dat)
{
	MLD.AUTO_SCAN = dat;
	
//	printf ("KJY_AUTO_SCAN=%d\r\n",MLD.AUTO_SCAN);
//	printf("MLD.MENU_LEVEL=%d",MLD.MENU_LEVEL);

	if (MLD.AUTO_SCAN == 1)
	{
		KJY_AUTO_SCAN_INIT();
		AUTO_SCAN_COUNT = 2000;			//2 [JYKIM-2023-1-31]			first	osdtime 2sec
		AUTO_SCAN_FLAG = 1;
		AUTO_SCAN_FIRST_FLAG = 1;
	}
	else
		KJY_AUTO_SCAN_CLEAR();
}

void KJY_AUTO_SCAN_INIT(void)
{
	AUTO_SCAN_COUNT=AUTOSCAN_SCANTIME_ARRAY[MLD.CAMERA_SCANTIME];
//	printf("SCAN=%d",AUTO_SCAN_COUNT);
	AUTO_SCAN_FLAG = 1;
}

void KJY_AUTO_SCAN_CLEAR(void)
{
//	AUTO_SCAN_COUNT=2000;
	AUTO_SCAN_COUNT=AUTOSCAN_SCANTIME_ARRAY[MLD.CAMERA_SCANTIME];
	AUTO_SCAN_FLAG = 0;
	AUTO_SCAN_FIRST_FLAG = 0;
	MLD.AUTO_SCAN = 0;
}


void KJY_AUTO_SCAN_CTRL(void)
{
	
	if ( (AUTO_SCAN_FLAG == 2) && (TRIGGER_FLAG == 0) )
	{
		KJY_AUTO_SCAN_INIT();

		if (AUTO_SCAN_FIRST_FLAG == 1)
		{
			AUTO_SCAN_FIRST_FLAG = 0;
			
			if(CAMERA_CH_STATUS != _CH1)
			{
				CAMERA_CH_STATUS = _CH1;
				CAMERA_STATUS = _UNKNOWN;
				
				kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
				MDINDLY_mSec(10);
				KJY_SEND_CAM_STATUS_INFO();
			}
		}
		else
			KJY_NEXT_CHANNEL_CHANGE();
	}
}

void KJY_CAM_SCANTIME(BYTE dat)
{
	MLD.CAMERA_SCANTIME = dat;
//	printf("s.time=%d",MLD.CAMERA_SCANTIME);
}


void KJY_MENU_PROCESS(BYTE dat)
{
	MLD.MENU_LEVEL=dat;
//	printf("M%d",MLD.MENU_LEVEL);
}


void KJY_AUTO_POWER(BYTE dat)
{
	MLD.AUTO_POWER = dat;
//	printf("AUTO_POWER=%d",MLD.AUTO_POWER);
}


void KJY_CAM1_MIRROR(BYTE dat)
{
//	BYTE _h_dat,_v_dat;
	
	MLD.CAMERA1_MIRROR = dat;
	
//	printf("CAMERA1_MIRROR=%d",MLD.CAMERA1_MIRROR);
	#if 1
	KJY_CURRENT_CHANNEL_MIRROR_CTRL();
	#else
	
	switch (MLD.CAMERA1_MIRROR)
	{
		default:
		case _MIRROR_OFF :	_h_dat = OFF;
							_v_dat = OFF;
							break;
							
		case _MIRROR_H :	_h_dat = ON;
							_v_dat = OFF;
							break;
							
		case _MIRROR_V :	_h_dat = OFF;
							_v_dat = ON;
							break;
							
		case _MIRROR_H_V :	_h_dat = ON;
							_v_dat = ON;
							break;			
	}
	MDIN3xx_EnableMirrorH(&stVideo, _h_dat);
	MDIN3xx_EnableMirrorV(&stVideo, _v_dat);
	#endif
}

void KJY_CAM2_MIRROR(BYTE dat)
{
//	BYTE _h_dat,_v_dat;

	MLD.CAMERA2_MIRROR = dat;

	#if 1
	KJY_CURRENT_CHANNEL_MIRROR_CTRL();
	#else

//	printf("CAMERA2_MIRROR=%d",MLD.CAMERA2_MIRROR);
	switch (MLD.CAMERA2_MIRROR)
	{
		default:
		case _MIRROR_OFF :	_h_dat = OFF;
							_v_dat = OFF;
							break;
							
		case _MIRROR_H :	_h_dat = ON;
							_v_dat = OFF;
							break;
							
		case _MIRROR_V :	_h_dat = OFF;
							_v_dat = ON;
							break;
							
		case _MIRROR_H_V :	_h_dat = ON;
							_v_dat = ON;
							break;			
	}
	MDIN3xx_EnableMirrorH(&stVideo, _h_dat);
	MDIN3xx_EnableMirrorV(&stVideo, _v_dat);
	#endif

}

void KJY_CAM3_MIRROR(BYTE dat)
{
//	BYTE _h_dat,_v_dat;

	MLD.CAMERA3_MIRROR = dat;

	#if 1
	KJY_CURRENT_CHANNEL_MIRROR_CTRL();
	#else
//	printf("CAMERA3_MIRROR=%d",MLD.CAMERA3_MIRROR);
	switch (MLD.CAMERA3_MIRROR)
	{
		default:
		case _MIRROR_OFF :	_h_dat = OFF;
							_v_dat = OFF;
							break;
							
		case _MIRROR_H :	_h_dat = ON;
							_v_dat = OFF;
							break;
							
		case _MIRROR_V :	_h_dat = OFF;
							_v_dat = ON;
							break;
							
		case _MIRROR_H_V :	_h_dat = ON;
							_v_dat = ON;
							break;			
	}
	MDIN3xx_EnableMirrorH(&stVideo, _h_dat);
	MDIN3xx_EnableMirrorV(&stVideo, _v_dat);
	#endif
}

void KJY_CAM4_MIRROR(BYTE dat)
{
//	BYTE _h_dat,_v_dat;

	MLD.CAMERA4_MIRROR = dat;
//	printf("CAMERA4_MIRROR=%d",MLD.CAMERA4_MIRROR);
	#if 1
	KJY_CURRENT_CHANNEL_MIRROR_CTRL();
	#else
	switch (MLD.CAMERA4_MIRROR)
	{
		default:
		case _MIRROR_OFF :	_h_dat = OFF;
							_v_dat = OFF;
							break;
							
		case _MIRROR_H :	_h_dat = ON;
							_v_dat = OFF;
							break;
							
		case _MIRROR_V :	_h_dat = OFF;
							_v_dat = ON;
							break;
							
		case _MIRROR_H_V :	_h_dat = ON;
							_v_dat = ON;
							break;			
	}
	MDIN3xx_EnableMirrorH(&stVideo, _h_dat);
	MDIN3xx_EnableMirrorV(&stVideo, _v_dat);
	#endif
}

void KJY_CURRENT_CHANNEL_MIRROR_CTRL(void)
{
	BYTE _current_ch, _h_dat, _v_dat;
	
	switch (CAMERA_CH_STATUS)
	{
		default:
		case 0 :	_current_ch = MLD.CAMERA1_MIRROR;
					break;

		case 1 :	_current_ch = MLD.CAMERA2_MIRROR;
					break;

		case 2 :	_current_ch = MLD.CAMERA3_MIRROR;
					break;

		case 3 :	_current_ch = MLD.CAMERA4_MIRROR;
					break;
	}
	
	switch (_current_ch)
	{
		default:
		case _MIRROR_OFF :	_h_dat = OFF;
							_v_dat = OFF;
							break;
							
		case _MIRROR_H :	_h_dat = ON;
							_v_dat = OFF;
							break;
							
		case _MIRROR_V :	_h_dat = OFF;
							_v_dat = ON;
							break;
							
		case _MIRROR_H_V :	_h_dat = ON;
							_v_dat = ON;
							break;			
	}
	
	MDIN3xx_EnableMirrorH(&stVideo, _h_dat);
	MDIN3xx_EnableMirrorV(&stVideo, _v_dat);
}


void KJY_TRIGGER_IN(BYTE dat)
{
	MLD.TRIGGER_IN = dat;
//	printf ("TRIGGER_IN=%d",MLD.TRIGGER_IN);
}


void KJY_OSD(BYTE dat)
{
	MLD.OSD = dat;
//	printf ("OSD=%d",MLD.OSD);
}


void KJY_CAM_STATUS_PROCESS(void)
{
	KJY_SEND_CAM_SIGNAL_INFO((BYTE) CAMERA_STATUS);
//	printf("CAM=%d\r\n",CAMERA_STATUS);
}


void KJY_TRIGGER_TIME(BYTE dat)
{
	MLD.TRIGGER_TIME = dat;

	#if 0
	switch(MLD.TRIGGER_TIME)
	{
		case 0:	printf("MLD.TRIGGER_TIME=800");
				break;

		defalut:
		case 1:	printf("MLD.TRIGGER_TIME=1200");
				break;
				
		case 2:	printf("MLD.TRIGGER_TIME=2000");
				break;

		case 3:	printf("MLD.TRIGGER_TIME=2500");
				break;

		case 4:	printf("MLD.TRIGGER_TIME=3000");
				break;
	}
	#endif	
}


void KJY_TRIGGER_PRIORITY(BYTE dat)
{
	MLD.TRIGGER_PRIORITY = dat;

	#if 0
	switch(MLD.TRIGGER_PRIORITY)
	{
		defalut:
		case 0:	printf("MLD.TRIGGER_PRIORITY=CAM_1");
				break;

		case 1:	printf("MLD.TRIGGER_PRIORITY=CAM_2");
				break;
				
		case 2:	printf("MLD.TRIGGER_PRIORITY=CAM_3");
				break;

		case 3:	printf("MLD.TRIGGER_PRIORITY=CAM_4");
				break;
	}
	#endif
	
}



void KJY_TRIGGER_CTRL(void)
{
	BYTE tmp;

	if (MSTAR_TRIGGER_READY_FLAG == 1)
	{
		return;
	}

	switch(MLD.TRIGGER_PRIORITY)
	{
		defalut:
		case _CH1 :	
					tmp = KJY_TRIGGER_1_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_2_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_3_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_4_PROCESS();
					if (tmp ==1)	return;
					break;

		case _CH2 :	
					tmp = KJY_TRIGGER_2_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_3_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_4_PROCESS();
					if (tmp ==1)	return;

					tmp = KJY_TRIGGER_1_PROCESS();
					if (tmp ==1)	return;
					break;

		case _CH3 :	
					tmp = KJY_TRIGGER_3_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_4_PROCESS();
					if (tmp ==1)	return;

					tmp = KJY_TRIGGER_1_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_2_PROCESS();
					if (tmp ==1)	return;
					break;

		case _CH4 :	
					tmp = KJY_TRIGGER_4_PROCESS();
					if (tmp ==1)	return;

					tmp = KJY_TRIGGER_1_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_2_PROCESS();
					if (tmp ==1)	return;
					
					tmp = KJY_TRIGGER_3_PROCESS();
					if (tmp ==1)	return;	
					break;

	}

}



void KJY_MSTAR_LOAD_CTRL(void)
{
	BYTE i;
	
	if (MSTAR_LOAD_FLAG	== 1)
	{
		MSTAR_LOAD_FLAG = 0;
		
//		printf("MSTAR_RELOAD\r\n");

		MLD.CAMERA_CH_INFO= 	mstar_load_data[ENUM_CAMERA_CH_INFO];
		MLD.AUTO_POWER = 		mstar_load_data[ENUM_AUTO_POWER];
		MLD.AUTO_SCAN = 		mstar_load_data[ENUM_AUTO_SCAN];		
		MLD.TRIGGER_IN = 		mstar_load_data[ENUM_TRIGGER_IN];
		MLD.TRIGGER_TIME = 		mstar_load_data[ENUM_TRIGGER_TIME];
		MLD.TRIGGER_PRIORITY = 	mstar_load_data[ENUM_TRIGGER_PRIORITY];
		MLD.CAMERA_DETECT = 	mstar_load_data[ENUM_CAMERA_DETECT];
		MLD.CAMERA1_MIRROR = 	mstar_load_data[ENUM_CAMERA1_MIRROR];
		MLD.CAMERA2_MIRROR = 	mstar_load_data[ENUM_CAMERA2_MIRROR];
		MLD.CAMERA3_MIRROR = 	mstar_load_data[ENUM_CAMERA3_MIRROR];
		MLD.CAMERA4_MIRROR = 	mstar_load_data[ENUM_CAMERA4_MIRROR];
		MLD.CAMERA1_SCALE = 	mstar_load_data[ENUM_CAMERA1_SCALE];
		MLD.CAMERA2_SCALE = 	mstar_load_data[ENUM_CAMERA2_SCALE];
		MLD.CAMERA3_SCALE = 	mstar_load_data[ENUM_CAMERA3_SCALE];
		MLD.CAMERA4_SCALE = 	mstar_load_data[ENUM_CAMERA4_SCALE];
		MLD.CAMERA_SCANTIME = 	mstar_load_data[ENUM_CAMERA_SCANTIME];
		MLD.MENU_LEVEL = 		mstar_load_data[ENUM_MENU_LEVEL];

		#if 0
			for(i=0;i<ENUM_MSTAR_LOAD_DATA_MAX;i++)
			{
				printf("mstar_load_data[%d]=%x\r\n",i,mstar_load_data[i]);
			}
		#endif

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
		

		
		if (MLD.CAMERA_CH_INFO > 3)
			MLD.CAMERA_CH_INFO = 0;

		
		CAMERA_CH_STATUS = MLD.CAMERA_CH_INFO;
		CAMERA_STATUS = _UNKNOWN;
		CAMERA_CH_PREV_STATUS = CAMERA_CH_STATUS;
		
//		kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);
//		KJY_SEND_CAM_STATUS_INFO();
		
		if (MLD.AUTO_POWER>1)
			MLD.AUTO_POWER = 1;
		
		if (MLD.AUTO_SCAN>1)
			MLD.AUTO_SCAN = 0;

		if (MLD.TRIGGER_IN>1)
			MLD.TRIGGER_IN = 1;
		
		if (MLD.TRIGGER_TIME>4)
			MLD.TRIGGER_TIME = 1;

		if (MLD.TRIGGER_PRIORITY>3)
			MLD.TRIGGER_PRIORITY = 0;

		if (MLD.CAMERA1_MIRROR>3)
			MLD.CAMERA1_MIRROR = 0;

		if (MLD.CAMERA2_MIRROR>3)
			MLD.CAMERA2_MIRROR = 0;

		if (MLD.CAMERA3_MIRROR>3)
			MLD.CAMERA3_MIRROR = 0;

		if (MLD.CAMERA4_MIRROR>3)
			MLD.CAMERA4_MIRROR = 0;

		kjy_ahd_ch_ctrl(CAMERA_CH_STATUS);


		if (MLD.CAMERA1_SCALE>1)
			MLD.CAMERA1_SCALE = 0;

		if (MLD.CAMERA2_SCALE>1)
			MLD.CAMERA2_SCALE = 0;

		if (MLD.CAMERA3_SCALE>1)
			MLD.CAMERA3_SCALE = 0;

		if (MLD.CAMERA4_SCALE>1)
			MLD.CAMERA4_SCALE = 0;
		
		if (MLD.CAMERA_SCANTIME>9)
			MLD.CAMERA_SCANTIME = 2;

		if (MLD.MENU_LEVEL>1)
			MLD.MENU_LEVEL = 0;

		printf("MLD__COMPLETE\r\n");


		if (MLD.AUTO_SCAN == 1)
		{
			KJY_AUTO_SCAN_INIT();
		}
		else
		{
			KJY_AUTO_SCAN_CLEAR();
		}

//		MDINDLY_mSec(500);
		MSTAR_TRIGGER_READY_FLAG	= 0;
		KJY_CTRL_FLAG = 1;
	}

	
}




#undef KJY_CTRL_C

