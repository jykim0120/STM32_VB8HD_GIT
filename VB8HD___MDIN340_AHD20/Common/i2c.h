//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  I2C.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------
#include	"..\drivers\mdintype.h"
#include    "board.h"


#if defined(__AHD_2CH_RX__)
	#define		I2C_DEC_ADDR			0x60  
#else
	#define		I2C_DEC_ADDR			0x60  
#endif
#define     I2C_TW9960_ADDR			0x8A


#define	NVP6114A_ADDR				0x60			//2		[ JYKim 		2015-5-4 ]
#define	NVP6124B_ADDR				0x60

// -----------------------------------------------------------------------------
// Exported function Prototype
// -----------------------------------------------------------------------------
// i2c
BYTE I2C_Write(BYTE sAddr, WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE I2C_Read(BYTE sAddr, WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE I2C_Write16(BYTE ID, BYTE dAddr, WORD rAddr, PBYTE pBuff, WORD bytes);	//by hungry 2013.04.25 test
BYTE I2C_Read16(BYTE ID, BYTE dAddr, WORD rAddr, PBYTE pBuff, WORD bytes);	//by hungry 2013.04.25 test

BYTE NVP1918C_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE NVP1918C_RegWrite(WORD rAddr, BYTE wData);
BYTE NVP1918C_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE NVP1918C_RegRead(WORD rAddr);

BYTE AHD_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE AHD_RegWrite(WORD rAddr, BYTE wData);
BYTE AHD_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes);


BYTE TW9960_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE TW9960_RegWrite(WORD rAddr, BYTE wData);
BYTE TW9960_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE TW9960_RegRead(WORD rAddr);

// IT6604E
BYTE HDRX_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE HDRX_RegWrite(WORD rAddr, BYTE wData);
BYTE HDRX_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes);
BYTE HDRX_RegRead(WORD rAddr);
BYTE HDRX_RegBitValue(WORD rAddr, BYTE mask, BYTE value);
BYTE HDRX_RegBitToggle(WORD rAddr, BYTE mask);

