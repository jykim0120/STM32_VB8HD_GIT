/**************************************************************************
	Header file include
**************************************************************************/
#include "common.h"
#include "i2c.h"
#include "coaxial.h"
/*************************************************************************
	SAMSUNG PROTOCOL Regiter set value Definition
*************************************************************************/
								 //  0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0A 0x0B 0x0C 0x0D 0x0E 0x0F
static ROMDATA BYTE SAM_SET_Buf[] =		 	{0xAA,0x1C,0x18,0xFF,0xAA,0x3C,0xFF,0xFF };
static ROMDATA BYTE SAM_OSD_Buf[] =		 	{0xAA,0x1C,0x17,0x01,0xAA,0x3C,0xFF,0xFF };
static ROMDATA BYTE SAM_UP_Buf[] =		 	{0xAA,0x1C,0x03,0xFF,0xAA,0x3C,0xFF,0xFF };
static ROMDATA BYTE SAM_DOWN_Buf[] =		{0xAA,0x1C,0x09,0xFF,0xAA,0x3C,0xFF,0xFF };
static ROMDATA BYTE SAM_LEFT_Buf[] = 	 	{0xAA,0x1C,0x05,0xFF,0xAA,0x3C,0xFF,0xFF };
static ROMDATA BYTE SAM_RIGHT_Buf[] = 	 	{0xAA,0x1C,0x07,0xFF,0xAA,0x3C,0xFF,0xFF };
static ROMDATA BYTE SAM_PTZ_UP_Buf[] = 	 	{0xAA,0x1B,0x00,0x04,0xAA,0x3B,0x00,0x1F };
static ROMDATA BYTE SAM_PTZ_DOWN_Buf[] =  	{0xAA,0x1B,0x00,0x08,0xAA,0x3B,0x00,0x1F };
static ROMDATA BYTE SAM_PTZ_LEFT_Buf[] =  	{0xAA,0x1B,0x00,0x01,0xAA,0x3B,0x1F,0x00 };
static ROMDATA BYTE SAM_PTZ_RIGHT_Buf[] = 	{0xAA,0x1B,0x00,0x02,0xAA,0x3B,0x1F,0x00 };
static ROMDATA BYTE SAM_IRIS_OPEN_Buf[] = 	{0xAA,0x1B,0x08,0x00,0xAA,0x3B,0x00,0x00 };
static ROMDATA BYTE SAM_IRIS_CLOSE_Buf[]= 	{0xAA,0x1B,0x10,0x00,0xAA,0x3B,0x00,0x00 };
static ROMDATA BYTE SAM_FOCUS_NEAR_Buf[]= 	{0xAA,0x1B,0x02,0x00,0xAA,0x3B,0x00,0x00 };
static ROMDATA BYTE SAM_FOCUS_FAR_Buf[] = 	{0xAA,0x1B,0x01,0x00,0xAA,0x3B,0x00,0x00 };
static ROMDATA BYTE SAM_ZOOM_WIDE_Buf[] = 	{0xAA,0x1B,0x40,0x00,0xAA,0x3B,0x00,0x00 };
static ROMDATA BYTE SAM_ZOOM_TELE_Buf[] = 	{0xAA,0x1B,0x20,0x00,0xAA,0x3B,0x00,0x00 };
static ROMDATA BYTE SAM_SCAN_SR_Buf[] =	 	{0xAA,0x1C,0x13,0x01,0xAA,0x3C,0x01,0x00 };
static ROMDATA BYTE SAM_SCAN_ST_Buf[] =	 	{0xAA,0x1C,0x13,0x00,0xAA,0x3C,0x01,0x00 };
static ROMDATA BYTE SAM_PRESET1_Buf[] =	 	{0xAA,0x1C,0x19,0x01,0xAA,0x3C,0x00,0x00 };
static ROMDATA BYTE SAM_PRESET2_Buf[] =	 	{0xAA,0x1C,0x19,0x02,0xAA,0x3C,0x00,0x00 };
static ROMDATA BYTE SAM_PRESET3_Buf[] =	 	{0xAA,0x1C,0x19,0x03,0xAA,0x3C,0x00,0x00 };
static ROMDATA BYTE SAM_PATTERN_1SR_Buf[] =	{0xAA,0x1C,0x1B,0x01,0xAA,0x3C,0x01,0x00 };
static ROMDATA BYTE SAM_PATTERN_1ST_Buf[] =	{0xAA,0x1C,0x1B,0x01,0xAA,0x3C,0x00,0x00 };
static ROMDATA BYTE SAM_PATTERN_2SR_Buf[] =	{0xAA,0x1C,0x1B,0x02,0xAA,0x3C,0x01,0x00 };
static ROMDATA BYTE SAM_PATTERN_2ST_Buf[] =	{0xAA,0x1C,0x1B,0x02,0xAA,0x3C,0x00,0x00 };
static ROMDATA BYTE SAM_PATTERN_3SR_Buf[] =	{0xAA,0x1C,0x1B,0x03,0xAA,0x3C,0x01,0x00 };
static ROMDATA BYTE SAM_PATTERN_3ST_Buf[] =	{0xAA,0x1C,0x1B,0x03,0xAA,0x3C,0x00,0x00 };
static ROMDATA BYTE SAM_2ND_CMD_Buf[]     =	{0xAA,0x1B,0x00,0x00,0xAA,0x3B,0x00,0x00 };

/*************************************************************************
	Global Variables Definition
*************************************************************************/

/**************************************************************************
	Function prototype
**************************************************************************/

/*************************************************************************
	Initial NVP1104A
**************************************************************************/

void pelco_coax_mode( void )
{
	NVP1918C_RegWrite( 0xFF, 1);
	NVP1918C_RegWrite( 0xBC, 0xDD);
	NVP1918C_RegWrite( 0xBD, 0xDD);

	#if defined(__NVP1918C_RX__)

		NVP1918C_RegWrite( 0xFF, 1);
		NVP1918C_RegWrite( COAX_BAUD, 0x37);
		NVP1918C_RegWrite( BL_HSP01, 0xD0);
		NVP1918C_RegWrite( BL_HSP02, 0x05);
		NVP1918C_RegWrite( PACKET_MODE, 0x06);
		NVP1918C_RegWrite( PEL_CTEN, 0x00);
		NVP1918C_RegWrite( PEL_TXST1, 0x09);
		NVP1918C_RegWrite( EVEN_LINE, 0x01);

	#else

		NVP1918C_RegWrite( 0xFF, 3);
		if(AHD_RegRead(0x5C)==0x60) // AHD 30p
		{
			NVP1918C_RegWrite( 0xFF, 4);
			//NVP1918C_RegWrite( COAX_BAUD, 0x37);
			NVP1918C_RegWrite( PEL_BAUD, 0x11);
			NVP1918C_RegWrite( BL_HSP01, 0x60);
			NVP1918C_RegWrite( BL_HSP02, 0x05);
			NVP1918C_RegWrite( PACKET_MODE, 0x06);
			NVP1918C_RegWrite( PEL_CTEN, 0x00);
			//NVP1918C_RegWrite( PEL_TXST1, 0x14);
			NVP1918C_RegWrite( PEL_TXST1, 0x0A);	// NEW AHD coaxial protocol TEST
			NVP1918C_RegWrite( EVEN_LINE, 0x00);
		}
		else						// AHD 25p
		{
			NVP1918C_RegWrite( 0xFF, 4);
			//NVP1918C_RegWrite( COAX_BAUD, 0x37);
			NVP1918C_RegWrite( PEL_BAUD, 0x11);
			NVP1918C_RegWrite( BL_HSP01, 0xF0);
			NVP1918C_RegWrite( BL_HSP02, 0x06);
			NVP1918C_RegWrite( PACKET_MODE, 0x06);
			NVP1918C_RegWrite( PEL_CTEN, 0x00);
			NVP1918C_RegWrite( PEL_TXST1, 0x15);
			NVP1918C_RegWrite( EVEN_LINE, 0x00);
		}
	#endif

}

BYTE pelco_command(BYTE command)
{
	BYTE str[4];

	MDINDLY_mSec(20);

	switch(command)
	{
		case  PELCO_CMD_RESET :
			str[0] = 0x00;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_SET:
			str[0] = 0x40;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_UP:
			str[0] = 0x00;str[1] = 0x10;str[2] = 0x00;str[3] = 0x4C;
		break;
		case  PELCO_CMD_DOWN:
			str[0] = 0x00;str[1] = 0x08;str[2] = 0x00;str[3] = 0x4C;
		break;
		case  PELCO_CMD_LEFT:
			str[0] = 0x00;str[1] = 0x20;str[2] = 0x4C;str[3] = 0x00;
		break;
		case  PELCO_CMD_RIGHT:
			str[0] = 0x00;str[1] = 0x40;str[2] = 0x4C;str[3] = 0x00;
		break;
		case  PELCO_CMD_OSD:
			str[0] = 0x00;str[1] = 0xC0;str[2] = 0x00;str[3] = 0xFA;
		break;
		case  PELCO_CMD_IRIS_OPEN:
			str[0] = 0x40;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_IRIS_CLOSE:
			str[0] = 0x20;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_FOCUS_NEAR:
			str[0] = 0x80;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_FOCUS_FAR:
			str[0] = 0x00;str[1] = 0x01;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_ZOOM_WIDE:
			str[0] = 0x00;str[1] = 0x02;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_ZOOM_TELE:
			str[0] = 0x00;str[1] = 0x04;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_SCAN_SR:
			str[0] = 0x00;str[1] = 0xE0;str[2] = 0x00;str[3] = 0x46;
		break;
		case  PELCO_CMD_SCAN_ST:
			str[0] = 0x00;str[1] = 0xE0;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_PRESET1:
			str[0] = 0x00;str[1] = 0xE0;str[2] = 0x00;str[3] = 0x80;
		break;
		case  PELCO_CMD_PRESET2:
			str[0] = 0x00;str[1] = 0xE0;str[2] = 0x00;str[3] = 0x40;
		break;
		case  PELCO_CMD_PRESET3:
			str[0] = 0x00;str[1] = 0xE0;str[2] = 0x00;str[3] = 0xC0;
		break;
		case  PELCO_CMD_PTN1_SR:
			str[0] = 0x00;str[1] = 0xF8;str[2] = 0x00;str[3] = 0x01;
		break;
		case  PELCO_CMD_PTN1_ST:
			str[0] = 0x00;str[1] = 0x84;str[2] = 0x00;str[3] = 0x01;
		break;
		case  PELCO_CMD_PTN2_SR:
			str[0] = 0x00;str[1] = 0xF8;str[2] = 0x00;str[3] = 0x02;
		break;
		case  PELCO_CMD_PTN2_ST:
			str[0] = 0x00;str[1] = 0x84;str[2] = 0x00;str[3] = 0x02;
		break;
		case  PELCO_CMD_PTN3_SR:
			str[0] = 0x00;str[1] = 0xF8;str[2] = 0x00;str[3] = 0x03;
		break;
		case  PELCO_CMD_PTN3_ST:
			str[0] = 0x00;str[1] = 0x84;str[2] = 0x00;str[3] = 0x03;
		break;
		case  PELCO_CMD_RUN:
			str[0] = 0x00;str[1] = 0xC4;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_COMET_OFF:
			str[0] = 0x70;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
			//str[0] = 0x07;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;
		case  PELCO_CMD_COMET_ON:
			str[0] = 0xB0;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
			//str[0] = 0x0B;str[1] = 0x00;str[2] = 0x00;str[3] = 0x00;
		break;


		default : return 0; //unexpected command
	}
	#if defined(__NVP1918C_RX__)
		NVP1918C_RegWrite( 0xFF, 1);
	#else
		NVP1918C_RegWrite( 0xFF, 4);
	#endif


	NVP1918C_RegWrite( PEL_D0, str[0]);
	//NVP1918C_RegWrite( PEL_D1, str[1]);
	//NVP1918C_RegWrite( PEL_D2, str[2]);
	NVP1918C_RegWrite( PEL_D2, str[1]);	// NEW AHD coaxial protocol TEST
	//NVP1918C_RegWrite( PEL_D3, str[3]);

	printf("\r\nCOAX Instruction %d Go!!\r\n ",command-1);
	NVP1918C_RegWrite( PEL_OUT, 0x01);
	NVP1918C_RegWrite( PEL_OUT, 0x00);

	MDINDLY_mSec(20);

	return 1;
}

void samsung_coax_mode( void )
{
	NVP1918C_RegWrite( 0xFF, 1);
	NVP1918C_RegWrite( 0xBC, 0xDD);
	NVP1918C_RegWrite( 0xBD, 0xDD);

	#if defined(__NVP1918C_RX__)
		NVP1918C_RegWrite( 0xFF, 1);
	#else
		NVP1918C_RegWrite( 0xFF, 4);
	#endif


	NVP1918C_RegWrite( COAX_BAUD, 0x28);
	NVP1918C_RegWrite( BL_HSP01, 0x46);
	//NVP1918C_RegWrite( TX_START, 0x00);
	NVP1918C_RegWrite( TX_BYTE_LEN, 0x08);			// 2Line mode
	NVP1918C_RegWrite( PACKET_MODE, 0x00);

	// Video format select
	NVP1918C_RegWrite( BL_TXST1, 0x05);	// TX Line
	NVP1918C_RegWrite( BL_RXST1, 0x08);	// RX Line
	NVP1918C_RegWrite( COAX_BAUD,0x23);	// TX Duty rate
	NVP1918C_RegWrite( COAX_RBAUD, 0x23);	// TX Duty rate

	NVP1918C_RegWrite( EVEN_LINE, 0x01);
}

BYTE samsung_coax_command(BYTE command)
{
	#if defined(__NVP1918C_RX__)
		NVP1918C_RegWrite( 0xFF, 1);
	#else
		NVP1918C_RegWrite( 0xFF, 4);
	#endif

	switch(command)
	{
		case SS_CMD_SET: 		NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_SET_Buf,  8); break;
		case SS_CMD_OSD : 		NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_OSD_Buf,  8); break;
		case SS_CMD_UP: 		NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_UP_Buf,  8); break;
		case SS_CMD_DOWN: 		NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_DOWN_Buf,  8); break;
		case SS_CMD_LEFT: 		NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_LEFT_Buf,  8); break;
		case SS_CMD_RIGHT: 		NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_RIGHT_Buf,  8); break;
		case SS_CMD_PTZ_UP: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PTZ_UP_Buf,  8); break;
		case SS_CMD_PTZ_DOWN: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PTZ_DOWN_Buf,  8); break;
		case SS_CMD_PTZ_LEFT: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PTZ_LEFT_Buf, 8); break;
		case SS_CMD_PTZ_RIGHT: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PTZ_RIGHT_Buf,  8); break;
		case SS_CMD_IRIS_OPEN: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_IRIS_OPEN_Buf, 8); break;
		case SS_CMD_IRIS_CLOSE: NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_IRIS_CLOSE_Buf, 8); break;
		case SS_CMD_FOCUS_NEAR: NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_FOCUS_NEAR_Buf, 8); break;
		case SS_CMD_FOCUS_FAR: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_FOCUS_FAR_Buf, 8); break;
		case SS_CMD_ZOOM_WIDE: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_ZOOM_WIDE_Buf, 8); break;
		case SS_CMD_ZOOM_TELE: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_ZOOM_TELE_Buf, 8); break;
		case SS_CMD_SCAN_SR: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_SCAN_SR_Buf, 8); break;
		case SS_CMD_SCAN_ST: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_SCAN_ST_Buf, 8); break;
		case SS_CMD_PRESET1: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PRESET1_Buf, 8); break;
		case SS_CMD_PRESET2: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PRESET2_Buf, 8); break;
		case SS_CMD_PRESET3: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PRESET3_Buf, 8); break;
		case SS_CMD_PTN1_SR: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PATTERN_1SR_Buf, 8); break;
		case SS_CMD_PTN1_ST: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PATTERN_1ST_Buf, 8); break;
		case SS_CMD_PTN2_SR: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PATTERN_2SR_Buf, 8); break;
		case SS_CMD_PTN2_ST: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PATTERN_2ST_Buf, 8); break;
		case SS_CMD_PTN3_SR: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PATTERN_3SR_Buf, 8); break;
		case SS_CMD_PTN3_ST: 	NVP1918C_MultiWrite( SAM_D0,  (PBYTE)SAM_PATTERN_3ST_Buf, 8); break;
		default			   :    return 0; // unexpected command
	}

	//NVP1918C_MultiWrite( SAM_D0+8,  (PBYTE)SAM_2ND_CMD_Buf, 8);

	//NVP1918C_MultiWrite( SAM_D0, (PBYTE)SAM_SET_Buf, 8 );
	NVP1918C_MultiWrite( SAM_D0+8, (PBYTE)SAM_2ND_CMD_Buf, 8);
	//NVP1918C_RegWrite( 0xB0, 0x00);

	NVP1918C_RegWrite( SAM_OUT, 0x01);
	NVP1918C_RegWrite( SAM_OUT, 0x00);
	MDINDLY_mSec(20);

	return 1;
}
