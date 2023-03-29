#include	<string.h>
#include	"..\video\video.h"
#include	"..\graphic\osd.h"
#include    "flash_config.h"



extern NCDEC_INFO	ncDec;
extern BYTE disp_mode;
extern void vformat_indicator(BYTE vformat);

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

BYTE	CAMERA_STATUS=_NO_SIGNAL;

void KJY_CTLR_FLAG_INIT(void);
void KJY_INPUT_DETECT_CTRL(void);
void KJY_AHD_REINITIAL_CTRL(void);
void KJY_AHD_VARIABLE_INIT(void);

void KJY_UART_CTRL(void);
void KJY_UART_RX_PROCESS(void);



BYTE	KJY_CTRL_FLAG;
WORD	KJY_CTRL_FLAG_COUNT;

BYTE	KJY_AHD_REINITIAL_FLAG;
WORD	KJY_AHD_REINITIAL_FLAG_COUNT;

BYTE	KJY_UART2_ACK;

BYTE	KJY_UART2_OVERTIME_FLAG;
WORD	KJY_UART2_OVERTIME_CNT;


WORD kjy_data_retry_count;
BYTE kjy_data_retry_flag;


void KJY_UART2_TIME_CEHCK(void);
void KJY_UART2_RETRY_CTRL(void);


extern void Serial2RxCount_INIT(void);
extern void KJY_UART2_RETRY_INIT(void);
extern void KJY_UART2_RETRY_CLEAR(void);
extern void KJY_UART2_SET_RETRY(void);
extern BYTE KJY_UART2_GET_RETRY(void);
extern void KJY_UART2_BYTE5_TX(BYTE _data1, BYTE _data2);
extern void KJY_UART2_RX_PROCESS(void);


extern void KJY_LED_R_INIT(void);
extern void KJY_LED_R_CLEAR(void);
extern void KJY_LED_R_REFRESH(void);
extern void KJY_LED_R_CTRL(void);


extern BYTE KJY_UART2_COMMNAD_BACKUP[];
extern BYTE		CAMERA_CH_STATUS;
extern BYTE MSTAR_LOAD_FLAG;
extern BYTE		MSTAR_TRIGGER_READY_FLAG;
extern WORD MSTAR_LOAD_COUNT;

//--------------------------------------------------------------------------------------------------
static void HDMI_RxCtrlHandler(void)
{
	if (InputSelect!=VIDEO_DIGITAL_HDMI) return;

	HDRX_VideoTask();
}

//--------------------------------------------------------------------------------------------------

static void bd_type_info(PNCDEC_INFO pINFO)
{
	printf("\r\nBOARD TYPE : ");
#if defined(__NVP1918C_RX__)
	printf("NVP1918C RX BOARD\r\n");
#elif defined (__NVP1918C_TX__)	
	printf("NVP1918C TX BOARD\r\n");
#elif defined (__AHD_4CH_RX__)	
	printf("AHD 4CH RX BOARD\r\n");
#elif defined (__NVP1918C_RX_1CH__)	
	printf("NVP1918C 1CH RX BOARD\r\n");
#elif defined (__AHD_2CH_RX__)	
	printf("AHD 2CH RX BOARD\r\n");
#elif defined (__AHD_TX__)	
	printf("AHD TX BOARD\r\n");
#endif
	printf("FIRMWARE VERSION : %s\r\n","2014.04.23");


printf("Video format : %s\r\n",pINFO->vformat==NTSC?"NTSC":"PAL");
#if (defined(__NVP1918C_RX__) || defined (__AHD_4CH_RX__) || defined (__NVP1918C_RX_1CH__)|| defined (__AHD_2CH_RX__))
printf("Resolution : %s\r\n", pINFO->rx.resolution==NCRESOL_720X480?"720X480":
								pINFO->rx.resolution==NCRESOL_720X576?"720X576":
								pINFO->rx.resolution==NCRESOL_960X480?"960X480":
								pINFO->rx.resolution==NCRESOL_960X576?"960X576":
								pINFO->rx.resolution==NCRESOL_1280X480?"1280X480":
								pINFO->rx.resolution==NCRESOL_1280X576?"1280X576":
								pINFO->rx.resolution==NCRESOL_1440X480?"1440X480":
								pINFO->rx.resolution==NCRESOL_1440X576?"1440X576":
//								pINFO->rx.resolution==NCRESOL_1280X720P?"1280X720P":"NONE");			// ORG
								pINFO->rx.resolution==NCRESOL_1280X720P?"1280X720P":
								pINFO->rx.resolution==NCRESOL_1920X1080P?"1920X1080P":"NONE");
									
printf("PLL CLK : %s\r\n",	pINFO->rx.pllclk==PLL_27M?"27Mhz":"36Mhz");
printf("VIDEO CLK : %sMhz\r\n", pINFO->rx.vclk==VCLK_27M?"27":
							 	pINFO->rx.vclk==VCLK_54M?"54":
							 	pINFO->rx.vclk==VCLK_108M?"108":
							 	pINFO->rx.vclk==VCLK_36M?"36":
//							 	pINFO->rx.vclk==VCLK_72M?"72":"144");									// ORG
							 	pINFO->rx.vclk==VCLK_72M?"72":
							 	pINFO->rx.vclk==VCLK_144M?"144":"NONE");
#else								
#if defined (__NVP1918C_TX__)	
printf("Encoder Resolution : %s\r\n",pINFO->tx.enc_resol==NC_ENCSRC_960X480?"960X480":"960X576");
#else
printf("Encoder Resolution : %s\r\n",pINFO->tx.enc_resol==NC_ENCSRC_1280X720P?"1280X720P":"NULL");
#endif
printf("Encoder Input CLK : %sMhz\r\n", pINFO->tx.enc_inputclk==ENC_CLK_37M?"37.125":
							 	pINFO->tx.enc_inputclk==ENC_CLK_74M?"74.25":"148.5");
#endif
}

void CreateVideoInstance(void)					//2 [JYKIM-2022-12-23]				MDIN380
{
	memset((PBYTE)&ncDec, 0, sizeof(NCDEC_INFO));

	Load_Config(&ncDec);
	#if 0
	if(ncDec.rx.resolution < NCRESOL_1280X480)	
		ncDec.dispmode  = MDIN_4CHVIEW_ALL;
	else if(ncDec.rx.resolution < NCRESOL_1920X1080P)		//2		[ JYKim 		2015-4-28 ]
		ncDec.dispmode  = MDIN_4CHVIEW_CH01;
	else
		ncDec.dispmode 	= MDIN_4CHVIEW_CH12_V;
	#else		//2 [JYKIM-2019-12-13]	
		ncDec.dispmode	= MDIN_4CHVIEW_CH01;
	#endif
	
	disp_mode =  ncDec.dispmode;

	bd_type_info(&ncDec);

//	vformat_indicator(ncDec.vformat);

//	TW9960_DecoderInitial(&ncDec, SVIDEO_TW);
	MDIN3xx_Initial(&ncDec);
	DecoderInitial(&ncDec);
	InputSelect = VIDEO_VDCNV_NVP1918C;
}

void CreateMDIN340VideoInstance(void)
{
	WORD nID = 0;

//	printf("mdin340_iniitial_start\r\n");

	MDIN_RST(LOW);	
	MDINDLY_mSec(20);	//	MDINDLY_mSec(30);
	MDIN_RST(HIGH); 	
	MDINDLY_mSec(20);	//	MDINDLY_mSec(30);

	while (nID!=0x85) MDIN3xx_GetChipID(&nID);	// get chip-id

//	printf("mdin340_chip_id___success\r\n");

	MDIN3xx_EnableMainDisplay(OFF); 	// set main display off
	MDIN3xx_SetMemoryConfig();			// initialize DDR memory

	MDIN3xx_SetVCLKPLLSource(MDIN_PLL_SOURCE_XTAL); // set PLL source
	MDIN3xx_EnableClockDrive(MDIN_CLK_DRV_ALL, ON);

//		MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP3);	// set in-data map
	MDIN3xx_SetInDataMapMode(MDIN_IN_DATA24_MAP0);	// set in-data map

	// setup enhancement
	MDIN3xx_SetFrontNRFilterCoef(NULL); 	// set default frontNR filter coef
	MDINAUX_SetFrontNRFilterCoef(NULL); 	// set default frontNR filter coef
	MDIN3xx_SetPeakingFilterCoef(NULL); 	// set default peaking filter coef
	MDIN3xx_SetColorEnFilterCoef(NULL); 	// set default color enhancer coef
	MDIN3xx_SetBlockNRFilterCoef(NULL); 	// set default blockNR filter coef
	MDIN3xx_SetMosquitFilterCoef(NULL); 	// set default mosquit filter coef
	MDIN3xx_SetColorTonFilterCoef(NULL);		// set default colorton filter coef

	MDIN3xx_EnableLTI(OFF); 				// set LTI off
	MDIN3xx_EnableCTI(OFF); 				// set CTI off
	MDIN3xx_SetPeakingFilterLevel(0);		// set peaking gain
	MDIN3xx_EnablePeakingFilter(ON);		// set peaking on
	MDIN3xx_EnableColorEnFilter(ON);		// set color enhancement on

	MDIN3xx_EnableFrontNRFilter(OFF);		// set frontNR off
	MDIN3xx_EnableBWExtension(OFF); 		// set B/W extension off

	MDIN3xx_SetIPCBlock();		// initialize IPC block (3DNR gain is 34)

	memset((PBYTE)&stVideo, 0, sizeof(MDIN_VIDEO_INFO));

	MDIN3xx_SetMFCHYFilterCoef(&stVideo, NULL); // set default MFC filters
	MDIN3xx_SetMFCHCFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVYFilterCoef(&stVideo, NULL);
	MDIN3xx_SetMFCVCFilterCoef(&stVideo, NULL);
	
	// enable irq
//	MDIN3xx_EnableIRQ(MDIN_IRQ_VSYNC_PULSE, ON);
//	MDIN3xx_EnableIRQ(MDIN_IRQ_GA_FINISH, ON);

	// set aux display OFF
	stVideo.dspFLAG = MDIN_AUX_DISPLAY_OFF | MDIN_AUX_FREEZE_ON;

	// set video path (main/aux/dac/enc)
	stVideo.srcPATH = PATH_MAIN_A_AUX_B;	// set main is A, aux is B
	stVideo.dacPATH = DAC_PATH_MAIN_OUT;	// set main only

	// if you need to front format conversion then set size.
//	stVideo.ffcH_m	= 1440;

	// define video format of PORTA-INPUT
	#if	1		//2 [JYKIM-2023-1-3]		
	stVideo.stSRC_a.frmt = VIDOUT_1920x1080p60;//VIDSRC_1920x1080p60;
	#else		//2 [JYKIM-2023-1-3]			org
	//	stVideo.stSRC_a.frmt = VIDSRC_720x480i60;
	#endif
	stVideo.stSRC_a.mode = 	MDIN_SRC_MUX656_8;	//MDIN_SRC_MUX656_10; // MDIN_SRC_MUX656_8
	stVideo.stSRC_a.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;
	stVideo.stSRC_a.offH = 0;
	stVideo.stSRC_a.offV = 0;

	// define video format of MAIN-OUTPUT
	stVideo.stOUT_m.frmt = VIDOUT_1920x1080p60;
	stVideo.stOUT_m.mode = MDIN_OUT_RGB444_8;//MDIN_OUT_YUV444_8;		//MDIN_OUT_RGB444_8;
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
	stVideo.stIPC_m.gain = 34;
	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_OFF | MDIN_DEINT_CCS_ON;
//	stVideo.stIPC_m.fine = MDIN_DEINT_3DNR_OFF | MDIN_DEINT_CCS_OFF;

	// define map of frame buffer
	stVideo.stMAP_m.frmt = MDIN_MAP_AUX_OFF_NR_ON;	// when MDIN_DEINT_3DNR_ON

	// define video format of PORTB-INPUT
	#if 1		//2 [JYKIM-2023-1-3]			test
	stVideo.stSRC_b.frmt = VIDSRC_1920x1080p60;
	#else		//2 [JYKIM-2023-1-3]			org
	stVideo.stSRC_b.frmt = VIDSRC_720x480i60;
	#endif

	stVideo.stSRC_b.mode = MDIN_SRC_MUX656_8;
	stVideo.stSRC_b.fine = MDIN_FIELDID_INPUT | MDIN_LOW_IS_TOPFLD;

	// define video format of AUX-OUTPUT
	#if 1		//2 [JYKIM-2023-1-3]			test
	stVideo.stOUT_x.frmt = VIDOUT_1920x1080p60;
	#else		//2 [JYKIM-2023-1-3]			org
	stVideo.stOUT_x.frmt = VIDOUT_720x480p60;
	#endif

	stVideo.stOUT_x.mode = MDIN_OUT_RGB444_8;
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

	// define video format of HDMI-OUTPUT
	stVideo.stVID_h.mode  = HDMI_OUT_YUV444_8;	//	HDMI_OUT_RGB444_8;
	stVideo.stVID_h.fine  = HDMI_CLK_EDGE_RISE;

	stVideo.stAUD_h.frmt  = AUDIO_INPUT_I2S_0;						// audio input format
	stVideo.stAUD_h.freq  = AUDIO_MCLK_256Fs | AUDIO_FREQ_48kHz;	// sampling frequency
	stVideo.stAUD_h.fine  = AUDIO_MAX24B_MINUS0 | AUDIO_SD_JUST_LEFT | AUDIO_WS_POLAR_HIGH |
							AUDIO_SCK_EDGE_RISE | AUDIO_SD_MSB_FIRST | AUDIO_SD_1ST_SHIFT;
	MDINHTX_SetHDMIBlock(&stVideo); 	// initialize HDMI block

	// define window for inter-area
	stInterWND.lx = 315;	stInterWND.rx = 405;
	stInterWND.ly = 90; 	stInterWND.ry = 150;
	MDIN3xx_SetDeintInterWND(&stInterWND, MDIN_INTER_BLOCK0);
	MDIN3xx_EnableDeintInterWND(MDIN_INTER_BLOCK0, OFF);

	stVideo.exeFLAG = MDIN_UPDATE_MAINFMT;	// execution of video process

	printf("mdin340_initial___complete\r\n");
}



void KJY_CTLR_FLAG_INIT(void)
{
	KJY_CTRL_FLAG = 0;
	KJY_CTRL_FLAG_COUNT = 250;	
}

void KJY_AHD_VARIABLE_INIT(void)
{
	KJY_AHD_REINITIAL_FLAG = 1;
	KJY_AHD_REINITIAL_FLAG_COUNT=1000;
}

void KJY_INPUT_DETECT_CTRL(void)
{
	BYTE tmp, tmp_ch;

	if (KJY_CTRL_FLAG == 0)
	{
		return;
	}
	else
	{
		KJY_CTLR_FLAG_INIT();
		
		AHD_RegWrite(0xFF, NVP6124B_BANK0);

		tmp_ch = 0xD0 + CAMERA_CH_STATUS;

		tmp = AHD_RegRead(tmp_ch);		
	}

	if (CAMERA_STATUS == tmp)
	{
//		printf("current ==	prev.return=0x%x\r\n ",tmp);
//		printf("already handling the current signal.\r\n ");
		return;
	}
	else if (tmp == _NO_SIGNAL)
	{
//		printf("NO_SIGNAL RETURN\r\n");
		printf("CH%d:NO_SIGNAL\r\n",(CAMERA_CH_STATUS+1) );
		CAMERA_STATUS = _NO_SIGNAL;
		MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_BLUE);
//		KJY_CHANNEL_BG_COLOR();
		KJY_LED_R_CLEAR();
//		KJY_SEND_CAM_SIGNAL_INFO((BYTE) _NO_SIGNAL);
		return;
	}
	else if (tmp == _UNKNOWN)
	{
		printf("CH%d:UNKNOWN\r\n",(CAMERA_CH_STATUS+1) );
		CAMERA_STATUS = _UNKNOWN; //_UNKNOWN;
		MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_BLUE);
//		KJY_CHANNEL_BG_COLOR();
		KJY_LED_R_CLEAR();
		return;
	}

	else
	{
//		printf("\r\n ");
//		printf("-------------------\r\n");
//		printf("  DETECT NEW SIGNAL\r\n");
//		printf("-------------------\r\n");
//		printf("tmp = 0x%x\r\n", tmp);
//		printf("\r\n ");

		if (tmp == _AFHD30P)
		{
			printf("CH%d:1080@30p\r\n",(CAMERA_CH_STATUS+1) );
			ncDec.rx.resolution = NCRESOL_1920X1080P;
//			Write_Config(&ncDec);
//			MDIN3xx_Initial(&ncDec);
//			DecoderInitial(&ncDec);
//			MDIN3xx_EnableDeint3DNR(&stVideo,OFF);
//			MDINAUX_EnableMirrorH(&stVideo, ON);
			CAMERA_STATUS=tmp;
//			MDINDLY_mSec(50);
			MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_DISABLE);
			KJY_LED_R_INIT();
//			KJY_SEND_CAM_SIGNAL_INFO((BYTE) _AFHD30P);
		}
		else if (tmp == _AHD30P)
		{
			printf("CH%d:720@30p\r\n",(CAMERA_CH_STATUS+1) );
			ncDec.rx.resolution = NCRESOL_1280X720P;
//			Write_Config(&ncDec);
//			MDIN3xx_Initial(&ncDec);
//			DecoderInitial(&ncDec);
//			MDIN3xx_EnableDeint3DNR(&stVideo,OFF);
//			MDINAUX_EnableMirrorH(&stVideo, OFF);
			CAMERA_STATUS=tmp;
//			MDINDLY_mSec(50);
			MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_DISABLE);
			KJY_LED_R_INIT();
//			KJY_SEND_CAM_SIGNAL_INFO((BYTE) _AHD30P);
		}
		else if(tmp == _NTSC_960H)
		{
			printf("CH%d:NTSC_960H\r\n",(CAMERA_CH_STATUS+1) );
			ncDec.rx.resolution = NCRESOL_720X480;//NCRESOL_960X480;
			
//			Write_Config(&ncDec);
//			MDIN3xx_Initial(&ncDec);
//			DecoderInitial(&ncDec);
//			MDIN3xx_EnableDeint3DNR(&stVideo,OFF);
			CAMERA_STATUS=tmp;
//			MDINDLY_mSec(50);
			MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_DISABLE);
			KJY_LED_R_INIT();
//			KJY_SEND_CAM_SIGNAL_INFO((BYTE) _NTSC_960H);
		}
		else if(tmp == _PAL_960H)
		{
			printf("CH%d:PAL_960H\r\n",(CAMERA_CH_STATUS+1) );
			ncDec.rx.resolution = NCRESOL_720X576;
//			Write_Config(&ncDec);
//			MDIN3xx_Initial(&ncDec);
//			DecoderInitial(&ncDec);
//			MDIN3xx_EnableDeint3DNR(&stVideo,OFF);
			CAMERA_STATUS=tmp;
//			MDINDLY_mSec(50);
			MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_DISABLE);
			KJY_LED_R_INIT();
//			KJY_SEND_CAM_SIGNAL_INFO((BYTE) _PAL_960H);
		}
		else
		{
			printf("CH%d:UNKNOWN\r\n",(CAMERA_CH_STATUS+1) );
//			printf("\r\n mdin380 initial ");
			CAMERA_STATUS=_NO_SIGNAL;
			MDIN3xx_SetOutTestPattern(MDIN_OUT_TEST_BLUE);
		}
	}
}

void KJY_AHD_REINITIAL_CTRL(void)
{
	if ( (KJY_AHD_REINITIAL_FLAG>0) && 
		(KJY_AHD_REINITIAL_FLAG_COUNT == 0) )
	{
		KJY_AHD_REINITIAL_FLAG--;
		KJY_AHD_REINITIAL_FLAG_COUNT=1000;
		
		printf("KJY_AHD_REINITIAL_FLAG=%d\r\n",KJY_AHD_REINITIAL_FLAG);
		
		ncDec.rx.resolution = NCRESOL_1920X1080P;
//		Write_Config(&ncDec);
//		MDIN3xx_Initial(&ncDec);
		DecoderInitial(&ncDec);
	}
}




void KJY_UART2_RETRY_CTRL(void)
{
	if (KJY_UART2_GET_RETRY() == 2)
	{
		puts("uart1_timeover:");
		KJY_UART2_SET_RETRY();
	}
	
	if (KJY_UART2_GET_RETRY() == 3)
	{
		puts("retry!\r\n");
		KJY_UART2_BYTE5_TX(KJY_UART2_COMMNAD_BACKUP[0], KJY_UART2_COMMNAD_BACKUP[1]);
		KJY_UART2_RETRY_CLEAR();
	}
}





void KJY_UART2_TIME_CEHCK(void)
{
	if (KJY_UART2_OVERTIME_FLAG == 2) 
	{	
		if (KJY_GET_COMMAND_PROCESS_FLAG() == 0)
		{
			KJY_UART2_VARIABLE_INIT();
		}
		else
			printf("WAIT_");
	}
}




//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------
int main(void)
{
	uComOnChipInitial();
//	MDINDLY_mSec(50);
//	MDINDLY_mSec(25);
//	CreateVideoInstance();
//	CreateSBoxInstance();	// added on 06Mar2014
	
	CreateMDIN340VideoInstance();
	
	KJY_TRIGGER_FLAG_CLEAR();
	KJY_CTLR_FLAG_INIT();
	KJY_AHD_VARIABLE_INIT();

	MLD_REQUEST_PIN(1);
	
	#if 1		//2 [JYKIM-2023-1-16]			test
		ncDec.rx.resolution = NCRESOL_1920X1080P;
		DecoderInitial(&ncDec);
	#endif
	
	MSTAR_TRIGGER_READY_FLAG = 1;
	
	CAMERA_STATUS = _AFHD30P;//_NO_SIGNAL;
	CAMERA_CH_STATUS = 0xff;//_CH1;
	MSTAR_LOAD_FLAG = 0;	//2;
	MSTAR_LOAD_COUNT = 1000;
	KJY_CTRL_FLAG = 0;
	
//	KJY_LED_R_CTRL();
	KJY_COMMAND_PROCESS_INIT();
	KJY_UART2_VARIABLE_INIT();
	KJY_UART2_RETRY_CLEAR();
	KJY_LED_R_INIT();
	
	printf("\r\n");
	printf("-----------------------------------------------\r\n");
	printf("------------	main_loop_start		-------\r\n");
	printf("-----------------------------------------------\r\n");
//	LED_G(ON);
	
	while (TRUE)
	{
//		MessageCtrlHandler();
//		KJY_AHD_REINITIAL_CTRL();
		KJY_DATA_SEND_GPIO_CHECK();
		KJY_UART2_RX_PROCESS();			//2 [JYKIM-2023-1-20]			UART2		STEP1
		KJY_UART2_COMMAND_CTRL();		//2 [JYKIM-2023-1-20]			UART2		STEP2
		KJY_UART2_TIME_CEHCK();			//2 [JYKIM-2023-1-20]			UART2		SETP3 	CLEAR	COMPLETE
		KJY_MSTAR_LOAD_CTRL();
		KJY_INPUT_DETECT_CTRL();

		VideoProcessHandler();	// video process handler
		VideoHTXCtrlHandler();	// video HDMI-TX handler
		KJY_TRIGGER_CTRL();
		KJY_AUTO_SCAN_CTRL();
//		KJY_SEND_MLD_REQUEST();
		debug_main();
//		KJY_LED_R_CTRL();
	}
}


/*  FILE_END_HERE */
