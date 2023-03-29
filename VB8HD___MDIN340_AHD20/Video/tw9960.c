#include	<string.h>
#include	"video.h"

void TW9960_DecoderInitial(PNCDEC_INFO pINFO, BYTE in_type)
{
#if defined(__NVP1918C_TX__)
	TW_RST(LOW);
	MDINDLY_mSec(20);
	TW_RST(HIGH);
	MDINDLY_mSec(20);

printf("\r\nTW9960 Initialing...");
	if(pINFO->vformat == NTSC)
	{
		TW9960_RegWrite( 0x88, 0x09);  
		TW9960_RegWrite( 0x03, 0xA2);  
		TW9960_RegWrite( 0x04, 0x0B);  
		TW9960_RegWrite( 0x05, 0x09);  
		TW9960_RegWrite( 0x08, 0x13);  ncDec->resolution	= NCRESOL_1280X720P) ||
		TW9960_RegWrite( 0x09, 0xF2);  
		TW9960_RegWrite( 0x0A, 0x08);
		TW9960_RegWrite( 0x19, 0x57);  
		TW9960_RegWrite( 0x1A, 0x0F);  
		TW9960_RegWrite( 0x1B, 0xC0); //jungwan
		TW9960_RegWrite( 0x1C, 0x00); 
		TW9960_RegWrite( 0x29, 0x03);  
		TW9960_RegWrite( 0x06, 0x80);  
	}
	else
	{
		TW9960_RegWrite( 0x88, 0x09);  
		TW9960_RegWrite( 0x03, 0xA2);  
		TW9960_RegWrite( 0x04, 0x0B);  
		TW9960_RegWrite( 0x05, 0x09);  
		TW9960_RegWrite( 0x08, 0x19);  
		TW9960_RegWrite( 0x09, 0x1F);  
		TW9960_RegWrite( 0x0A, 0x04);
		TW9960_RegWrite( 0x19, 0x57);  
		TW9960_RegWrite( 0x1A, 0x0F);  
		TW9960_RegWrite( 0x1B, 0xC0); //jungwan
		TW9960_RegWrite( 0x1C, 0x01);
		TW9960_RegWrite( 0x29, 0x03);  
		TW9960_RegWrite( 0x06, 0x80); 
	}

	if(in_type == SVIDEO_TW)
	{
		TW9960_RegWrite( 0x88, 0x01); 
		TW9960_RegWrite( 0x02, 0x54); 
		TW9960_RegWrite( 0x35, 0xA0); // C clamp disable
	}
	else
	{
		TW9960_RegWrite( 0x88, 0x01); 
		TW9960_RegWrite( 0x02, 0x40); 
	}
printf("OK\r\n");	
#endif
}

