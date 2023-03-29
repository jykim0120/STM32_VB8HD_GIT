//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2005  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:	I2C.C
// Description 		:
// Ref. Docment		: 
// Revision History 	:

// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	"..\video\video.h"
#include    "i2c.h"

// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------




// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------


#define		TIMEOUT_MAX			0x1000
#define I2C2_SCL	GPIO_Pin_10
#define I2C2_SDA	GPIO_Pin_11

#define I2C1_SCL	GPIO_Pin_6
#define I2C1_SDA	GPIO_Pin_7

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
BYTE I2C_ID;	//by hungry 2013.04.25 test

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------
//------------------GPIO I2C-----------------
void I2CStart_1(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = I2C1_SDA | I2C1_SCL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_SetBits(GPIOB, I2C1_SDA);
	I2CDLY_5uSec(1);
	GPIO_SetBits(GPIOB, I2C1_SCL);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C1_SDA);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C1_SCL);
}

void I2CStop_1(void)
{
	GPIO_ResetBits(GPIOB, I2C1_SDA);
	I2CDLY_5uSec(1);
	GPIO_SetBits(GPIOB, I2C1_SCL);
	I2CDLY_5uSec(1);
	GPIO_SetBits(GPIOB, I2C1_SDA);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C1_SCL);

}

void I2CSlaveAck_1(BYTE flag)
{
	if(flag == ACK)
		GPIO_ResetBits(GPIOB, I2C1_SDA);
	else
		GPIO_SetBits(GPIOB, I2C1_SDA);
	
	I2CDLY_5uSec(1);
	GPIO_SetBits(GPIOB, I2C1_SCL);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C1_SCL);
}		


void I2CWriteByte_1(BYTE byte)
{
	BYTE i, tmpbyte = byte;

	for(i = 0; i < 8; i++)
	{
		if(0x80 & tmpbyte)
			GPIO_SetBits(GPIOB, I2C1_SDA);
		else
			GPIO_ResetBits(GPIOB, I2C1_SDA);
		
		MDINDLY_5uSec(1);
		GPIO_SetBits(GPIOB, I2C1_SCL);
		I2CDLY_5uSec(1);
		GPIO_ResetBits(GPIOB, I2C1_SCL);
		tmpbyte = tmpbyte << 1;
	}
}

BYTE I2CReadByte_1(void)
{
	BYTE i;
	BYTE ReadValue = 0;
	GPIO_InitTypeDef GPIO_InitStruct;
	BYTE bit;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin  = I2C1_SDA;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	for(i = 0; i < 8; i++)
	{
		if(Bit_SET == GPIO_ReadInputDataBit(GPIOB, I2C1_SDA))	bit = 0x01;
		else													bit = 0x00;

		I2CDLY_5uSec(1);			
		GPIO_SetBits(GPIOB, I2C1_SCL);
		I2CDLY_5uSec(1);	
		GPIO_ResetBits(GPIOB, I2C1_SCL);		
		I2CDLY_5uSec(1);
		ReadValue = (ReadValue<<1)|bit;
	}
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = I2C1_SDA|I2C1_SCL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	return ReadValue;
}


//------------------GPIO I2C-----------------
void I2CStart(void)
{
	GPIO_InitTypeDef	GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = I2C2_SDA | I2C2_SCL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_SetBits(GPIOB, I2C2_SDA);
	GPIO_SetBits(GPIOB, I2C2_SCL);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C2_SDA);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C2_SCL);
}

void I2CStop(void)
{
	GPIO_ResetBits(GPIOB, I2C2_SDA);
	I2CDLY_5uSec(1);
	GPIO_SetBits(GPIOB, I2C2_SCL);
	I2CDLY_5uSec(1);
	GPIO_SetBits(GPIOB, I2C2_SDA);
}

void I2CSlaveAck(void)
{
	GPIO_SetBits(GPIOB, I2C2_SCL);
	I2CDLY_5uSec(1);
	GPIO_ResetBits(GPIOB, I2C2_SCL);
}		

void I2CWriteByte(BYTE byte)
{
	BYTE i, tmpbyte = byte;

	for(i = 0; i < 8; i++)
	{
		if(0x80 & tmpbyte)
			GPIO_SetBits(GPIOB, I2C2_SDA);
		else
			GPIO_ResetBits(GPIOB, I2C2_SDA);
		
		I2CDLY_5uSec(1);
		GPIO_SetBits(GPIOB, I2C2_SCL);
		I2CDLY_5uSec(1);
		GPIO_ResetBits(GPIOB, I2C2_SCL);
		tmpbyte = tmpbyte << 1;
	}
}

BYTE I2CReadByte(void)
{
	BYTE i, bit, ReadValue = 0;
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin  = I2C2_SDA;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	for(i = 0; i < 8; i++)
	{

		if(Bit_SET == GPIO_ReadInputDataBit(GPIOB, I2C2_SDA))	bit = 0x01;
		else													bit = 0x00;

		I2CDLY_5uSec(1);
		GPIO_SetBits(GPIOB, I2C2_SCL);
		I2CDLY_5uSec(1);		
		GPIO_ResetBits(GPIOB, I2C2_SCL);		
		I2CDLY_5uSec(1);
		ReadValue = (ReadValue<<1)|bit;
	}
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = I2C2_SDA | I2C2_SCL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	return ReadValue;
}


//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_Check(uint32_t I2C_Event)
{
	WORD TimeOut = TIMEOUT_MAX;
	BYTE return_flag = I2C_OK;

	while(!I2C_CheckEvent((I2C_ID == I2C_1)? I2C1 : I2C2, I2C_Event))
	{
		if((TimeOut--) == 0)
		{
			switch (I2C_Event)
			{
				case I2C_EVENT_MASTER_MODE_SELECT :
					printf("Error : I2C cannot start\n\r");
					return_flag = I2C_NOT_START;
					break;

				case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED :
					printf("Error : I2C No Ack\n\r");
					return_flag = I2C_HOST_NACK;
					break;

				case I2C_EVENT_MASTER_BYTE_TRANSMITTED :
					printf("Error : I2C data transfer not finish\n\r");
					return_flag = I2C_HOST_NACK;
					break;

				case I2C_EVENT_MASTER_BYTE_RECEIVED :				//by hungry 2013.04.25
					printf("Error : I2C data received time out\n\r");
					return_flag = I2C_HOST_NACK;
					break;

				case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED :		//by hungry 2013.04.25
					printf("Error : I2C receiver mode selected time out\n\r");
					return_flag = I2C_HOST_NACK;
					break;
			}
			I2C_GenerateSTOP((I2C_ID == I2C_1)? I2C1 : I2C2, ENABLE);
		}
	}

	return return_flag;
}

//--------------------------------------------------------------------------------------------------------------------------
static void I2C_CheckBusy(void)
{
	while (I2C_GetFlagStatus((I2C_ID == I2C_1)?I2C1 : I2C2, I2C_FLAG_BUSY));
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_Start(void)
{
	I2C_GenerateSTART((I2C_ID == I2C_1)?I2C1 : I2C2, ENABLE);
	if (I2C_Check(I2C_EVENT_MASTER_MODE_SELECT)) return I2C_NOT_START;

	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_Stop(void)
{
	I2C_GenerateSTOP((I2C_ID == I2C_1)?I2C1 : I2C2, ENABLE);
	return I2C_OK;
}

/*	//by hungry 2013.04.25
//--------------------------------------------------------------------------------------------------------------------------
static void I2C_Clear(void)
{
	if (I2C_ID == I2C_1) {(void)I2C1->SR1;	(void)I2C1->SR2;}
	else {(void)I2C2->SR1;	(void)I2C2->SR2;}
}
*/

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_SendAddr(BYTE dAddr, BYTE mode)
{
	// mode = 0 for transmitter, 1 for receiver

	if (mode == I2C_Direction_Transmitter)
	{
		I2C_Send7bitAddress((I2C_ID == I2C_1)?I2C1 : I2C2, dAddr, mode);
		if (I2C_Check(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			printf("dAddr = %02X\n\r", dAddr);
			while (1);
		}
	}
	else
	{
		I2C_Send7bitAddress((I2C_ID == I2C_1)?I2C1 : I2C2, dAddr, I2C_Direction_Receiver);
		if (I2C_Check(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
					printf("dAddr = %02X\n\r", dAddr);
					while (1); 
		}
	}

	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_SendByte(BYTE Data, BOOL mode)
{
	if (mode == NACK) I2C_AcknowledgeConfig((I2C_ID == I2C_1)?I2C1 : I2C2, DISABLE);

	I2C_SendData((I2C_ID == I2C_1)?I2C1 : I2C2, Data);
	if (I2C_Check(I2C_EVENT_MASTER_BYTE_TRANSMITTED)) return I2C_HOST_NACK;

	if (mode == NACK) I2C_AcknowledgeConfig((I2C_ID == I2C_1)?I2C1 : I2C2, ENABLE);
	return I2C_OK;
}

//--------------------------------------------------------------------------------------------------------------------------
static BYTE I2C_GetByte(BOOL mode)
{
	BYTE read;

	if (mode == NACK) I2C_AcknowledgeConfig((I2C_ID == I2C_1)?I2C1 : I2C2, DISABLE);
	if (mode == NACK) I2C_Stop();	//STOP generation before read last byte //by hungry 2013.04.25

//h	while(I2C_GetFlagStatus((I2C_ID == I2C_1)?I2C1 : I2C2, I2C_FLAG_RXNE) == RESET);	//by hungry 2013.04.25 test
	if(I2C_Check(I2C_EVENT_MASTER_BYTE_RECEIVED))	return I2C_HOST_NACK;	//by hungry 2013.04.25 test

	read = I2C_ReceiveData((I2C_ID == I2C_1)?I2C1 : I2C2);

	if (mode == NACK) I2C_AcknowledgeConfig((I2C_ID == I2C_1)?I2C1 : I2C2, ENABLE);

	return read;
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C_Write8(BYTE ID, BYTE dAddr, BYTE rAddr, PBYTE pBuff, WORD bytes)
{
#if 0
	WORD i;

	I2C_ID = ID;

	I2C_Cmd((I2C_ID == I2C_1)?I2C1 : I2C2, DISABLE);
	I2C_Cmd((I2C_ID == I2C_1)?I2C1 : I2C2, ENABLE);
	
	I2C_CheckBusy();
	if (I2C_Start()) return I2C_NOT_START;
	if (I2C_SendAddr(dAddr, I2C_Direction_Transmitter)) return I2C_HOST_NACK;
	if (I2C_SendByte(LOBYTE(rAddr), ACK)) return I2C_HOST_NACK;
	for (i=0; i<bytes-1; i++)
	{
		if (I2C_SendByte(pBuff[i], ACK)) return I2C_HOST_NACK;
	}
	if (I2C_SendByte(pBuff[i], NACK)) return I2C_HOST_NACK;
	I2C_Stop();


	return I2C_OK;
#else
	WORD i;

	I2C_ID = ID;

	I2CStart();
	I2CWriteByte(dAddr);               //slave address
	I2CSlaveAck();
	I2CDLY_5uSec(1);			//2		[ JYKim 		2015-4-29 ]
	
	I2CWriteByte( LOBYTE(rAddr) );     //start address
	I2CSlaveAck();
	I2CDLY_5uSec(1);			//2		[ JYKim 		2015-4-29 ]

	for (i=0; i<bytes; i++)
	{
		I2CWriteByte((BYTE)pBuff[i]);
		I2CSlaveAck();
		I2CDLY_5uSec(1);			//2		[ JYKim 		2015-4-29 ]
	}
	
	I2CStop();

	return I2C_OK;
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C_Read8(BYTE ID, BYTE dAddr, BYTE rAddr, PBYTE pBuff, WORD bytes)
{
#if 0
	WORD i;

	I2C_ID = ID;

	I2C_Cmd((I2C_ID == I2C_1)?I2C1 : I2C2, DISABLE);
	I2C_Cmd((I2C_ID == I2C_1)?I2C1 : I2C2, ENABLE);
	
	I2C_CheckBusy();

	if (I2C_Start()) return I2C_NOT_START;
	if (I2C_SendAddr(dAddr, I2C_Direction_Transmitter)) return I2C_HOST_NACK;
	if (I2C_SendByte(LOBYTE(rAddr), ACK)) return I2C_HOST_NACK;

	if (I2C_Start()) return I2C_NOT_START;
	if (I2C_SendAddr(dAddr, I2C_Direction_Receiver)) return I2C_HOST_NACK;

	for (i=0; i<bytes-1; i++)	pBuff[i] = I2C_GetByte(ACK);

	pBuff[i] = I2C_GetByte(NACK);
//	I2C_Stop();		//by hungry 2013.04.25
	return I2C_OK;
#else
	WORD i;
	
	I2C_ID = ID;

	I2CStart();
	I2CWriteByte(dAddr);
	I2CSlaveAck();
	I2CWriteByte( LOBYTE(rAddr) );
	I2CSlaveAck();
	
	I2CStart();
	I2CWriteByte(dAddr | 0x01);
	I2CSlaveAck();
	for (i=0; i<bytes; i++)
	{
		pBuff[i] = I2CReadByte();
		I2CSlaveAck();
	}
	I2CStop();

	return I2C_OK;
	
#endif
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C_Write16(BYTE ID, BYTE dAddr, WORD rAddr, PBYTE pBuff, WORD bytes)
{
#if 0
	WORD i;

	I2C_ID = ID;
	I2C_CheckBusy();

	if (I2C_Start()) return I2C_NOT_START;
	if (I2C_SendAddr(dAddr, I2C_Direction_Transmitter)) return I2C_HOST_NACK;
	if (I2C_SendByte(HIBYTE(rAddr), ACK)) return I2C_HOST_NACK;
	if (I2C_SendByte(LOBYTE(rAddr), ACK)) return I2C_HOST_NACK;

	for (i=0; i<bytes-1; i++)
		if (I2C_SendByte(pBuff[i^1], ACK)) return I2C_HOST_NACK;

	if (I2C_SendByte(pBuff[i^1], NACK)) return I2C_HOST_NACK;
	I2C_Stop();
	return I2C_OK;
#else
	WORD i;

	I2C_ID = ID;

	I2CStart_1();
	I2CWriteByte_1(dAddr);               //slave address
	I2CSlaveAck_1(ACK);
	
	I2CWriteByte_1( HIBYTE(rAddr) );     //start address
	I2CSlaveAck_1(ACK);
	I2CWriteByte_1( LOBYTE(rAddr) );     //start address
	I2CSlaveAck_1(ACK);

	for (i=0; i<bytes-1; i++)
	{
		I2CWriteByte_1(pBuff[i^1]);
		I2CSlaveAck_1(ACK);
	}
	I2CWriteByte_1(pBuff[i^1]);
	I2CSlaveAck_1(NACK);

	I2CStop_1();

	return I2C_OK;


#endif
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C_Read16(BYTE ID, BYTE dAddr, WORD rAddr, PBYTE pBuff, WORD bytes)
{
#if 0
	WORD i;

	I2C_ID = ID;
	I2C_CheckBusy();

	if (I2C_Start()) return I2C_NOT_START;
	if (I2C_SendAddr(dAddr, I2C_Direction_Transmitter)) return I2C_HOST_NACK;
	if (I2C_SendByte(HIBYTE(rAddr), ACK)) return I2C_HOST_NACK;
	if (I2C_SendByte(LOBYTE(rAddr), ACK)) return I2C_HOST_NACK;

	if (I2C_Start()) return I2C_NOT_START;
	if (I2C_SendAddr(dAddr, I2C_Direction_Receiver)) return I2C_HOST_NACK;
	//__disable_irq();  I2C_Clear();	__enable_irq();	//by hungry 2013.04.25

	for (i=0; i<bytes-1; i++)
		pBuff[i^0x01]  = I2C_GetByte(ACK);		// Receive a buffer data

	pBuff[i^0x01]  = I2C_GetByte(NACK);		// Receive a buffer data
	I2C_Stop();			//by hungry 2013.04.25
	return I2C_OK;
#else
	WORD i;

	I2C_ID = ID;

	I2CStart_1();
	I2CWriteByte_1(dAddr);
	I2CSlaveAck_1(ACK);
	
	I2CWriteByte_1( HIBYTE(rAddr));
	I2CSlaveAck_1(ACK);
	I2CWriteByte_1( LOBYTE(rAddr) );
	I2CSlaveAck_1(ACK);

	I2CStart_1();
	I2CWriteByte_1(dAddr | 0x01);
	I2CSlaveAck_1(ACK);

	for (i=0; i<bytes-1; i++)
	{
		pBuff[i^0x01] = I2CReadByte_1();
		I2CSlaveAck_1(ACK);
	}
	pBuff[i^0x01] = I2CReadByte_1();
	I2CSlaveAck_1(NACK);
	
	I2CStop_1();
	
	return I2C_OK;

#endif
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C_Read(BYTE sAddr, WORD rAddr, PBYTE pBuff, WORD bytes)	//by hungry 2013.04.25
{
	return I2C_Read8(I2C_1, sAddr, rAddr, pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C_Write(BYTE sAddr, WORD rAddr, PBYTE pBuff, WORD bytes)	//by hungry 2013.04.25
{
	return I2C_Write8(I2C_1, sAddr, rAddr, pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C2_Read(BYTE sAddr, WORD rAddr, PBYTE pBuff, WORD bytes)	//by hungry 2013.04.25
{
	return I2C_Read8(I2C_2, sAddr, rAddr, pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE I2C2_Write(BYTE sAddr, WORD rAddr, PBYTE pBuff, WORD bytes)	//by hungry 2013.04.25
{
	return I2C_Write8(I2C_2, sAddr, rAddr, pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE NVP1918C_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	return I2C2_Write(I2C_DEC_ADDR, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE NVP1918C_RegWrite(WORD rAddr, BYTE wData)
{
	return NVP1918C_MultiWrite(rAddr, (PBYTE)&wData, 1);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE NVP1918C_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	return I2C2_Read(I2C_DEC_ADDR, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE NVP1918C_RegRead(WORD rAddr)
{
	BYTE rData = 0;
	NVP1918C_MultiRead(rAddr, (PBYTE)&rData, 1);
	return rData;
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE AHD_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	return I2C2_Write(I2C_DEC_ADDR, rAddr, (PBYTE)pBuff, bytes);
//	return I2C2_Write(NVP6114A_ADDR, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE AHD_RegWrite(WORD rAddr, BYTE wData)
{
	return AHD_MultiWrite(rAddr, (PBYTE)&wData, 1);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE AHD_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	return I2C2_Read(I2C_DEC_ADDR, rAddr, (PBYTE)pBuff, bytes);
//	return I2C2_Read(NVP6114A_ADDR, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE AHD_RegRead(WORD rAddr)
{
	BYTE rData = 0;
	AHD_MultiRead(rAddr, (PBYTE)&rData, 1);
	return rData;
}

//--------------------------------------------------------------------------------------------------------------------------

BYTE TW9960_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	return I2C2_Read(I2C_TW9960_ADDR, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------

BYTE TW9960_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	return I2C2_Write(I2C_TW9960_ADDR, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------

BYTE TW9960_RegRead(WORD rAddr)
{
	BYTE rData = 0;
	TW9960_MultiRead(rAddr, (PBYTE)&rData, 1);
	return rData;
}

//--------------------------------------------------------------------------------------------------------------------------

BYTE TW9960_RegWrite(WORD rAddr, BYTE wData)
{
	return TW9960_MultiWrite(rAddr, (PBYTE)&wData, 1);
}



//--------------------------------------------------------------------------------------------------------------------------
static BYTE HDRX_GetSlaveAddr(WORD rAddr)
{
	return 0x90;
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE HDRX_MultiWrite(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE sAddr = HDRX_GetSlaveAddr(rAddr);

	return I2C_Write(sAddr, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE HDRX_RegWrite(WORD rAddr, BYTE wData)
{
	return HDRX_MultiWrite(rAddr, (PBYTE)&wData, 1);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE HDRX_MultiRead(WORD rAddr, PBYTE pBuff, WORD bytes)
{
	BYTE sAddr = HDRX_GetSlaveAddr(rAddr);

	return I2C_Read(sAddr, rAddr, (PBYTE)pBuff, bytes);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE HDRX_RegRead(WORD rAddr)
{
	BYTE rData = 0;

	HDRX_MultiRead(rAddr, (PBYTE)&rData, 1);
	return rData;
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE HDRX_RegBitValue(WORD rAddr, BYTE mask, BYTE value)
{
	BYTE temp = HDRX_RegRead(rAddr);
	temp &= (~mask);			// first clear all bits in mask
	temp |= (mask & value);		// then set bits from value
	return HDRX_RegWrite(rAddr, temp);
}

//--------------------------------------------------------------------------------------------------------------------------
BYTE HDRX_RegBitToggle(WORD rAddr, BYTE mask)
{
	BYTE err, temp = HDRX_RegRead(rAddr);

	temp |=  mask;						// first set the bits in mask
	err = HDRX_RegWrite(rAddr, temp);	// write register with bits set
	if (err) return err;

	temp &= ~mask;						// then clear the bits in mask
	return HDRX_RegWrite(rAddr, temp);	// write register with bits clear
}




/*  FILE_END_HERE */
