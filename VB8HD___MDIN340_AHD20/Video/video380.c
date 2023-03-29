// ----------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------
#include	<string.h>
#include    "..\common\i2c.h"
#include	"..\video\video.h"
#include	"..\graphic\osd.h"
#include    "..\hdmirx\HDRx_registers.h"
#include    "..\drivers\mdinosd.h"
#include    "..\hdmirx\HDRx_main.h"

#include		"..\common\board.h"			//2		[ JYKim 		2015-5-4 ]

#if defined(SYSTEM_USE_MDIN380)
// ----------------------------------------------------------------------
// Struct/Union Types and define
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

MDIN_VIDEO_INFO		stVideo;
MDIN_INTER_WINDOW	stInterWND;
MDIN_VIDEO_WINDOW	stZOOM, stCROP;
NCDEC_INFO			ncDec;
extern SBOX_CTL_INFO stSBOX[8];


BYTE AdjInterWND,  InputSelect, InputSelOld,  SrcSyncInfo;
BYTE SrcMainFrmt, PrevSrcMainFrmt, SrcMainMode, PrevSrcMainMode;
BYTE OutMainFrmt, PrevOutMainFrmt, OutMainMode, PrevOutMainMode;
BYTE SrcAuxFrmt, PrevSrcAuxFrmt, SrcAuxMode, PrevSrcAuxMode;
BYTE OutAuxFrmt, PrevOutAuxFrmt, OutAuxMode, PrevOutAuxMode;
BYTE AdcVideoFrmt, PrevAdcFrmt, EncVideoFrmt, PrevEncFrmt;
BOOL fSyncParsed;

// ----------------------------------------------------------------------
// External Variable 
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Prototype Functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static functions
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// Exported functions
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
void HDRX_SetRegInitial(void)
{
	printf("\r\nIT6604E Initialing...");
	HDRX_RST(HIGH); 
	MDINDLY_mSec(20);

	Turn_HDMIRX(OFF);
	EnableHDMIRXVideoOutput(VIDEO_ON) ;
	HDRX_Init();
	Turn_HDMIRX(ON);
	printf("OK\r\n");
}

static void MDIN3xx_SetHCLKMode(MDIN_HOST_CLK_MODE_t mode)
{
	switch (mode) {
		case MDIN_HCLK_CRYSTAL:	TEST_MODE2( LOW); TEST_MODE1( LOW); break;
		case MDIN_HCLK_MEM_DIV: TEST_MODE2(HIGH); TEST_MODE1(HIGH); break;

	#if	defined(SYSTEM_USE_MDIN380)
		case MDIN_HCLK_HCLK_IN: TEST_MODE2( LOW); TEST_MODE1(HIGH); break;
	#endif
	}
}

//--------------------------------------------------------------------------------------------------------------------------
void MDIN3xx_TxSetRegInitial(PNCDEC_INFO pINFO)
{
	WORD nID = 0;
	BYTE vformat, resolution;

	MDIN_RST(LOW); 			
	MDINDLY_mSec(20);
	MDIN_RST(HIGH); 			
	MDINDLY_mSec(20);

///////////////// nextchip <-> mdin information ///////////////////////
	if( (pINFO->vformat) == NTSC) 	vformat = VID_VENC_NTSC_M;
	else							vformat = VID_VENC_PAL_B;
	
	if( pINFO->tx.enc_resol == NC_ENCSRC_960X480)		resolution = VIDSRC_960x480i60;
	else if( pINFO->tx.enc_resol == NC_ENCSRC_960X576)	resolution = VIDSRC_960x576i50;
	else if( pINFO->tx.enc_resol == NC_ENCSRC_720X480)	resolution = VIDSRC_720x480i60;
	else if( pINFO->tx.enc_resol == NC_ENCSRC_720X576)	resolution = VIDSRC_720x576i50;
	else resolution =  (vformat == VID_VENC_NTSC_M)?VIDSRC_960x480i60:VIDSRC_960x576i50;


///////////////////////////////////////////////////////////////////////
	printf("\r\nMDIN380 TX Initialing...");

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_CRYSTAL);	// set HCLK to MCLK/2
	MDINDLY_mSec(50);	// delay 10ms
#endif

	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id
	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_MEM_DIV);	// set HCLK to MCLK/2
	MDINDLY_mSec(10);	// delay 10ms
#endif

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA36_MAP1);	// set in-data map
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP9);	// main digital out
	MDINOSD_SetBGLayerColor(RGB(128,128,128));		// set BG-Layer color
	MDINOSD_SetBGBoxColor(RGB(0,64,128));			// set BG-BOX color
	MDINOSD_SetAuxBGColor();  						//aux background color;
	
	MDIN3xx_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDIN3xx_SetColorEnFilterCoef(NULL);		// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(NULL);		// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(NULL);		// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(NULL);		// set default colorton filter coef
	MDIN3xx_SetPeakingFilterCoef(NULL);		// set default peaking filter coef

	MDIN3xx_EnablePeakingFilter(ON);
	MDIN3xx_SetPeakingFilterLevel(0x20);	
	MDIN3xx_SetIPCBlock();		// initialize IPC block (3DNR gain is 37)

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL); 
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);

	// set aux display ON
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_OFF | MDIN_AUX_FREEZE_OFF;

	// set video path

	stVideo.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is A
	stVideo.dacPATH = DAC_PATH_MAIN_OUT;	// set main onlyde
	stVideo.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// set front format conversion
	//	stVideo.ffcH_m  = 1440;

	// define video format of PORTA-INPUT
	stVideo.stSRC_a.frmt = resolution; 
	stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8;  //format
	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of MAIN-OUTPUT
	switch(resolution)
	{
		case VIDSRC_720x480i60 : stVideo.stOUT_m.frmt = VIDOUT_720x480i60;	break;	
		case VIDSRC_960x480i60 : stVideo.stOUT_m.frmt = VIDOUT_960x480i60;	break;	
		case VIDSRC_960x576i50 : stVideo.stOUT_m.frmt = VIDOUT_960x576i50;	break;	
		default : printf("Out of resolution\r\n"); break;
	}

	stVideo.stOUT_m.mode = MDIN_OUT_MUX656_8;
	stVideo.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo.stOUT_m.brightness = 128;			// set main picture factor
	stVideo.stOUT_m.contrast = 128;
	stVideo.stOUT_m.saturation = 128;
	stVideo.stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo.stOUT_m.g_gain = 128;
	stVideo.stOUT_m.b_gain = 128;
	stVideo.stOUT_m.r_offset = 128;
	stVideo.stOUT_m.g_offset = 128;
	stVideo.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo.stIPC_m.mode = MDIN_DEINT_INTER_ONLY;
	stVideo.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo.stIPC_m.gain = 40;
	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_OFF | MDIN_DEINT_CCS_OFF;  //cross color reduction off : MDIN_DEINT_CCS_OFF

	// define map of frame buffer
	stVideo.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_OFF;	// when MDIN_DEINT_3DNR_OFF

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = resolution;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of AUX-OUTPUT
	if(vformat == VID_VENC_NTSC_M)
		stVideo.stOUT_x.frmt = VIDOUT_1920x1080p60;
	else
		stVideo.stOUT_x.frmt = VIDOUT_1920x1080p50;
	
	stVideo.stOUT_x.mode = MDIN_OUT_RGB444_8;
	stVideo.stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run

	if(vformat == VID_VENC_NTSC_M)
	{
		stVideo.stOUT_x.brightness = 128+12;			// set aux picture factor
		stVideo.stOUT_x.contrast = 128-3;
		stVideo.stOUT_x.saturation = 128;
		stVideo.stOUT_x.hue = 128;
	}
	else
	{
		stVideo.stOUT_x.brightness = 128+12;			// set aux picture factor
		stVideo.stOUT_x.contrast = 128-15;
		stVideo.stOUT_x.saturation = 128;
		stVideo.stOUT_x.hue = 128;
	}
	
#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo.stOUT_x.g_gain = 128;
	stVideo.stOUT_x.b_gain = 128;
	stVideo.stOUT_x.r_offset = 128;
	stVideo.stOUT_x.g_offset = 128;
	stVideo.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo.encFRMT = vformat;

	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = pINFO->dispmode;	 // set 4CH view mode

	// define video format of HDMI-OUTPUT
	stVideo.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo.stVID_h.fine  = HDMI_CLK_EDGE_RISE;// | HDMI_USE_FORCE_DVI; // when HDMI_USE_FORCE_DVI is removed, screen can be blinked.

	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process
	MDIN3xx_SetDelayCLK_A(4);	
	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90;		stInterWND.ry = 150;

	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);
	

	// define variable for EDK application
	InputSelOld = 0xff;	InputSelect = VIDEO_VDCNV_NVP1918C;

	PrevSrcMainFrmt = 0xff;	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;	OutMainMode = stVideo.stOUT_m.mode;

	PrevSrcAuxFrmt = 0xff; SrcAuxFrmt = stVideo.stSRC_b.frmt;
	PrevSrcAuxMode = 0xff;	SrcAuxMode = stVideo.stSRC_b.mode;

	PrevOutAuxFrmt = 0xff; OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;	OutAuxMode = stVideo.stOUT_x.mode;

	PrevEncFrmt = 0xff;	EncVideoFrmt = stVideo.encFRMT;
//	MDIN3xx_EnableMainDisplay(ON);		// set main display off

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	printf("OK\r\n");
}

void MDIN3xx_SetReg_1CHRX_Initial(PNCDEC_INFO pINFO)
{
	WORD nID = 0;
	BYTE vformat, resolution;

	MDIN_RST(LOW); 			// VDEC-RST is low
	MDINDLY_mSec(20);
	MDIN_RST(HIGH); 			// VDEC-RST is low
	MDINDLY_mSec(20);

///////////////// nextchip <-> mdin information ///////////////////////
	if( pINFO->vformat == NTSC) vformat = VID_VENC_NTSC_M;
	else						vformat = VID_VENC_PAL_B;
	
	if( pINFO->rx.resolution == NCRESOL_720X480)		resolution = VIDSRC_720x480i60;
	else if( pINFO->rx.resolution == NCRESOL_720X576)	resolution = VIDSRC_720x576i50;
	else if( pINFO->rx.resolution == NCRESOL_960X480)	resolution = VIDSRC_960x480i60;
	else if( pINFO->rx.resolution == NCRESOL_960X576)	resolution = VIDSRC_960x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1280X480)	resolution = VIDSRC_1280x480i60;
	else if( pINFO->rx.resolution == NCRESOL_1280X576)	resolution = VIDSRC_1280x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1440X480)	resolution = VIDSRC_1440x480i60;
	else if( pINFO->rx.resolution == NCRESOL_1440X576)	resolution = VIDSRC_1440x576i50;
	else resolution =  (vformat == VID_VENC_NTSC_M)?VIDSRC_960x480i60:VIDSRC_960x576i50;

///////////////////////////////////////////////////////////////////////
	printf("\r\nMDIN380 1CH RX Initialing...");

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_CRYSTAL);	// set HCLK to MCLK/2
	MDINDLY_mSec(50);	// delay 10ms
#endif

	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id
	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_MEM_DIV);	// set HCLK to MCLK/2
	MDINDLY_mSec(10);	// delay 10ms
#endif

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA36_MAP1);	// set in-data map
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP0);	// main digital out
	MDINOSD_SetBGLayerColor(RGB(128,128,128));		// set BG-Layer color
	MDINOSD_SetBGBoxColor(RGB(0,64,128));			// set BG-BOX color
	MDINOSD_SetAuxBGColor();  						//aux background color;

	MDIN3xx_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDIN3xx_SetColorEnFilterCoef(NULL);		// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(NULL);		// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(NULL);		// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(NULL);		// set default colorton filter coef
	MDIN3xx_SetPeakingFilterCoef(NULL);		// set default peaking filter coef

	MDIN3xx_EnablePeakingFilter(ON);
	MDIN3xx_SetPeakingFilterLevel(0x20);
	MDIN3xx_SetIPCBlock();		// initialize IPC block (3DNR gain is 37)

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL); 
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);

	// set aux display ON
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path

	stVideo.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is A
	stVideo.dacPATH = DAC_PATH_MAIN_OUT;		// set main onlyde
	stVideo.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// set front format conversion
	//	stVideo.ffcH_m  = 1440;

	// define video format of PORTA-INPUT
	stVideo.stSRC_a.frmt = resolution; 
	stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8;  //format
	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of MAIN-OUTPUT

	switch(vformat)
	{
		case VID_VENC_NTSC_M : stVideo.stOUT_m.frmt = VIDOUT_1920x1080p60; break;
		case VID_VENC_PAL_B  : stVideo.stOUT_m.frmt = VIDOUT_1920x1080p50; break;
	}

	stVideo.stOUT_m.mode = MDIN_OUT_RGB444_8;
	stVideo.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	if(vformat == VID_VENC_NTSC_M)
	{
		stVideo.stOUT_m.brightness = 128+12;			// set aux picture factor
		stVideo.stOUT_m.contrast = 128-3;
		stVideo.stOUT_m.saturation = 128;
		stVideo.stOUT_m.hue = 128;
	}
	else
	{
		stVideo.stOUT_m.brightness = 128+19;			// set aux picture factor
		stVideo.stOUT_m.contrast = 128-12;
		stVideo.stOUT_m.saturation = 128;
		stVideo.stOUT_m.hue = 128;
	}
	

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo.stOUT_m.g_gain = 128;
	stVideo.stOUT_m.b_gain = 128;
	stVideo.stOUT_m.r_offset = 128;
	stVideo.stOUT_m.g_offset = 128;
	stVideo.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo.stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo.stIPC_m.gain = 34;
	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_ON;  //cross color reduction off : MDIN_DEINT_CCS_OFF

	// define map of frame buffer
	stVideo.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_OFF

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = resolution;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of AUX-OUTPUT
	if(vformat == VID_VENC_NTSC_M)
		stVideo.stOUT_x.frmt = VIDOUT_720x480i60;
	else
		stVideo.stOUT_x.frmt = VIDOUT_720x576i50;
	
	stVideo.stOUT_x.mode = MDIN_OUT_MUX656_8;
	
	stVideo.stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run

	stVideo.stOUT_x.brightness = 128;			// set aux picture factor
	stVideo.stOUT_x.contrast = 128;
	stVideo.stOUT_x.saturation = 128;
	stVideo.stOUT_x.hue = 128;
	
#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo.stOUT_x.g_gain = 128;
	stVideo.stOUT_x.b_gain = 128;
	stVideo.stOUT_x.r_offset = 128;
	stVideo.stOUT_x.g_offset = 128;
	stVideo.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo.encFRMT = vformat;

	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = pINFO->dispmode;	 // set 4CH view mode

	// define video format of HDMI-OUTPUT
	stVideo.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo.stVID_h.fine  = HDMI_CLK_EDGE_RISE;// | HDMI_USE_FORCE_DVI; // when HDMI_USE_FORCE_DVI is removed, screen can be blinked.

	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process
	
	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90;		stInterWND.ry = 150;

	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	//DrawCamName(stVideo.st4CH_x.view);
	DrawCamName(stVideo.st4CH_x.view, pINFO->rx.resolution);

	// define variable for EDK application
	InputSelOld = 0xff;	InputSelect = VIDEO_VDCNV_NVP1918C;

	PrevSrcMainFrmt = 0xff;	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;	OutMainMode = stVideo.stOUT_m.mode;

	PrevSrcAuxFrmt = 0xff; SrcAuxFrmt = stVideo.stSRC_b.frmt;
	PrevSrcAuxMode = 0xff;	SrcAuxMode = stVideo.stSRC_b.mode;

	PrevOutAuxFrmt = 0xff; OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;	OutAuxMode = stVideo.stOUT_x.mode;

	PrevEncFrmt = 0xff;	EncVideoFrmt = stVideo.encFRMT;
//	MDIN3xx_EnableMainDisplay(ON);		// set main display off

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	printf("OK\r\n");
}


void MDIN3xx_SetRegInitial(PNCDEC_INFO pINFO)
{
	WORD nID = 0;
	BYTE vformat, resolution;

	MDIN_RST(LOW); 			// VDEC-RST is low
	MDINDLY_mSec(20);
	MDIN_RST(HIGH); 			// VDEC-RST is low
	MDINDLY_mSec(20);

///////////////// nextchip <-> mdin information ///////////////////////
	if( pINFO->vformat == NTSC) vformat = VID_VENC_NTSC_M;
	else						vformat = VID_VENC_PAL_B;
	
	if( pINFO->rx.resolution == NCRESOL_720X480)		resolution = VIDSRC_720x480i60;
	else if( pINFO->rx.resolution == NCRESOL_720X576)	resolution = VIDSRC_720x576i50;
	else if( pINFO->rx.resolution == NCRESOL_960X480)	resolution = VIDSRC_960x480i60;
	else if( pINFO->rx.resolution == NCRESOL_960X576)	resolution = VIDSRC_960x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1280X480)	resolution = VIDSRC_1280x480i60;
	else if( pINFO->rx.resolution == NCRESOL_1280X576)	resolution = VIDSRC_1280x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1440X480)	resolution = VIDSRC_1440x480i60;
	else if( pINFO->rx.resolution == NCRESOL_1440X576)	resolution = VIDSRC_1440x576i50;
	else resolution =  (vformat == VID_VENC_NTSC_M)?VIDSRC_960x480i60:VIDSRC_960x576i50;

///////////////////////////////////////////////////////////////////////
	printf("\r\nMDIN380 Initialing...");

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_CRYSTAL);	// set HCLK to MCLK/2
	MDINDLY_mSec(50);	// delay 10ms
#endif

	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id
	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_MEM_DIV);	// set HCLK to MCLK/2
	MDINDLY_mSec(10);	// delay 10ms
#endif

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA36_MAP1);	// set in-data map
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP0);	// main digital out
	MDINOSD_SetBGLayerColor(RGB(128,128,128));		// set BG-Layer color
	MDINOSD_SetBGBoxColor(RGB(0,64,128));			// set BG-BOX color
	MDINOSD_SetAuxBGColor();  						//aux background color;

	MDIN3xx_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDIN3xx_SetColorEnFilterCoef(NULL);		// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(NULL);		// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(NULL);		// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(NULL);		// set default colorton filter coef
	MDIN3xx_SetPeakingFilterCoef(NULL);		// set default peaking filter coef

	MDIN3xx_EnablePeakingFilter(ON);
	MDIN3xx_SetPeakingFilterLevel(0x20);
	MDIN3xx_SetIPCBlock();		// initialize IPC block (3DNR gain is 37)

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL); 
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);

	// set aux display ON
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_ON | MDIN_AUX_FREEZE_OFF;

	// set video path

	stVideo.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is A
	stVideo.dacPATH = DAC_PATH_AUX_4CH;		// set main onlyde
	stVideo.encPATH = VENC_PATH_PORT_X;		// set venc is aux

	// define video format of PORTA-INPUT
	stVideo.stSRC_a.frmt = resolution; 
	stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8;  //format
	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of MAIN-OUTPUT
	switch(resolution)
	{
		case VIDSRC_720x480i60 : stVideo.stOUT_m.frmt = VIDOUT_720x480p60;	break;
		case VIDSRC_720x576i50 : stVideo.stOUT_m.frmt = VIDOUT_720x576p50;	break;	
		case VIDSRC_960x480i60 : stVideo.stOUT_m.frmt = VIDOUT_960x480p60;	break;	
		case VIDSRC_960x576i50 : stVideo.stOUT_m.frmt = VIDOUT_960x576p50;	break;	
		case VIDSRC_1280x480i60: stVideo.stOUT_m.frmt = VIDOUT_1280x480p60;	break;	
		case VIDSRC_1280x576i50: stVideo.stOUT_m.frmt = VIDOUT_1280x576p50;	break;	
		case VIDSRC_1440x480i60: stVideo.stOUT_m.frmt = VIDOUT_1440x480p60;	break;	
		case VIDSRC_1440x576i50: stVideo.stOUT_m.frmt = VIDOUT_1440x576p50;	break;	
		default : stVideo.stOUT_m.frmt = (vformat == VID_VENC_NTSC_M)?VIDOUT_960x480p60:VIDOUT_960x576p50; break;
	}

	stVideo.stOUT_m.mode = MDIN_OUT_EMB422_8;
	stVideo.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	stVideo.stOUT_m.brightness = 128;			// set main picture factor
	stVideo.stOUT_m.contrast = 128;
	stVideo.stOUT_m.saturation = 128;
	stVideo.stOUT_m.hue = 128;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo.stOUT_m.g_gain = 128;
	stVideo.stOUT_m.b_gain = 128;
	stVideo.stOUT_m.r_offset = 128;
	stVideo.stOUT_m.g_offset = 128;
	stVideo.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo.stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	stVideo.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo.stIPC_m.gain = 40;
	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_OFF;  //cross color reduction off : MDIN_DEINT_CCS_OFF

	// define map of frame buffer
	stVideo.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_OFF

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = resolution;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of AUX-OUTPUT
	if(vformat == VID_VENC_NTSC_M)
		stVideo.stOUT_x.frmt = VIDOUT_1920x1080p60;
	else
		stVideo.stOUT_x.frmt = VIDOUT_1920x1080p50;
	
	stVideo.stOUT_x.mode = MDIN_OUT_RGB444_8;
	stVideo.stOUT_x.fine = MDIN_SYNC_FREERUN;	// set aux outsync free-run

	if(vformat == VID_VENC_NTSC_M)
	{
		stVideo.stOUT_x.brightness = 128+12;			// set aux picture factor
		stVideo.stOUT_x.contrast = 128-3;
		stVideo.stOUT_x.saturation = 128;
		stVideo.stOUT_x.hue = 128;
	}
	else
	{
		stVideo.stOUT_x.brightness = 128+12;			// set aux picture factor
		stVideo.stOUT_x.contrast = 128-15;
		stVideo.stOUT_x.saturation = 128;
		stVideo.stOUT_x.hue = 128;
	}
	
#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo.stOUT_x.g_gain = 128;
	stVideo.stOUT_x.b_gain = 128;
	stVideo.stOUT_x.r_offset = 128;
	stVideo.stOUT_x.g_offset = 128;
	stVideo.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo.encFRMT = vformat;

	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = pINFO->dispmode;	 // set 4CH view mode

	// define video format of HDMI-OUTPUT
	stVideo.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo.stVID_h.fine  = HDMI_CLK_EDGE_RISE;// | HDMI_USE_FORCE_DVI; // when HDMI_USE_FORCE_DVI is removed, screen can be blinked.

	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process
	
	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90;		stInterWND.ry = 150;

	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	//DrawCamName(stVideo.st4CH_x.view);
	DrawCamName(stVideo.st4CH_x.view, pINFO->rx.resolution);

	// define variable for EDK application
	InputSelOld = 0xff;	InputSelect = VIDEO_VDCNV_NVP1918C;

	PrevSrcMainFrmt = 0xff;	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;	OutMainMode = stVideo.stOUT_m.mode;

	PrevSrcAuxFrmt = 0xff; SrcAuxFrmt = stVideo.stSRC_b.frmt;
	PrevSrcAuxMode = 0xff;	SrcAuxMode = stVideo.stSRC_b.mode;

	PrevOutAuxFrmt = 0xff; OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;	OutAuxMode = stVideo.stOUT_x.mode;

	PrevEncFrmt = 0xff;	EncVideoFrmt = stVideo.encFRMT;
//	MDIN3xx_EnableMainDisplay(ON);		// set main display off

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	printf("OK\r\n");
}

void MDIN3xx_AHDSetRegInitial(PNCDEC_INFO pINFO)
{
	WORD nID = 0;
	BYTE vformat, resolution;

	MDIN_RST(LOW); 	
	MDINDLY_mSec(20);
	MDIN_RST(HIGH); 	
	MDINDLY_mSec(20);

///////////////// nextchip <-> mdin information ///////////////////////
	if( pINFO->vformat == NTSC) vformat = VID_VENC_NTSC_M;
	else						vformat = VID_VENC_PAL_B;
	
	if( pINFO->rx.resolution == NCRESOL_720X480)			resolution = VIDSRC_720x480i60;
	else if( pINFO->rx.resolution == NCRESOL_720X576)		resolution = VIDSRC_720x576i50;
	else if( pINFO->rx.resolution == NCRESOL_960X480)		resolution = VIDSRC_960x480i60;
	else if( pINFO->rx.resolution == NCRESOL_960X576)		resolution = VIDSRC_960x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1280X480)		resolution = VIDSRC_1280x480i60;
	else if( pINFO->rx.resolution == NCRESOL_1280X576)		resolution = VIDSRC_1280x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1440X480)		resolution = VIDSRC_1440x480i60;
	else if( pINFO->rx.resolution == NCRESOL_1440X576)		resolution = VIDSRC_1440x576i50;
	else if( pINFO->rx.resolution == NCRESOL_1280X720P)
	{
//		if( vformat == VID_VENC_NTSC_M) resolution = VIDSRC_1280x720p30;
//		else							resolution = VIDSRC_1280x720p25;
		if( vformat == VID_VENC_NTSC_M) resolution = VIDSRC_1280x720p60;
		else							resolution = VIDSRC_1280x720p50;
	}
	else
	{
		if( vformat == VID_VENC_NTSC_M) resolution = VIDSRC_1920x1080p60;
		else							resolution = VIDSRC_1920x1080p50;
	}

///////////////////////////////////////////////////////////////////////

	printf("\r\nMDIN380 %s Initialing...",( pINFO->rx.resolution == NCRESOL_1920X1080P)?"1080P":"720P");

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_CRYSTAL);	// set HCLK to MCLK/2
	MDINDLY_mSec(50);	// delay 10ms
#endif

	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id
	MDIN3xx_EnableMainDisplay(OFF);		// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

#if	!defined(SYSTEM_USE_PCI_HIF)&&defined(SYSTEM_USE_MCLK202)
	MDIN3xx_SetHCLKMode(MDIN_HCLK_MEM_DIV);	// set HCLK to MCLK/2
	MDINDLY_mSec(10);	// delay 10ms
#endif

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL);		// set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA36_MAP1);	// set in-data map
	MDIN3xx_SetDIGOutMapMode(MDIN_DIG_OUT_M_MAP0);	// main digital out
	MDINOSD_SetBGLayerColor(RGB(128,128,128));		// set BG-Layer color
	MDINOSD_SetBGBoxColor(RGB(0,64,128));			// set BG-BOX color
	MDINOSD_SetAuxBGColor();  						//aux background color;
	

	MDIN3xx_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(NULL);		// set default frontNR filter coef
	MDIN3xx_SetColorEnFilterCoef(NULL);		// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(NULL);		// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(NULL);		// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(NULL);		// set default colorton filter coef
	MDIN3xx_SetPeakingFilterCoef(NULL);		// set default peaking filter coef

	MDIN3xx_EnablePeakingFilter(ON);
	MDIN3xx_SetPeakingFilterLevel(0x20);
	MDIN3xx_SetIPCBlock();		// initialize IPC block (3DNR gain is 37)

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));
	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL);	// set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL); 
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);

	// set aux display ON
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_OFF | MDIN_AUX_FREEZE_OFF;

	// set video path

	stVideo.srcPATH = PATH_MAIN_A_AUX_M;	// set main is A, aux is A
	stVideo.dacPATH = DAC_PATH_MAIN_OUT;	// set main onlyde // 20140402
	stVideo.encPATH = VENC_PATH_PORT_X;		// set venc is aux


	// define video format of PORTA-INPUT
	stVideo.stSRC_a.frmt = resolution; 
	#if 0
	if(pINFO->rx.resolution == NCRESOL_1280X720P)
		stVideo.stSRC_a.mode = MDIN_SRC_MUX656_10;  //when progressed, 10 bit is used
	else
		stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8; 
	#else
//	if(pINFO->rx.resolution == NCRESOL_1920X1080P)
	if( (pINFO->rx.resolution == NCRESOL_1920X1080P)|| (pINFO->rx.resolution == NCRESOL_1280X720P))
	{
//		LED_G(ON);
		#if (MDIN380_INPUT_STYLE_SELECT == EMBEDED_8BIT)						//2		[ JYKim 		2015-5-4 ]
		stVideo.stSRC_a.mode = MDIN_SRC_EMB422_8;
		#elif (MDIN380_INPUT_STYLE_SELECT == SEPERATE_8BIT)
		stVideo.stSRC_a.mode = MDIN_SRC_SEP422_8;
		#elif (MDIN380_INPUT_STYLE_SELECT == MUX656_8BIT)
		stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8; 
		#elif (MDIN380_INPUT_STYLE_SELECT == EMBEDED_10BIT)
		stVideo.stSRC_a.mode = MDIN_SRC_EMB422_10;
		#elif (MDIN380_INPUT_STYLE_SELECT == SEPERATE_10BIT)
		stVideo.stSRC_a.mode = MDIN_SRC_SEP422_10;
		#elif (MDIN380_INPUT_STYLE_SELECT == MUX656_10BIT)
		stVideo.stSRC_a.mode = MDIN_SRC_MUX656_10;
		#elif (MDIN380_INPUT_STYLE_SELECT == YUV444_8)
		stVideo.stSRC_a.mode = MDIN_SRC_YUV444_8;;
		#else
		stVideo.stSRC_a.mode = MDIN_SRC_RGB444_8;
		#endif
	}
	else
		stVideo.stSRC_a.mode = MDIN_SRC_MUX656_8; 
	#endif
		
	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of MAIN-OUTPUT
#if 1	
	switch(vformat)
	{
			#if (MDIN380_OUTPUT_RESOLUTION_SELECT == _1080p_60)||(MDIN380_OUTPUT_RESOLUTION_SELECT == _1080p_50)			//2		[ JYKim 		2015-5-4 ]
			case VID_VENC_NTSC_M : stVideo.stOUT_m.frmt = VIDOUT_1920x1080p60; break;
			case VID_VENC_PAL_B  : stVideo.stOUT_m.frmt = VIDOUT_1920x1080p50; break;
			#elif (MDIN380_OUTPUT_RESOLUTION_SELECT == _1080p_30)||(MDIN380_OUTPUT_RESOLUTION_SELECT == _1080p_25)			//2		[ JYKim 		2015-5-4 ]
			case VID_VENC_NTSC_M : stVideo.stOUT_m.frmt = VIDOUT_1920x1080p30; break;
			case VID_VENC_PAL_B  : stVideo.stOUT_m.frmt = VIDOUT_1920x1080p25; break;
			#else
			case VID_VENC_NTSC_M : stVideo.stOUT_m.frmt = VIDOUT_1280x720p60; break;
			case VID_VENC_PAL_B  : stVideo.stOUT_m.frmt = VIDOUT_1280x720p50; break;
			#endif
	}
#else
	switch(vformat)
	{
		case VID_VENC_NTSC_M : stVideo.stOUT_m.frmt = VIDOUT_1280x720p60; break;
		case VID_VENC_PAL_B  : stVideo.stOUT_m.frmt = VIDOUT_1280x720p50; break;
	}
#endif
	stVideo.stOUT_m.mode = MDIN_OUT_RGB444_8;//MDIN_OUT_RGB444_8;
	stVideo.stOUT_m.fine = MDIN_SYNC_FREERUN;	// set main outsync free-run

	if(vformat == VID_VENC_NTSC_M)
	{
		stVideo.stOUT_m.brightness = 0x80;			// set aux picture factor
		stVideo.stOUT_m.contrast = 0x68;
		stVideo.stOUT_m.saturation = 0x78;
		stVideo.stOUT_m.hue = 0x80;
	}
	else
	{
		stVideo.stOUT_m.brightness = 0x80;			// set aux picture factor
		stVideo.stOUT_m.contrast = 0x68;
		stVideo.stOUT_m.saturation = 0x78;
		stVideo.stOUT_m.hue = 0x80;
	}

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_m.r_gain = 128;				// set main gain/offset
	stVideo.stOUT_m.g_gain = 128;
	stVideo.stOUT_m.b_gain = 128;
	stVideo.stOUT_m.r_offset = 128;
	stVideo.stOUT_m.g_offset = 128;
	stVideo.stOUT_m.b_offset = 128;
#endif

	// define video format of IPC-block
	stVideo.stIPC_m.mode = MDIN_DEINT_ADAPTIVE;
	if ((resolution==VIDSRC_1280x720p30)||(resolution==VIDSRC_1280x720p25)||(resolution==VIDSRC_1920x1080p50)||(resolution==VIDSRC_1920x1080p60))	//2		[ JYKim 		2015-5-4 ]
	{
		stVideo.stSRC_m.stATTB.attb = MDIN_SCANTYPE_PROG;
	}
	else
	{
		stVideo.stSRC_m.stATTB.attb = MDIN_SCANTYPE_INTR;
	}
	
	stVideo.stIPC_m.film = MDIN_DEINT_FILM_OFF;
	stVideo.stIPC_m.gain = 40;
	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_ON | MDIN_DEINT_CCS_OFF;  //cross color reduction off : MDIN_DEINT_CCS_OFF

	// define map of frame buffer
	stVideo.stMAP_m.frmt = MDIN_MAP_AUX_ON_NR_ON;	// when MDIN_DEINT_3DNR_OFF

	// define video format of PORTB-INPUT
	stVideo.stSRC_b.frmt = resolution;
	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of AUX-OUTPUT
	if(vformat == VID_VENC_NTSC_M)
		stVideo.stOUT_x.frmt = VIDOUT_720x480i60;
	else
		stVideo.stOUT_x.frmt = VIDOUT_720x576i50;
	
	stVideo.stOUT_x.mode = MDIN_OUT_MUX656_8;

#if RGB_GAIN_OFFSET_TUNE == 1
	stVideo.stOUT_x.r_gain = 128;				// set aux gain/offset
	stVideo.stOUT_x.g_gain = 128;
	stVideo.stOUT_x.b_gain = 128;
	stVideo.stOUT_x.r_offset = 128;
	stVideo.stOUT_x.g_offset = 128;
	stVideo.stOUT_x.b_offset = 128;
#endif

	// define video format of video encoder
	stVideo.encFRMT = vformat;

	// define video format of 4CH-display
	stVideo.st4CH_x.chID  = MDIN_4CHID_IN_SYNC;	 // set CH-ID extract
	stVideo.st4CH_x.order = MDIN_4CHID_A1A2B1B2; // set CH-ID mapping
	stVideo.st4CH_x.view  = pINFO->dispmode;	 // set 4CH view mode

	// define video format of HDMI-OUTPUT
	stVideo.stVID_h.mode  = HDMI_OUT_RGB444_8;
	stVideo.stVID_h.fine  = HDMI_CLK_EDGE_RISE;// | HDMI_USE_FORCE_DVI; // when HDMI_USE_FORCE_DVI is removed, screen can be blinked.

	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process
	
	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90;		stInterWND.ry = 150;

	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	//DrawCamName(stVideo.st4CH_x.view);
	DrawCamName(stVideo.st4CH_x.view, pINFO->rx.resolution);

	// define variable for EDK application
	InputSelOld = 0xff;	InputSelect = VIDEO_VDCNV_NVP1918C;

	PrevSrcMainFrmt = 0xff;	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;	OutMainMode = stVideo.stOUT_m.mode;

	PrevSrcAuxFrmt = 0xff; SrcAuxFrmt = stVideo.stSRC_b.frmt;
	PrevSrcAuxMode = 0xff;	SrcAuxMode = stVideo.stSRC_b.mode;

	PrevOutAuxFrmt = 0xff; OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;	OutAuxMode = stVideo.stOUT_x.mode;

	PrevEncFrmt = 0xff;	EncVideoFrmt = stVideo.encFRMT;

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	printf("OK\r\n");
}


void MDIN3xx_AHDTuning(BYTE b, BYTE c, BYTE s, BYTE h)
{
	WORD nID = 0;
	BYTE vformat, resolution;

#if 1	
	stVideo.stOUT_m.brightness = b;			// set aux picture factor
	stVideo.stOUT_m.contrast = c;
	stVideo.stOUT_m.saturation = s;
	stVideo.stOUT_m.hue = h;
#else
	if(vformat == VID_VENC_NTSC_M)
	{
		stVideo.stOUT_m.brightness = 128; 		// set aux picture factor
		stVideo.stOUT_m.contrast = 128;
		stVideo.stOUT_m.saturation = 128;
		stVideo.stOUT_m.hue = 128;
	}
	else
	{
		stVideo.stOUT_m.brightness = 128;			// set aux picture factor
		stVideo.stOUT_m.contrast = 128;
		stVideo.stOUT_m.saturation = 128+20;
		stVideo.stOUT_m.hue = 128;
	}
#endif
//	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
//	MDIN3xx_VideoProcess(&stVideo);			// mdin3xx main video process
	MDIN3xx_SetOutVideoCSC(&stVideo);
	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90;		stInterWND.ry = 150;

	// define variable for EDK application
	InputSelOld = 0xff;	InputSelect = VIDEO_VDCNV_NVP1918C;

	PrevSrcMainFrmt = 0xff;	SrcMainFrmt = stVideo.stSRC_a.frmt;
	PrevSrcMainMode = 0xff;	SrcMainMode = stVideo.stSRC_a.mode;

	PrevOutMainFrmt = 0xff;	OutMainFrmt = stVideo.stOUT_m.frmt;
	PrevOutMainMode = 0xff;	OutMainMode = stVideo.stOUT_m.mode;

	PrevSrcAuxFrmt = 0xff; SrcAuxFrmt = stVideo.stSRC_b.frmt;
	PrevSrcAuxMode = 0xff;	SrcAuxMode = stVideo.stSRC_b.mode;

	PrevOutAuxFrmt = 0xff; OutAuxFrmt = stVideo.stOUT_x.frmt;
	PrevOutAuxMode = 0xff;	OutAuxMode = stVideo.stOUT_x.mode;

	PrevEncFrmt = 0xff;	EncVideoFrmt = stVideo.encFRMT;
//	MDIN3xx_EnableMainDisplay(ON);		// set main display off

//	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process
	printf("OK\r\n");
}


void MDIN3xx_Initial(PNCDEC_INFO pINFO)
{
#if defined(__NVP1918C_RX__)
	MDIN3xx_SetRegInitial(pINFO);
#elif defined(__NVP1918C_TX__)
	MDIN3xx_TxSetRegInitial(pINFO);
#elif defined(__AHD_4CH_RX__) || defined(__AHD_2CH_RX__)
	MDIN3xx_AHDSetRegInitial(pINFO);
#elif defined(__NVP1918C_RX_1CH__)
	MDIN3xx_SetReg_1CHRX_Initial(pINFO);
#endif
}

static void SetSBoxAreaRefresh(void)
{
	switch (stVideo.stOUT_m.frmt) {
		case VIDOUT_720x480p60: 		MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 719, 479); break;
		case VIDOUT_720x576p50: 		MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 719, 575); break;
		case VIDOUT_960x480p60: 		MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 959, 479); break;
		case VIDOUT_960x576p50: 		MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 959, 575); break;
		case VIDOUT_1280x480p60: 	MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 1279, 479); break;
		case VIDOUT_1280x576p50: 	MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 1279, 575); break;
		case VIDOUT_1440x480p60: 	MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 1439, 479); break;
		case VIDOUT_1440x576p50: 	MDINOSD_SetSBoxArea(&stSBOX[0], 0, 0, 1439, 575); break;
	}
}


//--------------------------------------------------------------------------------------------------
static void SetOSDMenuRefresh(void)
{
	SetSBoxAreaRefresh();				// refresh OSD-Sbox
	MDINOSD_EnableSBoxBorder(&stSBOX[0], ON);
	MDIN3xx_EnableAuxWithMainOSD(&stVideo, ON);
}

//--------------------------------------------------------------------------------------------------------------------------
void Set_ClockPath_Quad(PNCDEC_INFO pINFO)	// 10Mar2014
{
//	if(stVideo.st4CH_x.view == MDIN_4CHVIEW_ALL)
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x249, 12,	2, 2); 
//	else
//		MDINHIF_RegField(MDIN_LOCAL_ID, 0x249, 12,	2, 1); 

	if(ncDec.rx.resolution < NCRESOL_1280X480)	
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 0xf);	// enable ch1, ch2, ch3, ch4 // added on 12Mar2014
	else
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x01e, 8, 4, 3);	// enable ch1, ch2	// added on 12Mar2014

	if((ncDec.rx.resolution == NCRESOL_1280X480) || (ncDec.rx.resolution == NCRESOL_1280X576))
	{
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 12, 4, 4); //horizontally 1ch // added on 10Mar2014
		MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x168, 0x500); //horizontal size
		MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x16A, 0x140);	 //horizontal start point
	}
	else if((ncDec.rx.resolution == NCRESOL_1440X480) || (ncDec.rx.resolution == NCRESOL_1440X576))
	{
		MDINHIF_RegField(MDIN_LOCAL_ID, 0x14A, 12, 4, 4); //horizontally 1ch // added on 10Mar2014	
		MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x168, 0x5A0); //horizontal size
		MDINHIF_RegWrite(MDIN_LOCAL_ID, 0x16A, 0x0F0);	 //horizontal start point
	}

	if(ncDec.dispmode >= MDIN_4CHVIEW_CH01 && ncDec.dispmode <= MDIN_4CHVIEW_CH04)  //3DNR ON/OFF
		MDIN3xx_EnableDeint3DNR(&stVideo,ON);
	else
		MDIN3xx_EnableDeint3DNR(&stVideo,OFF);
}

//--------------------------------------------------------------------------------------------------
void VideoProcessHandler(void)
{
#if 1		//2 [JYKIM-2022-12-23]	
	//	MDIN3xx_EnableAuxDisplay(&stVideo, ON);
		MDIN3xx_EnableMainDisplay(ON);

		if (stVideo.exeFLAG==0)	return;		// not change video formats

		MDIN3xx_VideoProcess(&stVideo);		// mdin3xx main video process

		if ( (ncDec.rx.resolution == NCRESOL_1280X720P) || (ncDec.rx.resolution == NCRESOL_1920X1080P) )
		{
			MDIN3xx_SetCDeint(ON);
		}
#else		//2 [JYKIM-2022-12-23]			org
	//	MDIN3xx_EnableAuxDisplay(&stVideo, ON);
		MDIN3xx_EnableMainDisplay(ON);

		if (stVideo.exeFLAG==0)	return;		// not change video formats

		MDIN3xx_VideoProcess(&stVideo);		// mdin3xx main video process
		SetOSDMenuRefresh();	// for framebuffer map bug

	#if defined( __NVP1918C_RX__)
		Set_ClockPath_Quad(&ncDec);
		if (stVideo.dacPATH==DAC_PATH_AUX_4CH||stVideo.dacPATH==DAC_PATH_AUX_2HD)           
			MDINHIF_RegField(MDIN_LOCAL_ID, 0x242, 0, 14, 0);
	#elif (defined(__NVP1918C_TX__)||defined(__AHD_TX__))
		MDIN3xx_SetCDeint(ON);
	#elif defined(__AHD_4CH_RX__)
		#if 0
		if(ncDec.rx.resolution == NCRESOL_1280X720P)
			MDIN3xx_SetCDeint(ON);
		#else
		if(ncDec.rx.resolution == NCRESOL_1920X1080P)
		{
			MDIN3xx_SetCDeint(ON);
		}
		#endif
	#endif
#endif
}

//--------------------------------------------------------------------------------------------------
void VideoHTXCtrlHandler(void)
{
	MDINHTX_CtrlHandler(&stVideo);
}
#endif	/* defined(SYSTEM_USE_MDIN325A) */

/*  FILE_END_HERE */
