#include	<string.h>
#include	"../video/video.h"

#define FLASH_BASE_ADDR   		0x08070000
#define FLASH_ADDR_VFORMAT   	0x00
#define FLASH_ADDR_RESOLUTION   0x04
#define FLASH_ADDR_ENC_RESOL   	0x08
#define FLASH_ADDR_ENC_INPUTCLK 0x0C
#define FLASH_ADDR_PLLCLK   	0x10
#define FLASH_ADDR_VCLK   		0x14
#define FLASH_ADDR_COMET   		0x18
#define FLASH_ADDR_ENC_BUSSIZE	0x1C

DWORD FLASH_RegRead(DWORD rAddr)
{
	return *(vu32*)(FLASH_BASE_ADDR+rAddr);
}

BYTE Write_Config(PNCDEC_INFO pINFO)
{
	FLASH_Unlock();
	FLASH_ErasePage(FLASH_BASE_ADDR);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_VFORMAT, pINFO->vformat);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_RESOLUTION, pINFO->rx.resolution);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_ENC_RESOL, pINFO->tx.enc_resol);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_ENC_INPUTCLK, pINFO->tx.enc_inputclk);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_PLLCLK, pINFO->rx.pllclk);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_VCLK, pINFO->rx.vclk);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_COMET, pINFO->tx.comet);
	FLASH_ProgramWord(FLASH_BASE_ADDR+FLASH_ADDR_ENC_BUSSIZE, pINFO->tx.enc_input_bus);
	FLASH_Lock();

	return 1;
}

void Load_Config(PNCDEC_INFO pINFO)
{
	pINFO->vformat 			= (BYTE)FLASH_RegRead(FLASH_ADDR_VFORMAT);
	pINFO->rx.resolution 	= (BYTE)FLASH_RegRead(FLASH_ADDR_RESOLUTION);
	pINFO->rx.pllclk		= (BYTE)FLASH_RegRead(FLASH_ADDR_PLLCLK);
	pINFO->rx.vclk 			= (BYTE)FLASH_RegRead(FLASH_ADDR_VCLK);  
	pINFO->tx.comet			= (BYTE)FLASH_RegRead(FLASH_ADDR_COMET);
	pINFO->tx.enc_resol		= (BYTE)FLASH_RegRead(FLASH_ADDR_ENC_RESOL);
	pINFO->tx.enc_inputclk	= (BYTE)FLASH_RegRead(FLASH_ADDR_ENC_INPUTCLK);
	pINFO->tx.enc_input_bus	= (BYTE)FLASH_RegRead(FLASH_ADDR_ENC_BUSSIZE);

//	pINFO->vformat = NTSC;				// ORG

	#if ( (MDIN380_INPUT_RESOLUTION_SELECT == _720p_30) || (MDIN380_INPUT_RESOLUTION_SELECT == _720p_60) )
	if((pINFO->vformat 		> PAL) || 
	   (pINFO->rx.resolution	> NCRESOL_1280X720P) ||
	   (pINFO->tx.enc_resol 	> NC_ENCSRC_1280X720P) ||
	   (pINFO->tx.enc_inputclk > ENC_CLK_148M) ||
	   (pINFO->rx.pllclk 		> PLL_36M) ||
	   (pINFO->rx.vclk 		> VCLK_144M) ||
	   (pINFO->tx.comet 		> ON)	 ||
	   (pINFO->tx.enc_input_bus > ENC_INBUS_16B))
	#else
	if((pINFO->vformat 		> PAL) || 
	   (pINFO->rx.resolution	> NCRESOL_1920X1080P) ||
	   (pINFO->tx.enc_resol 	> NC_ENCSRC_1920X1080P) ||
	   (pINFO->tx.enc_inputclk > ENC_CLK_148M) ||
	   (pINFO->rx.pllclk 		> PLL_36M) ||
	   (pINFO->rx.vclk 		> VCLK_144M) ||
	   (pINFO->tx.comet 		> ON)	 ||
	   (pINFO->tx.enc_input_bus > ENC_INBUS_16B))
	#endif
	{
	printf("Flash default setting\r\n");
/////////////////////Don't touch/////////////////////////////////
///////////////////////////////////////////////////////////////   
	pINFO->vformat			= 0x00;
	pINFO->rx.resolution	= 0x00;
	pINFO->rx.pllclk		= 0x00;
	pINFO->rx.vclk			= 0x00;
	pINFO->tx.enc_resol 	= 0x00;
	pINFO->tx.enc_inputclk	= 0x00;
	pINFO->tx.comet 		= 0x00;
	pINFO->tx.enc_input_bus = 0x00;

#if defined(__NVP1918C_TX__)
	pINFO->vformat			= PAL;
	pINFO->tx.enc_resol		= NC_ENCSRC_960X576;
	pINFO->tx.enc_inputclk	= ENC_CLK_36M;
	pINFO->tx.comet			= ON;
	pINFO->tx.enc_input_bus = ENC_INBUS_8B;
#elif defined(__NVP1918C_RX__) 
	pINFO->vformat 			= PAL;
	pINFO->rx.resolution	= NCRESOL_960X576;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_72M;
#elif defined(__NVP1918C_RX_1CH__)
	pINFO->vformat			= PAL;
	pINFO->rx.resolution	= NCRESOL_960X576;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_36M;
#elif defined(__AHD_TX__)
	pINFO->vformat			= NTSC;
	pINFO->tx.enc_resol		= NC_ENCSRC_1280X720P;
	pINFO->tx.enc_inputclk 	= ENC_CLK_74M;
	pINFO->tx.comet			= ON;
	pINFO->tx.enc_input_bus = ENC_INBUS_16B;
#elif defined(__AHD_4CH_RX__)
	#if ( (MDIN380_INPUT_RESOLUTION_SELECT == _720p_60)||(MDIN380_INPUT_RESOLUTION_SELECT == _720p_30) )
	pINFO->vformat			= NTSC;
	pINFO->rx.resolution	= NCRESOL_1280X720P;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_72M;
	#else
	pINFO->vformat			= NTSC;
	pINFO->rx.resolution	= NCRESOL_1920X1080P;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_144M;
	#endif
	
#elif defined(__AHD_2CH_RX__)
	pINFO->vformat			= PAL;
	pINFO->rx.resolution	= NCRESOL_1280X720P;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_72M;
#endif
	Write_Config(pINFO);
	}
	#if ( (MDIN380_INPUT_RESOLUTION_SELECT == _720p_60)||(MDIN380_INPUT_RESOLUTION_SELECT == _720p_30) )
	pINFO->vformat			= NTSC;
	pINFO->rx.resolution	= NCRESOL_1280X720P;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_72M;
	#else
	pINFO->vformat			= NTSC;
	pINFO->rx.resolution	= NCRESOL_1920X1080P;
	pINFO->rx.pllclk		= PLL_36M;
	pINFO->rx.vclk 			= VCLK_144M;
	#endif
}

