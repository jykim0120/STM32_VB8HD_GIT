//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  COMMON.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#ifndef		__COMMON_H__
#define		__COMMON_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------

#ifndef		__MDINTYPE_H__
#include	"..\drivers\mdintype.h"
#endif
#ifndef		__BOARD_H__
#include	"..\common\board.h"
#endif
#ifndef		__UCOMI2C_H__
#include	"..\common\i2c.h"
#endif
#ifndef		__MDIN3xx_H__
#include	"..\drivers\mdin3xx.h"
#endif

#define		UARTprintf		printf
#include	<stdio.h>		//arm-none-eabi-gcc standard io library //by hungry 2013.01.24 for 'printf'
// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------
// msg.c


#define	_CH1			0
#define	_CH2			1
#define	_CH3			2
#define	_CH4			3


#define	_UNKNOWN		0xFF
#define	_NO_SIGNAL		0x00
#define	_NTSC_960H		0x01
#define	_PAL_960H		0x02
#define	_AHD30P			0x04
#define	_AHD25P			0x08
#define	_AHD60P			0x10
#define	_AHD50P			0x20
#define	_AFHD30P		0x40
#define	_AFHD25P		0x80



typedef	enum
{
	ENUM_CAMERA_CH_INFO			= 0,
	ENUM_AUTO_POWER				= 1,
	ENUM_AUTO_SCAN				= 2,		
	ENUM_TRIGGER_IN				= 3,
	ENUM_TRIGGER_TIME			= 4,
	ENUM_TRIGGER_PRIORITY		= 5,
	ENUM_CAMERA_DETECT			= 6,
	ENUM_CAMERA1_MIRROR			= 7,
	ENUM_CAMERA2_MIRROR			= 8,
	ENUM_CAMERA3_MIRROR			= 9,
	ENUM_CAMERA4_MIRROR			= 10,
	ENUM_CAMERA1_SCALE			= 11,
	ENUM_CAMERA2_SCALE			= 12,
	ENUM_CAMERA3_SCALE			= 13,
	ENUM_CAMERA4_SCALE			= 14,
	ENUM_CAMERA_SCANTIME		= 15,
	ENUM_MENU_LEVEL				= 16,
	ENUM_MSTAR_LOAD_DATA_MAX = 17,
}	ENUM_MSTAR_LOAD_DATA;



typedef struct _MSTAR_DATA
{
	BYTE	CAMERA_CH_INFO;
	BYTE	AUTO_POWER;
	BYTE	AUTO_SCAN;
	BYTE	OSD;
	BYTE	TRIGGER_IN;
	BYTE	TRIGGER_TIME;
	BYTE	TRIGGER_PRIORITY;
	BYTE	CAMERA_DETECT;
	BYTE	CAMERA1_MIRROR;
	BYTE	CAMERA2_MIRROR;
	BYTE	CAMERA3_MIRROR;
	BYTE	CAMERA4_MIRROR;
	BYTE	CAMERA1_SCALE;
	BYTE	CAMERA2_SCALE;
	BYTE	CAMERA3_SCALE;
	BYTE	CAMERA4_SCALE;
	BYTE	CAMERA_SCANTIME;
	BYTE	MENU_LEVEL;
} MSTAR_LOAD_DATA, *PMSTAR_LOAD_DATA;

MSTAR_LOAD_DATA MLD;



typedef enum {
	BANK0 =0,
	BANK1,
	BANK2,
	BANK3,
	BANK4,
	BANK5,
	BANK6
} 	DEC_BANK;

typedef enum {
	NVP6114A_BANK0 =0,
	NVP6114A_BANK1,
	NVP6114A_BANK2,
	NVP6114A_BANK3,
	NVP6114A_BANK4,
	NVP6114A_BANK5,
	NVP6114A_BANK6,
	NVP6114A_BANK7,
	NVP6114A_BANK8,
	NVP6114A_BANK9,
	NVP6114A_BANKA,
	NVP6114A_BANKB,
} 	NVP6114A_DEC_BANK;


typedef enum {
	NVP6124B_BANK0 =0,
	NVP6124B_BANK1,
	NVP6124B_BANK2,
	NVP6124B_BANK3,
	NVP6124B_BANK4,
	NVP6124B_BANK5,
	NVP6124B_BANK6,
	NVP6124B_BANK7,
	NVP6124B_BANK8,
	NVP6124B_BANK9,
	NVP6124B_BANKA,
	NVP6124B_BANKB,
} 	NVP6124B_DEC_BANK;


typedef enum {
	MSG_KEY = 0, MSG_IR, MSG_UART, MSG_USB
} 	MSG_CTRL_MODE;

typedef enum {
	NTSC = 0, PAL
} 	VFORMAT;


typedef struct {
	BYTE	cmd;
	BYTE    buf[50];
}	stPACKED NCDEC_MSG, *PNCDEC_MSG;


#define		BUTTON1			0xAA
#define		BUTTON2			0xBB
#define		BUTTON3			0xCC
#define		BUTTON4			0xDD
#define		BUTTON5			0xEE

// -----------------------------------------------------------------------------
// External Variables declaration
// -----------------------------------------------------------------------------
extern volatile BOOL fUSBXferMode, fZOOMMove, fCROPMove;


BYTE		KJY_LED_R__FLAG;
WORD		KJY_LED_R__COUNT;

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
// init.c
void uComOnChipInitial(void);

// ir.c
void IrReceivePulseDecode(void);
void IrCodeCommandParse(void);
void IrCodeTimeOutCounter(void);
void IrCodeMessageExecution(void);

// key.c
void ButtonMessageClassify(void);
void ButtonMessageExecution(void);

// serial.c
BYTE SerialRxHandler(void);
void SerialTxHandler(void);
BYTE Serial2RxHandler(void);
void Serial2TxHandler(void);

void KJY_UART2_VARIABLE_INIT(void);

#if 1		//2 [JYKIM-2023-1-5]	 	uart2

void KJY_UART2_RX_PROCESS(void);
void KJY_UART2_COMMAND_CTRL(void);
#endif 		//2 [JYKIM-2023-1-5]		uart2


void kjy__puts_test1(void);

void KJY_UART2_RETRY_INIT(void);
void KJY_UART2_RETRY_CLEAR(void);
void KJY_UART2_SET_RETRY(void);
BYTE KJY_UART2_GET_RETRY(void);

void KJY_NEXT_CHANNEL_CHANGE(void);
void KJY_SEND_CAM_STATUS_INFO(void);
void KJY_SEND_TRIGGER_ON_INFO(BYTE _data);
void KJY_SEND_TRIGGER_OFF_INFO(BYTE _data);



void KJY_UART2_BYTE5_TX(BYTE _data1, BYTE _data2);

void KJY_UART2_PROCESS_INIT(void);
void KJY_SET_COMMAND_PROCESS_FLAG(BYTE _dat);
BYTE KJY_GET_COMMAND_PROCESS_FLAG(void);

void KJY_SEND_CAM_SIGNAL_INFO(BYTE _data);
void KJY_SEND_MLD_REQUEST(void);


// msg.c
void MessageCtrlHandler(void);
#endif	/* __COMMON_H__ */
