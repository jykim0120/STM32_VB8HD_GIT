//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2010  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	MDINI2C.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"mdin3xx.h"

#include	"..\common\stm32f10x_conf.h"		//by hungry 2013.01.31 for STM32F103
#include	<stdio.h>			//for printf //by hungry 2013.04.16 test

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

#define		TIMEOUT_MAX			0x10

typedef enum
{
	I2C_OK = 0,
	I2C_NOT_FREE,
	I2C_NOT_START,
	I2C_HOST_NACK,
	I2C_NO_DATA,
	I2C_TIME_OUT
}I2C_ErrorCode;

typedef enum
{
	I2C_1 = 0,
	I2C_2 = 1
}I2C_BusID;

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------
static WORD PageID = 0;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------
extern BYTE I2C_Read16(BYTE ID, BYTE dAddr, WORD rAddr, PBYTE pBuff, WORD bytes);		//by hungry 2013.04.25 test
extern BYTE I2C_Write16(BYTE ID, BYTE dAddr, WORD rAddr, PBYTE pBuff, WORD bytes);		//by hungry 2013.04.25 test

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// You must make functions which is defined below.
static BYTE MDINI2C_Write(BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes);
static BYTE MDINI2C_Read(BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes);

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MDINI2C_SetPage(BYTE nID, WORD page)
{
#if	defined(SYSTEM_USE_MDIN380)&&defined(SYSTEM_USE_BUS_HIF)
	MDINBUS_SetPageID(page);	// set pageID to BUS-IF
#endif

	if (page==PageID) return I2C_OK;	PageID = page;
	return MDINI2C_Write(nID, 0x400, (PBYTE)&page, 2);	// write page
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHOST_I2CWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(MDIN_HOST_ID, 0x0000);	// write host page
	if (err) return err;

	err = MDINI2C_Write(MDIN_HOST_ID, rAddr, pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHOST_I2CRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(MDIN_HOST_ID, 0x0000);	// write host page
	if (err) return err;

	err = MDINI2C_Read(MDIN_HOST_ID, rAddr, pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE LOCAL_I2CWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(MDIN_LOCAL_ID, 0x0101);	// write local page
	if (err) return err;

	err = MDINI2C_Write(MDIN_LOCAL_ID, rAddr, pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE LOCAL_I2CRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	WORD RegOEN, err = I2C_OK;

	if		(rAddr>=0x030&&rAddr<0x036)	RegOEN = 0x04;	// mfc-size
	else if (rAddr>=0x043&&rAddr<0x045)	RegOEN = 0x09;	// out-ptrn
	else if (rAddr>=0x062&&rAddr<0x083)	RegOEN = 0x09;	// enhance
	else if (rAddr>=0x088&&rAddr<0x092)	RegOEN = 0x09;	// out-sync
	else if (rAddr>=0x094&&rAddr<0x097)	RegOEN = 0x09;	// out-sync
	else if (rAddr>=0x09a&&rAddr<0x09c)	RegOEN = 0x09;	// bg-color
	else if (rAddr>=0x0a0&&rAddr<0x0d0)	RegOEN = 0x09;	// out-ctrl
	else if (              rAddr<0x100)	RegOEN = 0x01;	// in-sync
	else if (rAddr>=0x100&&rAddr<0x140)	RegOEN = 0x05;	// main-fc
	else if (rAddr>=0x140&&rAddr<0x1a0)	RegOEN = 0x07;	// aux
	else if (rAddr>=0x1a0&&rAddr<0x1c0)	RegOEN = 0x03;	// arbiter
	else if (rAddr>=0x1c0&&rAddr<0x1e0)	RegOEN = 0x02;	// fc-mc
	else if (rAddr>=0x1e0&&rAddr<0x1f8)	RegOEN = 0x08;	// encoder
	else if (rAddr>=0x1f8&&rAddr<0x200)	RegOEN = 0x0a;	// audio
	else if (rAddr>=0x200&&rAddr<0x280)	RegOEN = 0x04;	// ipc
	else if (rAddr>=0x2a0&&rAddr<0x300)	RegOEN = 0x07;	// aux-osd
	else if (rAddr>=0x300&&rAddr<0x380)	RegOEN = 0x06;	// osd
	else if (rAddr>=0x3c0&&rAddr<0x3f8)	RegOEN = 0x09;	// enhance
	else								RegOEN = 0x00;	// host state

	err = LOCAL_I2CWrite(0x3ff, (PBYTE)&RegOEN, 2);	// write reg_oen
	if (err) return err;


	err = MDINI2C_Read(MDIN_LOCAL_ID, rAddr, pBuff, bytes);

	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHDMI_I2CWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	err = MDINI2C_SetPage(MDIN_HDMI_ID, 0x0202);	// write hdmi page
	if (err) return err;

	err = MDINI2C_Write(MDIN_HDMI_ID, rAddr/2, (PBYTE)pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHDMI_GetWriteDone(void)
{
	WORD rVal = 0, count = 100, err = I2C_OK;

	while (count&&(rVal==0)) {
		err = MDINI2C_Read(MDIN_HDMI_ID, 0x027, (PBYTE)&rVal, 2);
		if (err) return err;	rVal &= 0x04;	count--;
	}
	return (count)? I2C_OK : I2C_TIME_OUT;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHDMI_HOSTRead(WORD rAddr, PBYTE pBuff)
{
	WORD rData, err = I2C_OK;

	err = MDINI2C_SetPage(MDIN_HOST_ID, 0x0000);	// write host page
	if (err) return err;

	err = MDINI2C_Write(MDIN_HOST_ID, 0x025, (PBYTE)&rAddr, 2);
	if (err) return err;	rData = 0x0003;
	err = MDINI2C_Write(MDIN_HOST_ID, 0x027, (PBYTE)&rData, 2);
	if (err) return err;	rData = 0x0002;
	err = MDINI2C_Write(MDIN_HOST_ID, 0x027, (PBYTE)&rData, 2);
	if (err) return err;

	// check done flag
	err = MHDMI_GetWriteDone(); if (err) {mdinERR = 4; return err;}
	
	err = MDINI2C_Read(MDIN_HOST_ID, 0x026, (PBYTE)pBuff, 2);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MHDMI_I2CRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE err = I2C_OK;

	// DDC_STATUS, DDC_FIFOCNT
	if (rAddr==0x0f2||rAddr==0x0f5) return MHDMI_HOSTRead(rAddr, pBuff);

	err = MDINI2C_SetPage(MDIN_HDMI_ID, 0x0202);	// write hdmi page
	if (err) return err;

	err = MDINI2C_Read(MDIN_HDMI_ID, rAddr/2, (PBYTE)pBuff, bytes);
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE SDRAM_I2CWrite(DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	WORD row, len, idx, unit, err = I2C_OK;

	err = MDINI2C_RegRead(MDIN_HOST_ID, 0x005, &unit);	if (err) return err;
	unit = (unit&0x0100)? 4096 : 2048;

	while (bytes>0) {
		row = ADDR2ROW(rAddr, unit);	// get row
		idx = ADDR2COL(rAddr, unit);	// get col
		len = MIN((unit/2)-(rAddr%(unit/2)), bytes);

		err = MDINI2C_RegWrite(MDIN_HOST_ID, 0x003, row); if (err) return err;	// host access
		err = MDINI2C_SetPage(MDIN_HOST_ID, 0x0303); if (err) return err;	// write sdram page
		err = MDINI2C_Write(MDIN_SDRAM_ID, idx/2, (PBYTE)pBuff, len); if (err) return err;
		bytes-=len; rAddr+=len; pBuff+=len;
	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE SDRAM_I2CRead(DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	WORD row, len, idx, unit, err = I2C_OK;

	err = MDINI2C_RegRead(MDIN_HOST_ID, 0x005, &unit);	if (err) return err;
	unit = (unit&0x0100)? 4096 : 2048;

	while (bytes>0) {
		row = ADDR2ROW(rAddr, unit);	// get row
		idx = ADDR2COL(rAddr, unit);	// get col
		len = MIN((unit/2)-(rAddr%(unit/2)), bytes);

		err = MDINI2C_RegWrite(MDIN_HOST_ID, 0x003, row); if (err) return err;	// host access
		err = MDINI2C_SetPage(MDIN_HOST_ID, 0x0303); if (err) return err;	// write sdram page
		err = MDINI2C_Read(MDIN_SDRAM_ID, idx/2, (PBYTE)pBuff, len); if (err) return err;
		bytes-=len; rAddr+=len; pBuff+=len;
	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_WriteByID(BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	BYTE err = I2C_OK;

	switch (nID&0xfe) {
		case MDIN_HOST_ID:	err = MHOST_I2CWrite(rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_LOCAL_ID:	err = LOCAL_I2CWrite(rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_SDRAM_ID:	err = SDRAM_I2CWrite(rAddr, (PBYTE)pBuff, bytes); break;

	#if defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN380)
		case MDIN_HDMI_ID:	err = MHDMI_I2CWrite(rAddr, (PBYTE)pBuff, bytes); break;
	#endif

	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_ReadByID(BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	BYTE err = I2C_OK;

	switch (nID&0xfe) {
		case MDIN_HOST_ID:	err = MHOST_I2CRead(rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_LOCAL_ID:	err = LOCAL_I2CRead(rAddr, (PBYTE)pBuff, bytes); break;
		case MDIN_SDRAM_ID:	err = SDRAM_I2CRead(rAddr, (PBYTE)pBuff, bytes); break;

	#if defined(SYSTEM_USE_MDIN340)||defined(SYSTEM_USE_MDIN380)
		case MDIN_HDMI_ID:	err = MHDMI_I2CRead(rAddr, (PBYTE)pBuff, bytes); break;
	#endif

	}
	return err;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_SetPageID(WORD nID)
{
	PageID = nID;
	return MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_MultiWrite(BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	return (I2C_WriteByID(nID, rAddr, (PBYTE)pBuff, bytes))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_RegWrite(BYTE nID, DWORD rAddr, WORD wData)
{
	return (MDINI2C_MultiWrite(nID, rAddr, (PBYTE)&wData, 2))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_MultiRead(BYTE nID, DWORD rAddr, PBYTE pBuff, DWORD bytes)
{
	return (I2C_ReadByID(nID, rAddr, (PBYTE)pBuff, bytes))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_RegRead(BYTE nID, DWORD rAddr, PWORD rData)
{
	return (MDINI2C_MultiRead(nID, rAddr, (PBYTE)rData, 2))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
MDIN_ERROR_t MDINI2C_RegField(BYTE nID, DWORD rAddr, WORD bPos, WORD bCnt, WORD bData)
{
	WORD temp;

	if (bPos>15||bCnt==0||bCnt>16||(bPos+bCnt)>16) return MDIN_INVALID_PARAM;
	if (MDINI2C_RegRead(nID, rAddr, &temp)) return MDIN_I2C_ERROR;
	bCnt = ~(0xffff<<bCnt);
	temp &= ~(bCnt<<bPos);
	temp |= ((bData&bCnt)<<bPos);
	return (MDINI2C_RegWrite(nID, rAddr, temp))? MDIN_I2C_ERROR : MDIN_NO_ERROR;
}

//--------------------------------------------------------------------------------------------------------------------------
// Drive Function for I2C read & I2C write
// You must make functions which is defined below.
//--------------------------------------------------------------------------------------------------------------------------
static BYTE MDINI2C_Read(BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	I2C_Read16(I2C_1, I2C_MDIN3xx_ADDR, rAddr, pBuff, bytes);
	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE MDINI2C_Write(BYTE nID, WORD rAddr, PBYTE pBuff, WORD bytes)
{
	I2C_Write16(I2C_1, I2C_MDIN3xx_ADDR, rAddr, pBuff, bytes);
	return I2C_OK;
}


/*  FILE_END_HERE */
