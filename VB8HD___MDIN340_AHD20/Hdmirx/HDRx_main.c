// #define OUTPUT_CCIR656
// #define OUTPUT_SYNC_EMBEDDED
// #define OUTPUT_16BIT_YUV422
// #define OUTPUT_24BIT_YUV422
// #define OUTPUT_24BIT_YUV444
#define OUTPUT_24BIT_RGB444


///////////////////////////////////////////////////
// Rev 1.01
///////////////////////////////////////////////////
// reg6C = 0x03
// reg6B = 0x11
// reg3B = 0x40
// reg6E = 0x0C
///////////////////////////////////////////////////
// Rev 1.09
///////////////////////////////////////////////////
//Reg6C=0x00
//Reg93=0x43
//Reg94=0x4F
//Reg95=0x87
//Reg96=0x33
///////////////////////////////////////////////////


/*********************************************************************************
 * HDMIRX HDMI RX sample code                                                   *
 *********************************************************************************/
#include "HDRx_typedef.h"
#include "HDRx_registers.h"
#include "..\common\board.h"
#include "..\drivers\mdintype.h"
///////////////////////////////////////////////////////////
// Definition.
///////////////////////////////////////////////////////////

#define SetSPDIFMUTE(x) SetMUTE(~(1<<O_TRI_SPDIF),(x)?(1<<O_TRI_SPDIF):0)
#define SetI2S3MUTE(x) SetMUTE(~(1<<O_TRI_I2S3),(x)?(1<<O_TRI_I2S3):0)
#define SetI2S2MUTE(x) SetMUTE(~(1<<O_TRI_I2S2),(x)?(1<<O_TRI_I2S2):0)
#define SetI2S1MUTE(x) SetMUTE(~(1<<O_TRI_I2S1),(x)?(1<<O_TRI_I2S1):0)
#define SetI2S0MUTE(x) SetMUTE(~(1<<O_TRI_I2S0),(x)?(1<<O_TRI_I2S0):0)
//#define SetALLMute() SetMUTE(B_VDO_MUTE_DISABLE,(B_VDO_MUTE_DISABLE|B_TRI_ALL))

#define SwitchHDMIRXBank(x) HDRX_RegWrite(REG_RX_BANK,(x)&1)


CHAR const * VStateStr[] = {
    "VSTATE_Off",
    "VSTATE_PwrOff",
    "VSTATE_SyncWait ",
    "VSTATE_SWReset",
    "VSTATE_SyncChecking",
    "VSTATE_HDCPSet",
    "VSTATE_HDCP_Reset",
    "VSTATE_ModeDetecting",
    "VSTATE_VideoOn",
    "VSTATE_ColorDetectReset",
    "VSTATE_Reserved"
} ;

#if defined(OUTPUT_CCIR656)
    #define HDMIRX_OUTPUT_MAPPING (B_OUTPUT_16BIT)
    #define HDMIRX_OUTPUT_TYPE (B_SYNC_EMBEDDED|B_CCIR565)
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_SYNC_EMBEDDED)
    #define HDMIRX_OUTPUT_MAPPING (B_OUTPUT_16BIT)
    #define HDMIRX_OUTPUT_TYPE (B_SYNC_EMBEDDED)
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_16BIT_YUV422)
    #define HDMIRX_OUTPUT_MAPPING (B_OUTPUT_16BIT)
    #define HDMIRX_OUTPUT_TYPE 0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_24BIT_YUV422)
    #define HDMIRX_OUTPUT_MAPPING   0
    #define HDMIRX_OUTPUT_TYPE  0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV422<<O_OUTPUT_COLOR_MODE)
#elif defined(OUTPUT_24BIT_YUV444)
    #define HDMIRX_OUTPUT_MAPPING   0
    #define HDMIRX_OUTPUT_TYPE  0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_YUV444<<O_OUTPUT_COLOR_MODE)
#else // if defined(OUTPUT_24BIT_RGB444)
    #define HDMIRX_OUTPUT_MAPPING   0
    #define HDMIRX_OUTPUT_TYPE  0
    #define HDMIRX_OUTPUT_COLORMODE (B_OUTPUT_RGB24<<O_OUTPUT_COLOR_MODE)
#endif

#ifndef I2S_DSP_SETTING
#define I2S_DSP_SETTING 0x60
#endif

// 2009/11/04  modified by jau-chih.tseng@ite.com.tw
// marked for moving into typedef.h
// typedef struct {
//     WORD HActive ;
//     WORD VActive ;
//     WORD HTotal ;
//     WORD VTotal ;
//     LONG PCLK ;
//     BYTE xCnt ;
//     WORD HFrontPorch ;
//     WORD HSyncWidth ;
//     WORD HBackPorch ;
//     BYTE VFrontPorch ;
//     BYTE VSyncWidth ;
//     BYTE VBackPorch ;
//     BYTE ScanMode:1 ;
//     BYTE VPolarity:1 ;
//     BYTE HPolarity:1 ;
// } HDMI_VTiming ;
//
// #define PROG 1
// #define INTERLACE 0
// #define Vneg 0
// #define Hneg 0
// #define Vpos 1
// #define Hpos 1
//~jau-chih.tseng@ite.com.tw 2009/11/04
///////////////////////////////////////////////////////////
// Public Data
///////////////////////////////////////////////////////////
BYTE	ucDVISCDToffCNT=0;		// 20091021 for VG-859 HDMI / DVI change issue
Video_State_Type VState = VSTATE_PwrOff ;

///////////////////////////////////////////////////////////
// Global Data
///////////////////////////////////////////////////////////
static WORD VideoCountingTimer = 0 ;
static WORD MuteResumingTimer = 0 ;
static BOOL MuteAutoOff = FALSE ;
static BYTE bOutputVideoMode = F_MODE_EN_UDFILT | F_MODE_RGB24 ;
static BOOL EnaSWCDRRest = FALSE ;
BYTE bDisableAutoAVMute = 0 ;
static BYTE ucRevision = 0xFF ;
BYTE bHDCPMode = 0 ;


//2008/06/17 modified by jj_tseng@chipadvanced.com
#define LOOP_MSEC 32
#define MS_TimeOut(x)      (((x)+LOOP_MSEC-1)/LOOP_MSEC)
// #define VSTATE_MISS_SYNC_COUNT 		MS_TimeOut(2000)// 2000ms, 2sec
#define VSTATE_MISS_SYNC_COUNT 			MS_TimeOut(15000)// 8000ms, 8sec
#define VSATE_CONFIRM_SCDT_COUNT 		MS_TimeOut(150)// 150ms // direct change into 8 times of loop.
#define HDCP_WAITING_TIMEOUT 			MS_TimeOut(3000)// 3 sec
#define CDRRESET_TIMEOUT 				MS_TimeOut(3000)// 3 sec
#define VSTATE_SWRESET_COUNT 			MS_TimeOut(500)// 500ms
#define FORCE_SWRESET_TIMEOUT  			MS_TimeOut(16000)// 15000ms, 15sec
#define VIDEO_TIMER_CHECK_COUNT 		MS_TimeOut(1000)

#define SCDT_LOST_TIMEOUT  15
static WORD SWResetTimeOut = FORCE_SWRESET_TIMEOUT;

BOOL bIntPOL = FALSE ;
static BOOL NewAVIInfoFrameF = FALSE ;
static BOOL MuteByPKG = OFF ;
static BYTE bInputVideoMode ;

static BYTE prevAVIDB1 = 0 ;
static BYTE prevAVIDB2 = 0 ;

static BOOL bVSIpresent=FALSE ;
static WORD currHTotal ;
static BYTE currXcnt ;
static BOOL currScanMode ;
static BOOL bGetSyncInfo();
// BYTE iVTimingIndex = 0xFF ;

#ifndef DISABLE_COLOR_DEPTH_RESET
static BOOL bDisableColorDepthResetState = FALSE ;
#endif

static BOOL bIgnoreVideoChgEvent=FALSE ;

// 2009/11/04 removed by jau-chih.tseng@ite.com.tw
// to avoid the wrong space using on 8051 code.
//  HDMI_VTiming code *pVTiming ;
HDMI_VTiming s_CurrentVM ;
//~jau-chih.tseng@ite.com.tw 2009/11/04

BYTE bVideoOutputOption = VIDEO_AUTO ;

BYTE SCDTErrorCnt = 0;

///////////////////////////////////////////////////////////////////////
// Global Table
///////////////////////////////////////////////////////////////////////
#ifdef USE_MODE_TABLE
static HDMI_VTiming _CODE s_VMTable[] = {
    {640,480,800,525,25175L,0x89,16,96,48,10,2,33,PROG,Vneg,Hneg},    //640x480@60Hz
    {720,480,858,525,27000L,0x80,16,62,60,9,6,30,PROG,Vneg,Hneg},    //720x480@60Hz
    {1280,720,1650,750,74000L,0x2E,110,40,220,5,5,20,PROG,Vpos,Hpos},    //1280x720@60Hz
    {1920,540,2200,562,74000L,0x2E,88,44,148,2,5,15,INTERLACE,Vpos,Hpos},    //1920x1080(I)@60Hz
    {720,240,858,262,13500L,0xFF,19,62,57,4,3,15,INTERLACE,Vneg,Hneg},    //720x480(I)@60Hz
    {720,240,858,262,13500L,0xFF,19,62,57,4,3,15,PROG,Vneg,Hneg},    //720x480(I)@60Hz
    {1440,240,1716,262,27000L,0x80,38,124,114,5,3,15,INTERLACE,Vneg,Hneg},    //1440x480(I)@60Hz
    {1440,240,1716,263,27000L,0x80,38,124,114,5,3,15,PROG,Vneg,Hneg},    //1440x240@60Hz
    {2880,240,3432,262,54000L,0x40,76,248,288,4,3,15,INTERLACE,Vneg,Hneg},    //2880x480(I)@60Hz
    {2880,240,3432,262,54000L,0x40,76,248,288,4,3,15,PROG,Vneg,Hneg},    //2880x240@60Hz
    {2880,240,3432,263,54000L,0x40,76,248,288,5,3,15,PROG,Vneg,Hneg},    //2880x240@60Hz
    {1440,480,1716,525,54000L,0x40,32,124,120,9,6,30,PROG,Vneg,Hneg},    //1440x480@60Hz
    {1920,1080,2200,1125,148352L,0x17,88,44,148,4,5,36,PROG,Vpos,Hpos},    //1920x1080@60Hz
    {720,576,864,625,27000L,0x80,12,64,68,5,5,36,PROG,Vneg,Hneg},    //720x576@50Hz
    {1280,720,1980,750,74000L,0x2E,440,40,220,5,5,20,PROG,Vpos,Hpos},    //1280x720@50Hz
    {1920,540,2640,562,74000L,0x2E,528,44,148,2,5,15,INTERLACE,Vpos,Hpos},    //1920x1080(I)@50Hz
    {1440/2,288,1728/2,312,13500L,0xFF,24/2,126/2,138/2,2,3,19,INTERLACE,Vneg,Hneg},    //1440x576(I)@50Hz
    {1440,288,1728,312,27000L,0x80,24,126,138,2,3,19,INTERLACE,Vneg,Hneg},    //1440x576(I)@50Hz
    {1440/2,288,1728/2,312,13500L,0xFF,24/2,126/2,138/2,2,3,19,PROG,Vneg,Hneg},    //1440x288@50Hz
    {1440,288,1728,313,27000L,0x80,24,126,138,3,3,19,PROG,Vneg,Hneg},    //1440x288@50Hz
    {1440,288,1728,314,27000L,0x80,24,126,138,4,3,19,PROG,Vneg,Hneg},    //1440x288@50Hz
    {2880,288,3456,312,54000L,0x40,48,252,276,2,3,19,INTERLACE,Vneg,Hneg},    //2880x576(I)@50Hz
    {2880,288,3456,312,54000L,0x40,48,252,276,2,3,19,PROG,Vneg,Hneg},    //2880x288@50Hz
    {2880,288,3456,313,54000L,0x40,48,252,276,3,3,19,PROG,Vneg,Hneg},    //2880x288@50Hz
    {2880,288,3456,314,54000L,0x40,48,252,276,4,3,19,PROG,Vneg,Hneg},    //2880x288@50Hz
    {1440,576,1728,625,54000L,0x40,24,128,136,5,5,39,PROG,Vpos,Hneg},    //1440x576@50Hz
    {1920,1080,2640,1125,148000L,0x17,528,44,148,4,5,36,PROG,Vpos,Hpos},    //1920x1080@50Hz
    {1920,1080,2750,1125,74000L,0x2E,638,44,148,4,5,36,PROG,Vpos,Hpos},    //1920x1080@24Hz
    {1920,1080,2640,1125,74000L,0x2E,528,44,148,4,5,36,PROG,Vpos,Hpos},    //1920x1080@25Hz
    {1920,1080,2200,1125,74000L,0x2E,88,44,148,4,5,36,PROG,Vpos,Hpos},    //1920x1080@30Hz
    // VESA mode
    {640,350,832,445,31500L,0x6D,32,64,96,32,3,60,PROG,Vneg,Hpos},         // 640x350@85
    {640,400,832,445,31500L,0x6D,32,64,96,1,3,41,PROG,Vneg,Hneg},          // 640x400@85
    {832,624,1152,667,57283L,0x3C,32,64,224,1,3,39,PROG,Vneg,Hneg},        // 832x624@75Hz
    {720,350,900,449,28322L,0x7A,18,108,54,59,2,38,PROG,Vneg,Hneg},        // 720x350@70Hz
    {720,400,900,449,28322L,0x7A,18,108,54,13,2,34,PROG,Vpos,Hneg},        // 720x400@70Hz
    {720,400,936,446,35500L,0x61,36,72,108,1,3,42,PROG,Vpos,Hneg},         // 720x400@85
    {640,480,800,525,25175L,0x89,16,96,48,10,2,33,PROG,Vneg,Hneg},         // 640x480@60
    {640,480,832,520,31500L,0x6D,24,40,128,9,3,28,PROG,Vneg,Hneg},         // 640x480@72
    {640,480,840,500,31500L,0x6D,16,64,120,1,3,16,PROG,Vneg,Hneg},         // 640x480@75
    {640,480,832,509,36000L,0x60,56,56,80,1,3,25,PROG,Vneg,Hneg},          // 640x480@85
    {800,600,1024,625,36000L,0x60,24,72,128,1,2,22,PROG,Vpos,Hpos},        // 800x600@56
    {800,600,1056,628,40000L,0x56,40,128,88,1,4,23,PROG,Vpos,Hpos},        // 800x600@60
    {800,600,1040,666,50000L,0x45,56,120,64,37,6,23,PROG,Vpos,Hpos},       // 800x600@72
    {800,600,1056,625,49500L,0x45,16,80,160,1,3,21,PROG,Vpos,Hpos},        // 800x600@75
    {800,600,1048,631,56250L,0x3D,32,64,152,1,3,27,PROG,Vpos,Hpos},        // 800X600@85
    {848,480,1088,517,33750L,0x66,16,112,112,6,8,23,PROG,Vpos,Hpos},       // 840X480@60
    {1024,384,1264,408,44900L,0x4d,8,176,56,0,4,20,INTERLACE,Vpos,Hpos},    //1024x768(I)@87Hz
    {1024,768,1344,806,65000L,0x35,24,136,160,3,6,29,PROG,Vneg,Hneg},      // 1024x768@60
    {1024,768,1328,806,75000L,0x2E,24,136,144,3,6,29,PROG,Vneg,Hneg},      // 1024x768@70
    {1024,768,1312,800,78750L,0x2B,16,96,176,1,3,28,PROG,Vpos,Hpos},       // 1024x768@75
    {1024,768,1376,808,94500L,0x24,48,96,208,1,3,36,PROG,Vpos,Hpos},       // 1024x768@85
    {1152,864,1600,900,108000L,0x20,64,128,256,1,3,32,PROG,Vpos,Hpos},     // 1152x864@75


    {1280,768,1440,790,68250L,0x32,48,32,80,3,7,12,PROG,Vneg,Hpos},        // 1280x768@60-R
    {1280,768,1664,798,79500L,0x2B,64,128,192,3,7,20,PROG,Vpos,Hneg},      // 1280x768@60
    {1280,768,1696,805,102250L,0x21,80,128,208,3,7,27,PROG,Vpos,Hneg},     // 1280x768@75
    {1280,768,1712,809,117500L,0x1D,80,136,216,3,7,31,PROG,Vpos,Hneg},     // 1280x768@85

    {1280,800,1440,823,71000L,0x31,48,32,80,3,6,14,PROG,Vneg,Hpos}, // 1280x800@60-R HReq = 49.306KHz
    {1280,800,1680,831,83500L,0x2A,72,128,200,3,6,22,PROG,Vpos,Hneg},//1280x800@60, HReq = 49.702KHz


    {1280,960,1800,1000,108000L,0x20,96,112,312,1,3,36,PROG,Vpos,Hpos},    // 1280x960@60
    {1280,960,1728,1011,148500L,0x17,64,160,224,1,3,47,PROG,Vpos,Hpos},    // 1280x960@85
    {1280,1024,1688,1066,108000L,0x20,48,112,248,1,3,38,PROG,Vpos,Hpos},   // 1280x1024@60
    {1280,1024,1688,1066,135000L,0x19,16,144,248,1,3,38,PROG,Vpos,Hpos},   // 1280x1024@75
    {1280,1024,1728,1072,157500L,0x15,64,160,224,1,3,44,PROG,Vpos,Hpos},   // 1280X1024@85
    {1360,768,1792,795,85500L,0x28,64,112,256,3,6,18,PROG,Vpos,Hpos},      // 1360X768@60
    {1366,768,1500,800, 72000L, 0x30, 14,56,64,1,3,28,PROG,Vpos,Hpos} , // 1366x768@60-R, HReq = 48KHz
    {1366,768,1792,798, 85500L, 0x29, 70, 143, 213,3,3,24,PROG,Vpos,Hpos} , // 1366x768@60, HReq = 47.712KHz

    {1400,1050,1560,1080,101000L,0x22,48,32,80,3,4,23,PROG,Vneg,Hpos},     // 1400x768@60-R
    {1400,1050,1864,1089,121750L,0x1C,88,144,232,3,4,32,PROG,Vpos,Hneg},   // 1400x768@60
    {1400,1050,1896,1099,156000L,0x16,104,144,248,3,4,42,PROG,Vpos,Hneg},  // 1400x1050@75
    {1400,1050,1912,1105,179500L,0x13,104,152,256,3,4,48,PROG,Vpos,Hneg},  // 1400x1050@85
    {1440,900,1600,926,88750L,0x26,48,32,80,3,6,17,PROG,Vneg,Hpos},        // 1440x900@60-R
    {1440,900,1904,934,106500L,0x20,80,152,232,3,6,25,PROG,Vpos,Hneg},     // 1440x900@60
    {1440,900,1936,942,136750L,0x19,96,152,248,3,6,33,PROG,Vpos,Hneg},     // 1440x900@75
    {1440,900,1952,948,157000L,0x16,104,152,256,3,6,39,PROG,Vpos,Hneg},    // 1440x900@85
    {1600,1200,2160,1250,162000L,0x15,64,192,304,1,3,46,PROG,Vpos,Hpos},   // 1600x1200@60
    {1600,1200,2160,1250,175500L,0x13,64,192,304,1,3,46,PROG,Vpos,Hpos},   // 1600x1200@65
    {1600,1200,2160,1250,189000L,0x12,64,192,304,1,3,46,PROG,Vpos,Hpos},   // 1600x1200@70
    {1600,1200,2160,1250,202500L,0x11,64,192,304,1,3,46,PROG,Vpos,Hpos},   // 1600x1200@75
    {1600,1200,2160,1250,229500L,0x0F,64,192,304,1,3,46,PROG,Vpos,Hpos},   // 1600x1200@85
    {1680,1050,1840,1080,119000L,0x1D,48,32,80,3,6,21,PROG,Vneg,Hpos},     // 1680x1050@60-R
    {1680,1050,2240,1089,146250L,0x17,104,176,280,3,6,30,PROG,Vpos,Hneg},  // 1680x1050@60
    {1680,1050,2272,1099,187000L,0x12,120,176,296,3,6,40,PROG,Vpos,Hneg},  // 1680x1050@75
    {1680,1050,2288,1105,214750L,0x10,128,176,304,3,6,46,PROG,Vpos,Hneg},  // 1680x1050@85
    {1792,1344,2448,1394,204750L,0x10,128,200,328,1,3,46,PROG,Vpos,Hneg},  // 1792x1344@60
    {1792,1344,2456,1417,261000L,0x0D,96,216,352,1,3,69,PROG,Vpos,Hneg},   // 1792x1344@75
    {1856,1392,2528,1439,218250L,0x0F,96,224,352,1,3,43,PROG,Vpos,Hneg},   // 1856x1392@60
    {1856,1392,2560,1500,288000L,0x0C,128,224,352,1,3,104,PROG,Vpos,Hneg}, // 1856x1392@75
    {1920,1200,2080,1235,154000L,0x16,48,32,80,3,6,26,PROG,Vneg,Hpos},     // 1920x1200@60-R
    {1920,1200,2592,1245,193250L,0x11,136,200,336,3,6,36,PROG,Vpos,Hneg},  // 1920x1200@60
    {1920,1200,2608,1255,245250L,0x0E,136,208,344,3,6,46,PROG,Vpos,Hneg},  // 1920x1200@75
    {1920,1200,2624,1262,281250L,0x0C,144,208,352,3,6,53,PROG,Vpos,Hneg},  // 1920x1200@85
    {1920,1440,2600,1500,234000L,0x0E,128,208,344,1,3,56,PROG,Vpos,Hneg},  // 1920x1440@60
    {1920,1440,2640,1500,297000L,0x0B,144,224,352,1,3,56,PROG,Vpos,Hneg},  // 1920x1440@75
};

#define     SizeofVMTable   (sizeof(s_VMTable)/sizeof(HDMI_VTiming))
#else
#define     SizeofVMTable    0
#endif

extern const BYTE bCSCOffset_16_235[] ;
extern const BYTE bCSCOffset_0_255[] ;
#if (defined(SUPPORT_OUTPUTYUV))&&(defined(SUPPORT_INPUTRGB))
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU601_16_235[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU601_0_255[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU709_16_235[] ;
    extern _CODE BYTE bCSCMtx_RGB2YUV_ITU709_0_255[] ;
#endif

#if (defined(SUPPORT_OUTPUTRGB))&&(defined(SUPPORT_INPUTYUV))
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU601_16_235[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU601_0_255[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU709_16_235[] ;
    extern _CODE BYTE bCSCMtx_YUV2RGB_ITU709_0_255[] ;

#endif


static BYTE ucCurrentHDMIPort = 0 ;
static BOOL AcceptCDRReset = TRUE ;

///////////////////////////////////////////////////////////
// Function Prototype
///////////////////////////////////////////////////////////

BOOL CheckHDMIRX(void);

void Timer_Handler(void);
void Video_Handler(void);

static void HWReset_HDMIRX(void);
static void SWReset_HDMIRX(void);
void Terminator_Off(void);
void Terminator_On(void);

void Check_RDROM(void);
void RDROM_Reset(void);
void SetDefaultRegisterValue(void);
static void LoadCustomizeDefaultSetting(void);

static void ClearIntFlags(BYTE flag);
static void ClearHDCPIntFlags(void);
BOOL IsSCDT(void);
BOOL CheckPlg5VPwr(void);
void SetMUTE(BYTE AndMask, BYTE OrMask);
void SetIntMask1(BYTE AndMask,BYTE OrMask);
void SetIntMask2(BYTE AndMask,BYTE OrMask);
void SetIntMask3(BYTE AndMask,BYTE OrMask);
void SetIntMask4(BYTE AndMask,BYTE OrMask);
BOOL IsHDMIRXHDMIMode(void);


///////////////////////////////////////////////////////////
// Function Prototype
///////////////////////////////////////////////////////////
void RXINT_5V_PwrOn(void);
void RXINT_5V_PwrOff(void);
void RXINT_SCDT_On(void);
void RXINT_SCDT_Off(void);
void RXINT_RXCKON(void);
void RXINT_VideoMode_Chg(void);
void RXINT_HDMIMode_Chg(void);
void RXINT_AVMute_Set(void);
void RXINT_AVMute_Clear(void);
void RXINT_SetNewAVIInfo(void);
void RXINT_ResetHDCP(void);
void RXINT_CheckVendorSpecInfo(void) ;


static void VideoTimerHandler(void);
static void MuteProcessTimerHandler(void);

void AssignVideoTimerTimeout(WORD TimeOut);
void SwitchVideoState(Video_State_Type state);

#define EnableMuteProcessTimer()		{ MuteResumingTimer = MuteByPKG?MUTE_RESUMING_TIMEOUT:0 ; }
#define DisableMuteProcessTimer()		{ MuteResumingTimer = 0 ; }

static void DumpSyncInfo(HDMI_VTiming *pVTiming);


static void SetVideoInputFormatWithoutInfoFrame(BYTE bInMode);
static void SetColorimetryByMode(/* PSYNC_INFO pSyncInfo */);
void SetVideoInputFormatWithInfoFrame(void);
BOOL SetColorimetryByInfoFrame(void);
void SetColorSpaceConvert(void);
void SetDVIVideoOutput(void);
void SetNewInfoVideoOutput(void);
static void SetVideoMute(BOOL bMute);
static void SetALLMute(void) ;

///////////////////////////////////////////////////////////
// Connection Interface
///////////////////////////////////////////////////////////

BYTE HDRX_GetVideoSystem(void)
{
	return 0;
}

static void PollPortSwitch(void)
{
#if 0
	BYTE bNewPortSelection;

	bNewPortSelection = HAL_GetPortSelectionDebounced();

	if (bNewPortSelection != PORT_SELECT__NO_CHANGE)
	{
		CurrentStatus.PortSelection = bNewPortSelection;
		SystemDataReset();
	}
#endif
}

void Check_HDMInterrupt()
{
	//Interrupt_Handler();
}

void HDRX_VideoTask(void)
{
	PollPortSwitch();
	PollInterrupt();
}

BOOL CheckHDMIRX()
{
    Timer_Handler();
    Video_Handler();

    if(VState == VSTATE_VideoOn &&(!MuteByPKG))
    {
        return TRUE ;
    }

    return FALSE ;
}

BYTE GetCurrentHDMIPort(void)
{
	return ucCurrentHDMIPort ;
}

void HDRX_EnableOutVideo(BOOL OnOff)
{
	if (OnOff == ON)
	{
		//HDRX_RegBitValue(REG__SYS_CTRL1, BIT__PD, SET);
		TurnVideoMute(OFF);
	}
	else
	{
		TurnVideoMute(ON);
		//HDRX_RegBitValue(REG__SYS_CTRL1, BIT__PD, CLEAR);
	}
}


void HDRX_Init(void)
{
    BYTE uc ;

    Terminator_Off();
    //////////////////////////////////////////////
    // Initialize HDMIRX chip uc.
    //////////////////////////////////////////////
    HDRX_RegWrite(REG_RX_PWD_CTRL0, 0);
#if 0
    // this reset will activate the I2C no ACK in this call.
    HDMIRX_WriteI2C_Byte(REG_RX_RST_CTRL, B_REGRST); // register reset
#endif
   // MDINDLY_mSec(1);
    HDRX_RegWrite(REG_RX_RST_CTRL, B_SWRST|B_CDRRST|B_EN_AUTOVDORST); // sw reset
    MDINDLY_mSec(1);
    ucRevision = HDRX_RegRead(0x04);
    //// uc = 0x89 ; // for external ROM
    //uc = B_EXTROM | B_HDCP_ROMDISWR | B_HDCP_EN ;
    //HDRX_RegWrite(REG_RX_HDCP_CTRL, uc);

	if(ucCurrentHDMIPort==CAT_HDMI_PORTA)
		uc = B_PORT_SEL_A|B_PWD_AFEALL|B_PWDC_ETC ;
	else
		uc = B_PORT_SEL_B|B_PWD_AFEALL|B_PWDC_ETC ;

	HDRX_RegWrite(REG_RX_PWD_CTRL1, uc);

    SetIntMask1(0,B_PWR5VON|B_SCDTON|B_PWR5VOFF|B_SCDTOFF);
    SetIntMask2(0,B_NEW_AVI_PKG|B_PKT_SET_MUTE|B_PKT_CLR_MUTE);
    SetIntMask3(0,B_ECCERR|B_R_AUTH_DONE|B_R_AUTH_START);
    SetIntMask4(0,0) ; // B_M_RXCKON_DET);

    SetDefaultRegisterValue();
    LoadCustomizeDefaultSetting();

    SetALLMute(); // MUTE ALL with tristate video, SPDIF and all I2S channel

    HDRX_RegWrite(REG_RX_RST_CTRL, B_EN_AUTOVDORST); // normal operation
    bDisableAutoAVMute = FALSE ;
    if(ucRevision == 0xA2)
    {
        HDRX_RegWrite(REG_RX_HDCP_CTRL, 0x09);
        HDRX_RegWrite(REG_RX_HDCP_CTRL, 0x19);
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // DO NOT MOVE THE ACTION LOCATION!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // this delay is the experience by previous project support compatibility issue.
    MDINDLY_mSec(300); // delay 0.3 sec by TPV project experience.

    RDROM_Reset(); // it should be do SWRESET again AFTER RDROM_Reset().
    HDRX_RegWrite(REG_RX_RST_CTRL, B_SWRST|B_EN_AUTOVDORST); // sw reset
    MDINDLY_mSec(1);
    SetALLMute();
    HDRX_RegWrite(REG_RX_RST_CTRL, B_EN_AUTOVDORST); // normal operation

#ifndef MANUAL_TURNON_HDMIRX
    Terminator_Off();
    SwitchVideoState(VSTATE_SWReset);
#endif
	HDRX_RegWrite(REG_RX_CDEPTH_CTRL,HDRX_RegRead(REG_RX_CDEPTH_CTRL)&0xF);

    AcceptCDRReset = TRUE;
}

void Turn_HDMIRX(BOOL bEnable)
{
//    printf("Turn_HDMIRX(%s)\r\n",bEnable?"ON":"OFF");
    if( bEnable )
    {
        SWReset_HDMIRX() ;
    }
    else
    {
        Terminator_Off();
        SwitchVideoState(VSTATE_Off);
    }
}

void PowerDownHDMI(void)
{
	HDRX_RegWrite(REG_RX_PWD_CTRL1, B_PWD_AFEALL|B_PWDC_ETC|B_PWDC_SRV|B_EN_AUTOPWD);
	HDRX_RegWrite(REG_RX_PWD_CTRL0, B_PWD_ALL);
}

BOOL IsHDMIRXInterlace(void)
{
    if(HDRX_RegRead(REG_RX_VID_MODE)&B_INTERLACE)
    {
        return TRUE ;
    }
    return FALSE ;
}

BYTE getHDMIRX_InputColor(void)
{
    BYTE uc ;
    if(IsHDMIRXHDMIMode())
    {
        uc = HDRX_RegRead(REG_RX_AVI_DB1) & 0x60 ;

        switch(uc)
        {
        	case 0x40: return 2 ;
        	case 0x20: return 1 ;
        	default: return 0 ;
        }
    }
    return 0 ;

}

WORD getHDMIRXHorzTotal(void)
{
    BYTE uc[2] ;
	WORD hTotal ;

	uc[0] = HDRX_RegRead(REG_RX_VID_HTOTAL_L);
	uc[1] = HDRX_RegRead(REG_RX_VID_HTOTAL_H);
	hTotal =(WORD)(uc [1] & M_HTOTAL_H);
	hTotal <<= 8 ;
	hTotal |=(WORD)uc[0] ;

	return hTotal ;
}

WORD getHDMIRXHorzActive(void)
{
    BYTE uc[3] ;

	WORD hTotal, hActive ;

	uc[0] = HDRX_RegRead(REG_RX_VID_HTOTAL_L);
	uc[1] = HDRX_RegRead(REG_RX_VID_HTOTAL_H);
	uc[2] = HDRX_RegRead(REG_RX_VID_HACT_L);

	hTotal =(WORD)(uc [1] & M_HTOTAL_H);
	hTotal <<= 8 ;
	hTotal |=(WORD)uc[0] ;

	hActive =(WORD)(uc[1] >> O_HACT_H)& M_HACT_H ;
	hActive <<= 8 ;
	hActive |=(WORD)uc[2] ;

	if((hActive |(1<<11))< hTotal)
	{
		hActive |= 1<<11 ;
	}

	return hActive ;

}

WORD getHDMIRXHorzFrontPorch(void)
{
    BYTE uc[2] ;
	WORD hFrontPorch ;

	uc[0] = HDRX_RegRead(REG_RX_VID_H_FT_PORCH_L);
	uc[1] =(HDRX_RegRead(REG_RX_VID_HSYNC_WID_H)>> O_H_FT_PORCH)& M_H_FT_PORCH ;
	hFrontPorch =(WORD)uc[1] ;
	hFrontPorch <<= 8 ;
	hFrontPorch |=(WORD)uc[0] ;

	return hFrontPorch ;
}

WORD getHDMIRXHorzSyncWidth(void)
{
    BYTE uc[2] ;
	WORD hSyncWidth ;

	uc[0] = HDRX_RegRead(REG_RX_VID_HSYNC_WID_L);
	uc[1] = HDRX_RegRead(REG_RX_VID_HSYNC_WID_H)& M_HSYNC_WID_H ;

	hSyncWidth =(WORD)uc[1] ;
	hSyncWidth <<= 8 ;
	hSyncWidth |=(WORD)uc[0] ;

	return hSyncWidth ;
}

WORD getHDMIRXHorzBackPorch(void)
{
	WORD hBackPorch ;

	hBackPorch = getHDMIRXHorzTotal()- getHDMIRXHorzActive()- getHDMIRXHorzFrontPorch()- getHDMIRXHorzSyncWidth();

	return hBackPorch ;
}

WORD getHDMIRXVertTotal(void)
{
    BYTE uc[3] ;
	WORD vTotal, vActive ;
	uc[0] = HDRX_RegRead(REG_RX_VID_VTOTAL_L);
	uc[1] = HDRX_RegRead(REG_RX_VID_VTOTAL_H);
	uc[2] = HDRX_RegRead(REG_RX_VID_VACT_L);

	vTotal =(WORD)uc[1] & M_VTOTAL_H ;
	vTotal <<= 8 ;
	vTotal |=(WORD)uc[0] ;

	vActive =(WORD)(uc[1] >> O_VACT_H)& M_VACT_H ;
	vActive |=(WORD)uc[2] ;

	if(vTotal >(vActive |(1<<10)))
	{
		vActive |= 1<<10 ;
	}

	// for vertical front porch bit lost, ...
	#if 0
	if(vActive == 600 && vTotal == 634)
	{
		vTotal = 666 ; // fix the 800x600@72 issue
	}
	#endif

	return vTotal ;
}

WORD getHDMIRXVertActive(void)
{
    BYTE uc[3] ;
	WORD vTotal, vActive;

	uc[0] = HDRX_RegRead(REG_RX_VID_VTOTAL_L);
	uc[1] = HDRX_RegRead(REG_RX_VID_VTOTAL_H);
	uc[2] = HDRX_RegRead(REG_RX_VID_VACT_L);

	vTotal =(WORD)uc[1] & M_VTOTAL_H ;
	vTotal <<= 8 ;
	vTotal |=(WORD)uc[0] ;

	vActive =(WORD)(uc[1] >> O_VACT_H)& M_VACT_H ;
	vActive <<= 8 ;
	vActive |=(WORD)uc[2] ;

	if(vTotal >(vActive |(1<<10)))
	{
		vActive |= 1<<10 ;
	}

	return vActive ;
}

WORD getHDMIRXVertFrontPorch(void)
{
    WORD vFrontPorch ;

	vFrontPorch =(WORD)HDRX_RegRead(REG_RX_VID_V_FT_PORCH)& 0xF ;

	if(getHDMIRXVertActive()== 600 && getHDMIRXVertTotal()== 666)
	{
		vFrontPorch |= 0x20 ;
	}

	return vFrontPorch ;

}

WORD getHDMIRXVertSyncToDE(void)
{
    WORD vSync2DE ;

    vSync2DE =(WORD)HDRX_RegRead(REG_RX_VID_VSYNC2DE);
    return vSync2DE ;
}

WORD getHDMIRXVertSyncWidth(void)
{
    WORD vSync2DE ;
    WORD vTotal, vActive, hActive  ;

    vSync2DE = getHDMIRXVertSyncToDE();
    vTotal = getHDMIRXVertTotal();
    vActive = getHDMIRXVertActive();
    hActive = getHDMIRXHorzActive();
#ifndef HDMIRX_A1
    // estamite value.
    if(vActive < 300)  	return 3 ;

    if(hActive == 640 && hActive == 480)
    {
    	if(HDRX_RegRead(REG_RX_VID_XTALCNT_128PEL)< 0x80) 	return 3 ;
    	return 2;
    }

    return 5 ;
#endif
}

WORD getHDMIRXVertSyncBackPorch(void)
{
    WORD vBackPorch ;

    vBackPorch = getHDMIRXVertSyncToDE()- getHDMIRXVertSyncWidth();
    return vBackPorch ;
}

BYTE getHDMIRXxCnt(void)
{
    return HDRX_RegRead(REG_RX_VID_XTALCNT_128PEL);
}

///////////////////////////////////////////////////////////
// Get Info Frame and HDMI Package
// Need upper program pass information and read them.
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//  Testing Function
///////////////////////////////////////////////////////////

void getHDMIRXRegs(BYTE *pData)
{
    int i, j ;

    SwitchHDMIRXBank(0);
    for(i = j = 0 ; i < 256 ; i++,j++)
    {
        pData[j] = HDRX_RegRead((BYTE)(i&0xFF));
    }
    SwitchHDMIRXBank(1);
    for(i = 0xA0 ; i <= 0xF2 ; i++, j++)
    {
        pData[j] = HDRX_RegRead((BYTE)(i&0xFF));
    }
    SwitchHDMIRXBank(0);
}

BYTE getHDMIRXOutputColorMode()
{
    return bOutputVideoMode & F_MODE_CLRMOD_MASK ;
}

BYTE getHDMIRXOutputColorDepth()
{
    BYTE uc ;

    uc = HDRX_RegRead(REG_RX_FS)& M_GCP_CD ;
    return uc >> O_GCP_CD ;
}

// Initialization
///////////////////////////////////////////////////////////

static void HWReset_HDMIRX(void)
{
    // reset HW Reset Pin.
    // Write HDMIRX pin = 1 ;
}


void Terminator_Off(void)
{
    BYTE uc ;
    uc = HDRX_RegRead(REG_RX_PWD_CTRL1)|(B_PWD_AFEALL|B_PWDC_ETC);
    HDRX_RegWrite(REG_RX_PWD_CTRL1, uc);
  //  printf("Terminator_Off, reg07 = %02x\r\n",uc);
}

void Terminator_On(void)
{
    BYTE uc ;
    uc = HDRX_RegRead(REG_RX_PWD_CTRL1)& ~(B_PWD_AFEALL|B_PWDC_ETC);
    HDRX_RegWrite(REG_RX_PWD_CTRL1, uc);
 //   printf("Terminator_On, reg07 = %02x\r\n",uc);
}

/*
static void Terminator_Reset(void)
{
    Terminator_Off();
    MDINDLY_mSec(500); // delay 500 ms
    Terminator_On();
}
*/

void RDROM_Reset(void)
{
    BYTE i ;
    BYTE uc ;

  //  printf("RDROM_Reset()\r\n");
    // uc =((bDisableAutoAVMute)?B_VDO_MUTE_DISABLE:0)|1;
    uc = HDRX_RegRead(REG_RX_RDROM_CLKCTRL)& ~(B_ROM_CLK_SEL_REG|B_ROM_CLK_VALUE);
    for(i=0 ;i < 16 ; i++)
    {
        HDRX_RegWrite(REG_RX_RDROM_CLKCTRL, B_ROM_CLK_SEL_REG|uc);
        HDRX_RegWrite(REG_RX_RDROM_CLKCTRL, B_ROM_CLK_SEL_REG|B_ROM_CLK_VALUE|uc);
    }
    // 2006/10/31 modified by jjtseng
    // added oring bDisableAutoAVMute
    HDRX_RegWrite(REG_RX_RDROM_CLKCTRL,uc);
    //~jjtseng 2006/10/31
}

void Check_RDROM(void)
{
    BYTE uc ;
//    printf("Check_HDCP_RDROM()\r\n");

    HDRX_RegWrite(REG_RX_RST_CTRL, B_EN_AUTOVDORST);

    if(IsSCDT())
    {
        // int count ;
        // for(count = 0 ;; count++)
		{
            uc = HDRX_RegRead(REG_RX_RDROM_STATUS);
            if((uc & 0xF)!= 0x9)
            {
                RDROM_Reset();
            }
  //          printf("Check_HDCP_RDROM()done.\r\n");
            return ;
        }
    }
}

static void SWReset_HDMIRX(void)
{
    Check_RDROM();
    HDRX_RegWrite(REG_RX_RST_CTRL, B_SWRST|B_EN_AUTOVDORST); // sw reset
    MDINDLY_mSec(1);
    SetALLMute();
    HDRX_RegWrite(REG_RX_RST_CTRL, B_EN_AUTOVDORST); // normal operation

    // Terminator_Reset();

    // 2006/10/26 modified by jjtseng
    // SwitchVideoState(VSTATE_SyncWait);
    // SwitchVideoState(VSTATE_PwrOff);
    //~jjtseng 2006/10/26

    Terminator_Off();
    SwitchVideoState(VSTATE_SWReset);


    //2008/10/02 modified by hermes
    SCDTErrorCnt = 0;

}

// 2006/10/31 added by jjtseng
// for customized uc
typedef struct _REGPAIR {
    BYTE ucAddr ;
    BYTE ucValue ;
} REGPAIR ;
//~jjtseng 2006/10/31

/////////////////////////////////////////////////////////////////
// Customer Defined uc area.
/////////////////////////////////////////////////////////////////
// 2006/10/31 added by jjtseng
// for customized uc
static REGPAIR const acCustomizeValue[] =
{
    //2009/12/08 added by jau-chih.tseng@ite.com.tw
    // {REG_RX_VCLK_CTRL, 0x20}, // request by Clive for adjusting A2 version.
    {REG_RX_VCLK_CTRL, 0x30}, // request by Clive for IT6603 board .... !@#$!@$#! .
    //~jau-chih.tseng 2009/12/08
    // {REG_RX_I2S_CTRL,0x61},
    {REG_RX_I2S_CTRL,I2S_DSP_SETTING},
    // CCIR656
    {REG_RX_PG_CTRL2,HDMIRX_OUTPUT_COLORMODE},
    {REG_RX_VIDEO_MAP,HDMIRX_OUTPUT_MAPPING},
    {REG_RX_VIDEO_CTRL1,HDMIRX_OUTPUT_TYPE},
    {REG_RX_MCLK_CTRL, 0xC1},
    {0xFF,0xFF}
} ;
// jjtseng 2006/10/31

static void LoadCustomizeDefaultSetting(void)
{
    BYTE i, uc ;
    for(i = 0 ; acCustomizeValue[i].ucAddr != 0xFF ; i++)
    {
    	HDRX_RegWrite(acCustomizeValue[i].ucAddr,acCustomizeValue[i].ucValue);
    }

    /*
    uc = HDRX_RegRead(REG_RX_PG_CTRL2)& ~(M_OUTPUT_COLOR_MASK<<O_OUTPUT_COLOR_MODE);
    switch(bOutputVideoMode&F_MODE_CLRMOD_MASK)
    {
    case F_MODE_YUV444:
        uc |= B_OUTPUT_YUV444 << O_OUTPUT_COLOR_MODE ;
        break ;
    case F_MODE_YUV422:
        uc |= B_OUTPUT_YUV422 << O_OUTPUT_COLOR_MODE ;
        break ;
    }
    HDRX_RegWrite(REG_RX_PG_CTRL2, uc);
    */
    bOutputVideoMode&=~F_MODE_CLRMOD_MASK;
    uc = HDRX_RegRead(REG_RX_PG_CTRL2)&(M_OUTPUT_COLOR_MASK<<O_OUTPUT_COLOR_MODE);

    switch(uc)
    {
    	case (B_OUTPUT_YUV444 << O_OUTPUT_COLOR_MODE):
    			bOutputVideoMode |= F_MODE_YUV444;
    	break ;
    	case (B_OUTPUT_YUV422 << O_OUTPUT_COLOR_MODE):
    			bOutputVideoMode |= F_MODE_YUV422;
    	break ;
    	case	0:
    			bOutputVideoMode |= F_MODE_RGB444;
    	break ;
    	default:
    			bOutputVideoMode|=F_MODE_RGB444;
    	break ;
    }
    bIntPOL =(HDRX_RegRead(REG_RX_INTERRUPT_CTRL)& B_INTPOL)?LO_ACTIVE:HI_ACTIVE ;
}

//////////////////////////////////////////////////
// SetDefaultRegisterValue
// some register value have to be hard coded and
// need to adjust by case. Set here.
//////////////////////////////////////////////////
//  There are some register default setting has changed, please make sure
// when release to customer.
///////////////////////////////////////////////////
// Rev 1.09
///////////////////////////////////////////////////
//reg3B=0x40
//reg68=0x03
//reg69=0x00//HW DEF
//reg6A=0xA8//HW DEF
//reg6B=0x11
//Reg6C=0x00
//reg6D=0x64//HW DEF
//reg6E=0x0C//HW DEF
//Reg93=0x43
//Reg94=0x4F
//Reg95=0x87
//Reg96=0x33
///////////////////////////////////////////////////

// 2006/10/31 added by jjtseng
static REGPAIR const acDefaultValue[] =
{
    {0x0F,0x00},// Reg08
	// 2010/10/13 modified by jau-chih.tseng@ite.com.tw
	// recommand by Clive to modify the driving from 0xAE to 0xCE
    // {REG_RX_VIO_CTRL,0xAE},// Reg08
    {REG_RX_VIO_CTRL,0xCE},// Reg08
	//~jau-chih.tseng@ite.com.tw
    //~jj_tseng@chipadvanced.com
    // 2008/09/25 added by jj_tseng@chiopadvanced.com
    // by IT's command, reg3B should set default value with 0x40
    {REG_RX_DESKEW_CTRL, 0x40},
    //~jj_tseng@chipadvanced.com 2008/09/25
    {REG_RX_PLL_CTRL,0x03},// Reg68=0x03
    // {REG_RX_TERM_CTRL1,0x00},// Reg69=0x00 // HW Default


	{REG_RX_EQUAL_CTRL1,0x11}, 	// reg6B = 0x11
    {REG_RX_EQUAL_CTRL2, 0x00}, // reg6C = 0x00
    // reg6D = HW Default
    // {REG_RX_DES_CTRL1, 0x64},
    // reg6E = HW Default
    // {REG_RX_DES_CTRL2, 0x0C}, // CDR Auto Reset, only CDR

    {0x93,0x43},
    {0x94,0x4F},
    {0x95,0x87},
    {0x96,0x33},

    // {0x9B, 0x01},
//20100928 added by jau-chih.tseng@ite.com.tw
    {0x56, 0x01},
    {0x97, 0x0E},
//~20100928 jau-chih.tseng@ite.com.tw
// 2011/08/10 modified by jau-chih.tseng@ite.com.tw
    {REG_RX_CSC_CTRL,B_VIO_SEL},
    {REG_RX_TRISTATE_CTRL,0x5F}, // set Tri-state
//~jau-chih.tseng@ite.com.tw 2011/08/10
    {0xFF,0xFF}


} ;
//~jjtseng 2006/10/31

void SetDefaultRegisterValue(void)
{
    BYTE i ;

    for(i = 0 ; acDefaultValue[i].ucAddr != 0xFF ; i++)
    {
    	HDRX_RegWrite(acDefaultValue[i].ucAddr, acDefaultValue[i].ucValue);
    }

    if(ucRevision >= 0xA3)
    {
    	HDRX_RegWrite(0x9B, 0x01);
    }
}

///////////////////////////////////////////////////////////
// Basic IO
///////////////////////////////////////////////////////////




static void ClearIntFlags(BYTE flag)
{
    BYTE uc ;
    uc = HDRX_RegRead(REG_RX_INTERRUPT_CTRL);
    uc &= FLAG_CLEAR_INT_MASK ;
    uc |= flag ;
    HDRX_RegWrite(REG_RX_INTERRUPT_CTRL,uc);
    MDINDLY_mSec(1);
    uc &= FLAG_CLEAR_INT_MASK ;
    HDRX_RegWrite(REG_RX_INTERRUPT_CTRL,uc);  // write 1, then write 0, the corresponded clear action is activated.
    MDINDLY_mSec(1);
    // printf("ClearIntFlags with %02X\r\n",uc);
}

static void ClearHDCPIntFlags(void)
{
    BYTE uc ;

    uc = HDRX_RegRead(REG_RX_INTERRUPT_CTRL1);
    HDRX_RegWrite(REG_RX_INTERRUPT_CTRL1,(BYTE)B_CLR_HDCP_INT|uc);
    MDINDLY_mSec(1);
    HDRX_RegWrite(REG_RX_INTERRUPT_CTRL1, uc&((BYTE)~B_CLR_HDCP_INT));
}

///////////////////////////////////////////////////
// IsSCDT()
// return TRUE if SCDT ON
// return FALSE if SCDT OFF
///////////////////////////////////////////////////

BOOL IsSCDT(void)
{
    BYTE uc ;

    uc = HDRX_RegRead(REG_RX_SYS_STATE)&(B_SCDT|B_VCLK_DET/*|B_PWR5V_DET*/);
    return(uc==(B_SCDT|B_VCLK_DET/*|B_PWR5V_DET*/))?TRUE:FALSE ;
}

BOOL CheckPlg5VPwr(void)
{
    BYTE uc ;

    // HDRX_RegRead(REG_RX_INTERRUPT1,&uc);
    uc = HDRX_RegRead(REG_RX_SYS_STATE);
    // printf("CheckPlg5VPwr(): REG_RX_SYS_STATE = %02X %s\r\n",(int)uc,(uc&B_PWR5V_DET)?"TRUE":"FALSE");

    if(ucCurrentHDMIPort == CAT_HDMI_PORTB)
    {
        return(uc&B_PWR5V_DET_PORTB)?TRUE:FALSE ;

    }

    return(uc&B_PWR5V_DET_PORTA)?TRUE:FALSE ;
}

void SetMUTE(BYTE AndMask, BYTE OrMask)
{
    BYTE uc = 0 ;

    if(AndMask)
    {
        uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDRX_RegWrite(REG_RX_TRISTATE_CTRL,uc);
    uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
}

void SetIntMask1(BYTE AndMask,BYTE OrMask)
{
    BYTE uc = 0;

    if(AndMask != 0)
    {
        uc = HDRX_RegRead(REG_RX_INTERRUPT_MASK1);
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDRX_RegWrite(REG_RX_INTERRUPT_MASK1, uc);
}

void SetIntMask2(BYTE AndMask,BYTE OrMask)
{
    BYTE uc = 0;
    if(AndMask != 0)
    {
        uc = HDRX_RegRead(REG_RX_INTERRUPT_MASK2);
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDRX_RegWrite(REG_RX_INTERRUPT_MASK2, uc);
}

void SetIntMask3(BYTE AndMask,BYTE OrMask)
{
    BYTE uc = 0;
    if(AndMask != 0)
    {
        uc = HDRX_RegRead(REG_RX_INTERRUPT_MASK3);
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDRX_RegWrite(REG_RX_INTERRUPT_MASK3, uc);
}

void SetIntMask4(BYTE AndMask,BYTE OrMask)
{
    BYTE uc = 0;
    if(AndMask != 0)
    {
        uc = HDRX_RegRead(REG_RX_INTERRUPT_MASK4);
    }
    uc &= AndMask ;
    uc |= OrMask ;
    HDRX_RegWrite(REG_RX_INTERRUPT_MASK4, uc);
}

BOOL IsHDMIRXHDMIMode(void)
{
    BYTE uc ;
    uc = HDRX_RegRead(REG_RX_SYS_STATE);
    // printf(("IsHDMIRXHDMIMode(): read %02x from reg%02x, result is %s\n",
    //(int)uc,(int)REG_RX_SYS_STATE,(uc&B_HDMIRX_MODE)?"TRUE":"FALSE"));
    return (uc&B_HDMIRX_MODE)?TRUE:FALSE ;
}

BOOL IsHDCPOn(void)
{
    BYTE uc,stat ;
    uc = HDRX_RegRead(REG_RX_HDCP_STATUS);
    stat = HDRX_RegRead(REG_RX_VID_INPUT_ST) ;
//    printf(("reg12 = %02X reg65 = %02X\n",(int)uc,(int)stat)) ;
    if((uc & 1 )&&(!(stat&8)))
    {
        return TRUE ;
    }

    return FALSE ;
}

BOOL IsHDMIRX_VideoReady(void)
{
    if(VState == VSTATE_VideoOn &&(!MuteByPKG))
    {
        return TRUE ;
    }

    return FALSE ;
}

BOOL IsHDMIRX_VideoOn(void)
{
    return (VState == VSTATE_VideoOn)?TRUE:FALSE ;
}


BOOL EnableHDMIRXVideoOutput(BYTE Option)
{
    switch(Option)
    {
    	case VIDEO_ON:
    		bVideoOutputOption = VIDEO_ON ;
    		SetMUTE(~B_TRI_VIDEO,0) ;
    		break ;
    	case VIDEO_OFF:
    		bVideoOutputOption = VIDEO_OFF ;
    		SetMUTE(~B_TRI_VIDEO,B_TRI_VIDEO) ;
    		break ;
    	case VIDEO_AUTO :
    	default:
    		bVideoOutputOption = VIDEO_AUTO ;
    		SetMUTE(~B_TRI_VIDEO,(VState == VSTATE_VideoOn) ? 0 : B_TRI_VIDEO) ;
    		break ;
    }

    return FALSE ;
}
///////////////////////////////////////////////////////////
// Interrupt Service
///////////////////////////////////////////////////////////
#if 0
void Interrupt_Handler(void)
{
	BYTE int1data = 0 ;
	BYTE int2data = 0 ;
	BYTE int3data = 0 ;
	BYTE int4data = 0 ;
	BYTE sys_state ;
	BYTE flag = FLAG_CLEAR_INT_ALL;

    // ClearIntFlags(0);
    if(VState == VSTATE_SWReset || VState == VSTATE_Off )
    {
    	return ; // if SWReset, ignore all interrupt.
    }

    sys_state = HDRX_RegRead(REG_RX_SYS_STATE);


    // int4data = HDRX_RegRead(REG_RX_INTERRUPT4);
    int1data = HDRX_RegRead(REG_RX_INTERRUPT1);

    if(int1data /*||(int4data&B_RXCKON_DET) */)
    {
     //   printf("system state = %02X\r\n",sys_state);
      //  printf("Interrupt 1 = %02X\r\n",int1data);
        //printf("Interrupt 4 = %02X\r\n",int4data);
        ClearIntFlags(B_CLR_MODE_INT);

		if(!CheckPlg5VPwr())
		{
			if(VState != VSTATE_SWReset && VState != VSTATE_PwrOff)
			{
				SWReset_HDMIRX();
				return ;
			}
		}

        if(int1data & B_PWR5VOFF)
        {
      //      printf("5V Power Off interrupt\r\n");
            RXINT_5V_PwrOff();
        }

        if(VState == VSTATE_SWReset)
        {
        	return ;
        }

        if(int1data & B_SCDTOFF)
        {
       //     printf("SCDT Off interrupt\r\n");
            RXINT_SCDT_Off();
        }

        if(int1data & B_PWR5VON)
        {
       //     printf("5V Power On interrupt\r\n");
            RXINT_5V_PwrOn();
        }


        if(VState == VSTATE_SyncWait)
        {

            if(int1data & B_SCDTON)
            {
       //         printf("SCDT On interrupt\r\n");
                RXINT_SCDT_On();
            }

            // if(int4data & B_RXCKON_DET)
            // {
            //     printf(("RXCKON DET interrupt\n"));
            //     RXINT_RXCKON();
            // }
        }

        if( VState == VSTATE_VideoOn || VState == VSTATE_HDCP_Reset)
        {
            if(int1data & B_HDMIMODE_CHG)
            {
      //          printf("HDMI Mode change interrupt.\r\n");
                RXINT_HDMIMode_Chg();
            }

            if(int1data & B_VIDMODE_CHG)
            {
     //           printf("Video mode change interrupt.\r\n");
                RXINT_VideoMode_Chg();
            }
        }

    }

    int2data = HDRX_RegRead(REG_RX_INTERRUPT2);
	if(VState == VSTATE_VideoOn || VState == VSTATE_HDCP_Reset)
	{
        int4data = HDRX_RegRead(REG_RX_INTERRUPT4);
	}
	else
	{
	    int4data = 0 ;
	}
    if(int2data||(int4data & B_GENPKT_DET))
    {
        BYTE vid_stat = HDRX_RegRead(REG_RX_VID_INPUT_ST);
   //     printf("Interrupt 2 = %02X\r\n",int2data);
        ClearIntFlags(B_CLR_PKT_INT|B_CLR_MUTECLR_INT|B_CLR_MUTESET_INT);

        if(int2data & B_PKT_SET_MUTE)
        {
    //        printf("AVMute set interrupt.\r\n");
            RXINT_AVMute_Set();
        }

        if(int2data & B_NEW_AVI_PKG)
        {
  //          printf("New AVI Info Frame Change interrupt\r\n");
            RXINT_SetNewAVIInfo();
        }

        if((int2data & B_PKT_CLR_MUTE))
        {
  //          printf("AVMute clear interrupt.\r\n");
            RXINT_AVMute_Clear();
        }

    	if(VState == VSTATE_VideoOn || VState == VSTATE_HDCP_Reset)
    	{

            if(int4data & B_GENPKT_DET)
            {
                RXINT_CheckVendorSpecInfo() ;
            }
    	}
    }

    int3data = HDRX_RegRead(REG_RX_INTERRUPT3);

    if(int3data &(B_R_AUTH_DONE|B_R_AUTH_START))
    {
        ClearHDCPIntFlags();
    }

	if(VState == VSTATE_VideoOn || VState == VSTATE_HDCP_Reset)
	{
	    // int3data = HDRX_RegRead(REG_RX_INTERRUPT3);

	    if(int3data & B_ECCERR)
	    {
         //   printf("int3 = %02X,ECC error interrupt\r\n",int3data);
            RXINT_ResetHDCP();
	    }
	}

    #ifdef __HRX_DBGPRT__
    if(int1data | int2data)
    {
        int1data = HDRX_RegRead(REG_RX_INTERRUPT1);
        int2data = HDRX_RegRead(REG_RX_INTERRUPT2);
        int3data = HDRX_RegRead(REG_RX_INTERRUPT3);
        sys_state = HDRX_RegRead(REG_RX_SYS_STATE);
  //      printf("%02x %02x %02x %02x %02x\r\n",
   //                             int1data,
    //                            int2data,
     //                           int3data,
      //                          HDRX_RegRead(REG_RX_INTERRUPT4),
       //                         sys_state);
    }
    #endif

}
#endif

void RXINT_CheckVendorSpecInfo(void)
{
	BYTE uc ;

    if(ucRevision >= 0xA3)
    {
        if(HDRX_RegRead(REG_RX_GENPKT_HB0)==(VENDORSPEC_INFOFRAME_TYPE|0x80))
        {
//            printf("Detecting a VENDORSPECIFIC_INFOFRAME\r\n") ;
			bVSIpresent = TRUE ;
            if((HDRX_RegRead(REG_RX_GENPKT_DB4)&0xE0)== 0x40)
            {
 //               printf("Detecting a FramePacking\r\n");
                uc = HDRX_RegRead(0x3C);
                uc |=(1<<2);
                HDRX_RegWrite(0x3C,uc);
            }
            uc = HDRX_RegRead(REG_RX_REGPKTFLAG_CTRL) ;
            uc &= ~B_INT_GENERAL_EVERY ;
            HDRX_RegWrite(REG_RX_REGPKTFLAG_CTRL, uc) ;
        }
        AssignVideoTimerTimeout(VIDEO_TIMER_CHECK_COUNT);
    }
}

void RXINT_5V_PwrOn(void)
{
    // BYTE sys_state ;

    if(VState == VSTATE_PwrOff)
    {
        // sys_state = HDRX_RegRead(REG_RX_SYS_STATE);

        // if(sys_state & B_PWR5VON)
        if(CheckPlg5VPwr())
        {
            SwitchVideoState(VSTATE_SyncWait);
        }
    }
}

void RXINT_5V_PwrOff(void)
{
    BYTE sys_state ;

    sys_state = HDRX_RegRead(REG_RX_SYS_STATE);

    SWReset_HDMIRX();
}

void RXINT_SCDT_On(void)
{
    if(VState == VSTATE_SyncWait)
    {
        if(IsSCDT())
        {
        #ifdef DISABLE_COLOR_DEPTH_RESET
            SwitchVideoState(VSTATE_SyncChecking);
        #else
			if( bDisableColorDepthResetState == FALSE )
            	SwitchVideoState(VSTATE_ColorDetectReset);
			else
            	SwitchVideoState(VSTATE_SyncChecking);
        #endif
        }
    }
}

//=============================================================================
// 1. Reg97[5] = '1'
// 2. Reg05[7][1] = '1' '1'
// 3. Reg73[3]  = '1'
//
// 4. Reg97[5] = '0'
// 5. REg05[7][1] = '0''0'
// 6. Reg73[3] = '0'
//=============================================================================





void RXINT_SCDT_Off(void)
{

    if(VState != VSTATE_PwrOff)
    {
        printf("GetSCDT OFF\r\n");
        SwitchVideoState(VSTATE_SyncWait);

        //2008/10/02 modified by hermes
        SCDTErrorCnt++;
    }
}

void RXINT_VideoMode_Chg(void)
{
    BYTE sys_state ;

    // CAT6023/IT6605 only detect video mode change while AVMute clear, thus
    // the first Video mode change after AVMUTE clear and video on should be
    // ignore.
 //   printf("RXINT_VideoMode_Chg\r\n");

    sys_state = HDRX_RegRead(REG_RX_SYS_STATE);
    // SetALLMute();

    if(CheckPlg5VPwr())
    {
        if( bIgnoreVideoChgEvent == FALSE )
        {
            SwitchVideoState(VSTATE_SyncWait);
        }
    }
    else
    {
        SWReset_HDMIRX();
    }
    bIgnoreVideoChgEvent = FALSE ;
}

void RXINT_HDMIMode_Chg(void)
{
    if(VState == VSTATE_VideoOn)
    {
        if(IsHDMIRXHDMIMode())
        {
            printf("HDMI Mode.\r\n");
            // wait for new AVIInfoFrame to switch color space.
        }
        else
        {
            printf("DVI Mode.\r\n");
            NewAVIInfoFrameF = FALSE ;

            // should switch input color mode to RGB24 mode.
            SetDVIVideoOutput();
            // No info frame active.
        }
    }
}

void RXINT_RXCKON(void)
{
}

void RXINT_AVMute_Set(void)
{
    BYTE uc ;
    MuteByPKG = ON ;
    // SetALLMute();
    SetVideoMute(ON);
    SetIntMask2(~(B_PKT_CLR_MUTE),(B_PKT_CLR_MUTE)); // enable the CLR MUTE interrupt.

    bDisableAutoAVMute = 0 ;
//     uc = HDRX_RegRead(REG_RX_RDROM_CLKCTRL);
    uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
    uc &= ~B_VDO_MUTE_DISABLE ;
//     HDRX_RegWrite(REG_RX_RDROM_CLKCTRL, uc);
    HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);
}

void RXINT_AVMute_Clear(void)
{
    BYTE uc ;
    MuteByPKG = OFF ;

    bDisableAutoAVMute = 0 ;
    // HDRX_RegWrite(REG_RX_RDROM_CLKCTRL, HDRX_RegRead(REG_RX_RDROM_CLKCTRL)&(~B_VDO_MUTE_DISABLE));
    uc =  HDRX_RegRead(REG_RX_TRISTATE_CTRL);
    uc &= ~B_VDO_MUTE_DISABLE ;
    HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);

    if(VState == VSTATE_VideoOn)
    {
        SetVideoMute(OFF);

    }

    SetIntMask2(~(B_PKT_CLR_MUTE),0); // clear the CLR MUTE interrupt.
}

void RXINT_SetNewAVIInfo(void)
{
    NewAVIInfoFrameF = TRUE ;

    if(VState == VSTATE_VideoOn)
    {
        SetNewInfoVideoOutput();
    }

    prevAVIDB1 = HDRX_RegRead(REG_RX_AVI_DB1);
    prevAVIDB2 = HDRX_RegRead(REG_RX_AVI_DB2);

}

void RXINT_ResetHDCP(void)
{
    BYTE uc ;

    if(VState == VSTATE_VideoOn)
    {
        ClearIntFlags(B_CLR_ECC_INT);
        MDINDLY_mSec(1);
        uc = HDRX_RegRead(REG_RX_INTERRUPT3);

        if(uc & B_ECCERR)
        {
    		SwitchVideoState(VSTATE_HDCP_Reset);
        }

        // HDCP_Reset();
        // SetVideoMute(MuteByPKG);
    }
}

///////////////////////////////////////////////////////////
// Timer Service
///////////////////////////////////////////////////////////

void Timer_Handler(void)
{
//	Interrupt_Handler();
    VideoTimerHandler();
    MuteProcessTimerHandler();
}

static void VideoTimerHandler(void)
{
	BYTE uc ;

    if( VState == VSTATE_Off )
    {
        return ;
    }

	if( VideoCountingTimer > 0 )
	{
	    VideoCountingTimer -- ;
	}

	//2008/10/02 modified by hermes
	if(SCDTErrorCnt>= SCDT_LOST_TIMEOUT)
    {
		SWReset_HDMIRX();
	}


	// monitor if no state
	if(VState == VSTATE_SWReset)
	{
		if(VideoCountingTimer==0)
		{
			Terminator_On();
			SwitchVideoState(VSTATE_PwrOff);
			return ;
		}

		return ;
	}

	if(VState == VSTATE_PwrOff)
	{
	    if(CheckPlg5VPwr())
	    {
            SwitchVideoState(VSTATE_SyncWait);
            return ;
	    }
	}

	if((VState != VSTATE_PwrOff)&&(VState != VSTATE_SyncWait)&&(VState != VSTATE_SWReset)&&(VState != VSTATE_ColorDetectReset))
	{
	    if(!IsSCDT())
	    {
            SwitchVideoState(VSTATE_SyncWait);
            return ;
	    }
	}
	else if((VState != VSTATE_PwrOff)&&(VState != VSTATE_SWReset))
	{
	    if(!CheckPlg5VPwr())
	    {
            // SwitchVideoState(VSTATE_PwrOff);
            SWReset_HDMIRX();
            return ;
	    }
	}

    // 2007/01/12 added by jjtseng
    // add the software reset timeout setting.
    if(VState == VSTATE_SyncWait || VState == VSTATE_SyncChecking)
    {
        SWResetTimeOut-- ;
        if(SWResetTimeOut == 0)
        {
            SWReset_HDMIRX();
            return ;
        }
    }
    //~jjtseng

    if(VState == VSTATE_SyncWait)
    {

        if(VideoCountingTimer == 0)
        {
            //printf(("VsyncWaitResetTimer up, call SWReset_HDMIRX()\n",VideoCountingTimer));
            SWReset_HDMIRX();
            return ;
            // AssignVideoTimerTimeout(VSTATE_MISS_SYNC_COUNT);
        }
        else
        {

    		uc=HDRX_RegRead(REG_RX_SYS_STATE);
            printf(("REG_RX_SYS_STATE = %x\r\n",(int)uc));
    		uc &=(B_RXPLL_LOCK|B_RXCK_VALID|B_SCDT|B_VCLK_DET);


    		if(uc ==(B_RXPLL_LOCK|B_RXCK_VALID|B_SCDT|B_VCLK_DET))// for check SCDT !!
            {
                #ifdef DISABLE_COLOR_DEPTH_RESET
                    SwitchVideoState(VSTATE_SyncChecking);
                #else
        		// 2011/10/17 added by jau-chih.tseng@ite.com.tw
        			if( bDisableColorDepthResetState == FALSE )
        			{
                    	SwitchVideoState(VSTATE_ColorDetectReset);
        			}
        			else
        			{
                    	SwitchVideoState(VSTATE_SyncChecking);
        			}
        		//~jau-chih.tseng@ite.com.tw 2011/10/17
                #endif
                return ;
            }

        }
    }

    if(VState==VSTATE_ColorDetectReset)
    {
        if(VideoCountingTimer==0 /*|| (IsSCDT() == TRUE) */)
        {
            // SwitchVideoState(VSTATE_ModeDetecting);
            SwitchVideoState(VSTATE_SyncChecking);
            return;
        }
    }

    if(VState == VSTATE_SyncChecking)
    {
        // printf(("SyncChecking %d\n",VideoCountingTimer));
        if(VideoCountingTimer == 0)
        {
            SwitchVideoState(VSTATE_ModeDetecting);
            return ;
        }
    }

    if(VState == VSTATE_HDCP_Reset)
    {
        // printf(("SyncChecking %d\n",VideoCountingTimer));
        if(VideoCountingTimer == 0)
        {
        	printf(("HDCP timer reach, reset !!\n"));
            // SwitchVideoState(VSTATE_PwrOff);
            SWReset_HDMIRX();
            return ;
        }
        else
        {
            printf(("VideoTimerHandler[VSTATE_HDCP_Reset](%d)\r\n",VideoCountingTimer));
            do {
	        	ClearIntFlags(B_CLR_ECC_INT);
	        	MDINDLY_mSec(1);
	        	uc = HDRX_RegRead(REG_RX_INTERRUPT3);
	        	if(uc & B_ECCERR)
	        	{
	                break ;
	        	}
	        	MDINDLY_mSec(1);
	        	ClearIntFlags(B_CLR_ECC_INT);
	        	MDINDLY_mSec(1);
	        	uc = HDRX_RegRead(REG_RX_INTERRUPT3);
	        	if(!(uc & B_ECCERR))
	        	{
	                SwitchVideoState(VSTATE_VideoOn);
	                return ;
	        	}
	        }while(0);
        }
    }

    if(VState == VSTATE_VideoOn)
    {
		char diff ;
		unsigned short HTotal ;
		unsigned char xCnt ;
		BOOL bVidModeChange = FALSE ;
		BOOL ScanMode ;
		// bGetSyncInfo();

        if( MuteByPKG == ON )
        {
            // if AVMute, ignore the video parameter compare.
            // if AVMute clear, the video parameter compare should be ignored.
            AssignVideoTimerTimeout(5) ;
        }
        else
        {
            if(VideoCountingTimer == 1)
            {
                bGetSyncInfo();
        		currHTotal = s_CurrentVM.HTotal ;
        		currXcnt = s_CurrentVM.xCnt ;
        		currScanMode = s_CurrentVM.ScanMode ;
            }
        }

        if(VideoCountingTimer == 0)
        {
            SCDTErrorCnt = 0 ;

            if( MuteByPKG == OFF )
            {
                // modified by jau-chih.tseng@ite.com.tw
                // Only AVMUTE OFF the video mode can be detected.
        		HTotal =(WORD)HDRX_RegRead(REG_RX_VID_HTOTAL_L);
        		HTotal |=(WORD)(HDRX_RegRead(REG_RX_VID_HTOTAL_H)&M_HTOTAL_H)<< 8 ;
        		// if(ABS((int)HTotal -(int)currHTotal)>4)
        		if(HTotal > currHTotal)
        		{
        			HTotal -= currHTotal ;
        		}
        		else
        		{
        			HTotal = currHTotal - HTotal ;
        		}

        		if(HTotal>4)
        		{
        			bVidModeChange = TRUE ;
        			printf(("HTotal changed.\r\n"));
        		}

        		if(!bVidModeChange)
        		{
        			xCnt =(BYTE)HDRX_RegRead(REG_RX_VID_XTALCNT_128PEL);

        			// 2011/02/18 modified by jau-chih.tseng@ite.com.tw
        			// to avoid the compiler calculation error. Change calculating
        			// method.
        			// diff =(char)currXcnt -(char)xCnt ;
        			if(currXcnt > xCnt )
        			{
        				diff = currXcnt - xCnt ;
        			}
        			else
        			{
        				diff = xCnt - currXcnt ;
        			}
        			//~jau-chih.tseng@ite.com.tw 2011/02/18


        			if(xCnt > 0x80)
        			{
        				if(diff> 6)
        				{
        					//printf("Xcnt changed. %02x -> %02x ",xCnt,currXcnt);
        					//printf("diff = %d\r\n",diff);
        					bVidModeChange = TRUE ;
        				}
        			}
        			else if(xCnt > 0x40)
        			{
        				if(diff> 4)
        				{
        					//printf("Xcnt changed. %02x -> %02x ",xCnt,currXcnt);
        					//printf("diff = %d\r\n",diff);
        					bVidModeChange = TRUE ;
        				}
        			}
        			else if(xCnt > 0x20)
        			{
        				if(diff> 2)
        				{
        					//printf("Xcnt changed. %02x -> %02x ",xCnt,currXcnt);
        					//printf("diff = %d\n\r",diff);
        					bVidModeChange = TRUE ;
        				}
        			}
        			else
        			{
        				if(diff> 1)
        				{
        					//printf("Xcnt changed. %02x -> %02x ",xCnt,currXcnt);
        					//printf("diff = %d\r\n",diff);
        					bVidModeChange = TRUE ;
        				}
        			}
        		}

                if(s_CurrentVM.VActive < 300)
                {
            		if(!bVidModeChange)
            		{
            			ScanMode =(HDRX_RegRead(REG_RX_VID_MODE)&B_INTERLACE)?INTERLACE:PROG ;
            			if(ScanMode != currScanMode)
            			{
            				//printf("ScanMode change.\r\n");
            				bVidModeChange = TRUE ;
            			}
            		}
                }
            }
        }



		if(bVidModeChange)
		{

			SwitchVideoState(VSTATE_SyncWait);
			return ;
		}
        else
        {
            unsigned char currAVI_DB1, currAVI_DB2 ;

            currAVI_DB1 = HDRX_RegRead(REG_RX_AVI_DB1);
            currAVI_DB2 = HDRX_RegRead(REG_RX_AVI_DB2);

            if(IsHDMIRXHDMIMode()){
                if((currAVI_DB1 != prevAVIDB1)||(currAVI_DB2 != prevAVIDB2)){
                    RXINT_SetNewAVIInfo();
                }
            }
            prevAVIDB1 = currAVI_DB1 ;
            prevAVIDB2 = currAVI_DB2 ;
        }

    }
}

BYTE HDMIRXFsGet(void)
{
    BYTE RxFS ;
    RxFS=HDRX_RegRead(REG_RX_FS)& 0x0F;
    return    RxFS;
}

//=============================================================================
void HDMIRXHBRMclkSet(BYTE cFs)
{
    BYTE    uc;

    uc=HDRX_RegRead(REG_RX_MCLK_CTRL)& 0xF8;

    switch(cFs)
    {
    case    9:
        uc |=0x0;    // 128xFs
        break;
    default:
        uc |=0x1;    // 256xFs
        break;
    }
    HDRX_RegWrite(REG_RX_MCLK_CTRL,uc);
}

static void MuteProcessTimerHandler(void)
{
    BYTE uc ;
    BOOL TurnOffMute = FALSE ;

    if(MuteByPKG == ON)
    {
        // printf(("MuteProcessTimerHandler()\n"));
        if((MuteResumingTimer > 0)&&(VState == VSTATE_VideoOn))
        {
            MuteResumingTimer -- ;
            uc = HDRX_RegRead(REG_RX_VID_INPUT_ST);
            //printf("MuteResumingTimer = %d uc = %02X\r\n",MuteResumingTimer ,(int)uc);

            if(!(uc&B_AVMUTE))
            {
                TurnOffMute = TRUE ;
                MuteByPKG = OFF ;
            }
            else if((MuteResumingTimer == 0))
            {
                bDisableAutoAVMute = B_VDO_MUTE_DISABLE ;

                uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
                uc |= B_VDO_MUTE_DISABLE ;
                HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);

                TurnOffMute = TRUE ;
                MuteByPKG = OFF ;
            }
        }

        if(MuteAutoOff)
        {
            uc = HDRX_RegRead(REG_RX_VID_INPUT_ST);
            if(!(uc & B_AVMUTE))
            {
                EndAutoMuteOffTimer();
                TurnOffMute = TRUE ;
            }
        }
    }

    if(TurnOffMute)
    {
        if(VState == VSTATE_VideoOn)
        {
            SetVideoMute(OFF);
            TurnOffMute = FALSE ;
        }
    }
}


void AssignVideoTimerTimeout(WORD TimeOut)
{
    VideoCountingTimer = TimeOut ;
}

void SwitchVideoState(Video_State_Type state)
{
    BYTE uc ;
	if(VState == state)
	{
		return ;
	}

//    printf("RX VState %s -> %s\r\n",VStateStr[VState],VStateStr[state]);
    VState = state ;

    if(VState != VSTATE_SyncWait && VState != VSTATE_SyncChecking)
    {
        SWResetTimeOut = FORCE_SWRESET_TIMEOUT;
        // init the SWResetTimeOut, decreasing when timer.
        // if down to zero when SyncWait or SyncChecking,
        // SWReset.
    }

    switch(bVideoOutputOption)
    {
    	case VIDEO_OFF:
    		SetMUTE(~B_TRI_VIDEO,B_TRI_VIDEO) ;
        break ;
    	case VIDEO_ON:
    		SetMUTE(~B_TRI_VIDEO,0) ;
        break ;
    	case VIDEO_AUTO:
    	default:
    		if( VState == VSTATE_VideoOn
    		|| VState == VSTATE_HDCP_Reset
            || VState == VSTATE_SyncChecking
            || VState == VSTATE_ModeDetecting )
    		{
    			SetMUTE(~B_TRI_VIDEO,0) ;
    		}
    		else
    		{
    			SetMUTE(~B_TRI_VIDEO,B_TRI_VIDEO) ;
    		}
    	break;
    }


    switch(VState)
    {
		case VSTATE_PwrOff:
			AcceptCDRReset = TRUE ;
			break ;
		case VSTATE_SWReset:
			// HDRX_RegWrite(REG_RX_GEN_PKT_TYPE, 0x03); // set default general control packet received in 0xA8
			AssignVideoTimerTimeout(VSTATE_SWRESET_COUNT);
			break ;
		case VSTATE_SyncWait:
			HDRX_RegWrite(REG_RX_REGPKTFLAG_CTRL,0);
			SetIntMask1(~(B_SCDTOFF|B_VIDMODE_CHG),0);
			if(ucRevision >= 0xA3)
			{
				uc = HDRX_RegRead(0x3C);
				uc &= ~(1<<2);
				HDRX_RegWrite(0x3C, uc);
			}
			bVSIpresent=FALSE ;
			SetVideoMute(ON);
			AssignVideoTimerTimeout(VSTATE_MISS_SYNC_COUNT);
			break ;
		#ifndef DISABLE_COLOR_DEPTH_RESET
		case VSTATE_ColorDetectReset:

			uc = HDRX_RegRead(REG_RX_INTERRUPT_MASK1) &(~B_SCDTOFF) ;
			HDRX_RegWrite(REG_RX_INTERRUPT_MASK1, uc) ;
			ClearIntFlags(B_CLR_MODE_INT);
			uc = HDRX_RegRead(REG_RX_CDEPTH_CTRL) & (~B_RSTCD);
			HDRX_RegWrite(REG_RX_CDEPTH_CTRL,B_RSTCD|uc) ;
			HDRX_RegWrite(REG_RX_CDEPTH_CTRL,uc) ;
			AssignVideoTimerTimeout(MS_TimeOut(400));
			break ;
		#endif

		case VSTATE_SyncChecking:
			HDRX_RegWrite(REG_RX_GEN_PKT_TYPE, 0x81); // set default general control packet received in 0xA8
			HDRX_RegWrite(REG_RX_REGPKTFLAG_CTRL, B_INT_GENERAL_EVERY) ;
			AssignVideoTimerTimeout(VSATE_CONFIRM_SCDT_COUNT);
			HDRX_RegRead(REG_RX_CHANNEL_ERR) ; // read 0x85 for clear CDR counter in reg9A.
			//printf(("switch VSTATE_SyncChecking, reg9A = %02X\n",(int)HDRX_RegRead(0x9A))) ;
			break ;
		case VSTATE_HDCP_Reset:
			SetVideoMute(ON);
			AssignVideoTimerTimeout(HDCP_WAITING_TIMEOUT);
			break ;
		case VSTATE_VideoOn:
			SetIntMask1(~(B_SCDTOFF|B_VIDMODE_CHG),(B_SCDTOFF|B_VIDMODE_CHG));
			AssignVideoTimerTimeout(5);
			// AcceptCDRReset = TRUE ;

			AssignVideoTimerTimeout(CDRRESET_TIMEOUT);
			if(!NewAVIInfoFrameF)
			{
				SetVideoInputFormatWithoutInfoFrame(F_MODE_RGB24);
				SetColorimetryByMode(/*&SyncInfo*/);
				SetColorSpaceConvert();
			}

			if(!IsHDMIRXHDMIMode())
			{
				SetIntMask1(~(B_SCDTOFF|B_PWR5VOFF),(B_SCDTOFF|B_PWR5VOFF));
				SetVideoMute(OFF); // turned on Video.
				NewAVIInfoFrameF = FALSE ;
			}
			else
			{

				if(NewAVIInfoFrameF)
				{
					SetNewInfoVideoOutput();
				}

				SetIntMask3(~(B_R_AUTH_DONE|B_R_AUTH_START),B_ECCERR);
				SetIntMask2(~(B_NEW_AVI_PKG|B_PKT_SET_MUTE|B_PKT_CLR_MUTE),(B_NEW_AVI_PKG|B_PKT_SET_MUTE|B_PKT_CLR_MUTE));
				SetIntMask1(~(B_SCDTOFF|B_PWR5VOFF),(B_SCDTOFF|B_PWR5VOFF));
				SetIntMask4(0,B_M_RXCKON_DET);

				MuteByPKG =(HDRX_RegRead(REG_RX_VID_INPUT_ST)& B_AVMUTE)?TRUE:FALSE ;
				bIgnoreVideoChgEvent = MuteByPKG ;

				SetVideoMute(MuteByPKG); // turned on Video.

				uc = HDRX_RegRead(REG_RX_MCLK_CTRL)&(~B_CTSINI_EN);
				HDRX_RegWrite(REG_RX_MCLK_CTRL, uc);
			}

			currHTotal = s_CurrentVM.HTotal ;
			currXcnt = s_CurrentVM.xCnt ;
			currScanMode = s_CurrentVM.ScanMode ;
		break;
    }
}

static void DumpSyncInfo(HDMI_VTiming *pVTiming)
{
#if 0
    double VFreq ;
    printf("{%4d,",pVTiming->HActive);
    printf("%4d,",pVTiming->VActive);
    printf("%4d,",pVTiming->HTotal);
    printf("%4d,",pVTiming->VTotal);
    printf("%8ld,",pVTiming->PCLK);
    printf("0x%02x,",pVTiming->xCnt);
    printf("%3d,",pVTiming->HFrontPorch);
    printf("%3d,",pVTiming->HSyncWidth);
    printf("%3d,",pVTiming->HBackPorch);
    printf("%2d,",pVTiming->VFrontPorch);
    printf("%2d,",pVTiming->VSyncWidth);
    printf("%2d,",pVTiming->VBackPorch);
    printf("%s,",pVTiming->ScanMode?"PROG":"INTERLACE");
    printf("%s,",pVTiming->VPolarity?"Vpos":"Vneg");
    printf("%s},",pVTiming->HPolarity?"Hpos":"Hneg");
    VFreq =(double)pVTiming->PCLK ;
    VFreq *= 1000.0 ;
    VFreq /= pVTiming->HTotal ;
    VFreq /= pVTiming->VTotal ;
    printf("/* %dx%d@%5.2lfHz */\r\n",pVTiming->HActive,pVTiming->VActive,VFreq);
#endif
}

static BOOL bGetSyncInfo(void)
{

    BYTE uc1, uc2, uc3 ;
#ifdef USE_MODE_TABLE
    long diff ;
    int i ;
#endif
//    pVTiming = NULL ;

//    pVTiming = &s_CurrentVM ;
    uc1 = HDRX_RegRead(REG_RX_VID_HTOTAL_L);
    uc2 = HDRX_RegRead(REG_RX_VID_HTOTAL_H);
    uc3 = HDRX_RegRead(REG_RX_VID_HACT_L);

    s_CurrentVM.HTotal =((WORD)(uc2&0xF)<<8)|(WORD)uc1;
    s_CurrentVM.HActive =((WORD)(uc2 & 0x70)<<4)|(WORD)uc3 ;
    if((s_CurrentVM.HActive |(1<<11))<s_CurrentVM.HTotal)
    {
        s_CurrentVM.HActive |=(1<<11);
    }
    uc1 = HDRX_RegRead(REG_RX_VID_HSYNC_WID_L);
    uc2 = HDRX_RegRead(REG_RX_VID_HSYNC_WID_H);
    uc3 = HDRX_RegRead(REG_RX_VID_H_FT_PORCH_L);

    s_CurrentVM.HSyncWidth =((WORD)(uc2&0x1)<<8)|(WORD)uc1;
    s_CurrentVM.HFrontPorch =((WORD)(uc2 & 0xf0)<<4)|(WORD)uc3 ;
    s_CurrentVM.HBackPorch = s_CurrentVM.HTotal - s_CurrentVM.HActive - s_CurrentVM.HSyncWidth - s_CurrentVM.HFrontPorch ;

    uc1 = HDRX_RegRead(REG_RX_VID_VTOTAL_L);
    uc2 = HDRX_RegRead(REG_RX_VID_VTOTAL_H);
    uc3 = HDRX_RegRead(REG_RX_VID_VACT_L);

    s_CurrentVM.VTotal =((WORD)(uc2&0x7)<<8)|(WORD)uc1;
    s_CurrentVM.VActive =((WORD)(uc2 & 0x30)<<4)|(WORD)uc3 ;
    if((s_CurrentVM.VActive |(1<<10))<s_CurrentVM.VTotal)
    {
        s_CurrentVM.VActive |=(1<<10);
    }

    s_CurrentVM.VBackPorch = HDRX_RegRead(REG_RX_VID_VSYNC2DE);
    s_CurrentVM.VFrontPorch = HDRX_RegRead(REG_RX_VID_V_FT_PORCH);
    s_CurrentVM.VSyncWidth = 0 ;

    s_CurrentVM.ScanMode =(HDRX_RegRead(REG_RX_VID_MODE)&B_INTERLACE)?INTERLACE:PROG ;

    s_CurrentVM.xCnt = HDRX_RegRead(REG_RX_VID_XTALCNT_128PEL);

    if(s_CurrentVM.xCnt)
    {
        s_CurrentVM.PCLK = 128L * 27000L / s_CurrentVM.xCnt ;
    }
    else
    {
        printf(("s_CurrentVM.xCnt == %02x\n",s_CurrentVM.xCnt));
        s_CurrentVM.PCLK = 1234 ;
        /*
        for(i = 0x58 ; i < 0x66 ; i++)
        {
            printf(("HDRX_RegRead(%02x)= %02X\n",i,(int)HDRX_RegRead(i)));
        }
        */
        return FALSE ;
    }

    // printf("Current Get: "); DumpSyncInfo(&s_CurrentVM);
    // printf("Matched %d Result in loop 1: ", i); DumpSyncInfo(pVTiming);

#ifndef USE_MODE_TABLE
	if((s_CurrentVM.VActive > 200)
		&&(s_CurrentVM.VTotal>s_CurrentVM.VActive)
		&&(s_CurrentVM.HActive > 300)
		&&(s_CurrentVM.HTotal>s_CurrentVM.HActive))
	{
		return TRUE ;
	}
#else
	// return TRUE ;
    for(i = 0 ; i < SizeofVMTable ; i++)
    {
        // 2006/10/17 modified by jjtseng
        // Compare PCLK in 3% difference instead of comparing xCnt

        // diff =(long)s_VMTable[i].xCnt -(long)s_CurrentVM.xCnt ;
        // if(ABS(diff)> 1)
        // {
        //     continue ;
        // }
        //~jjtseng 2006/10/17

		// 2011/02/18 modified by jau-chih.tseng@ite.com.tw
		// to avoid the compiler calculation error. Change calculating
		// method.
        // diff = ABS(s_VMTable[i].PCLK - s_CurrentVM.PCLK);
        if(s_VMTable[i].PCLK - s_CurrentVM.PCLK)
		{
			diff = s_VMTable[i].PCLK - s_CurrentVM.PCLK;
		}
		else
		{

	        diff = s_CurrentVM.PCLK - s_VMTable[i].PCLK;
		}
		//~jau-chih.tseng@ite.com.tw 2011/02/18

        diff *= 100 ;
        diff /= s_VMTable[i].PCLK ;

        if(diff > 3)
        {
            // over 3%
            continue ;
        }

        if(s_VMTable[i].HActive != s_CurrentVM.HActive)
        {
            continue ;
        }

        //if(s_VMTable[i].VActive != s_CurrentVM.VActive)
        //{
        //    continue ;
        //}

        if((long)s_VMTable[i].HTotal >=(long)s_CurrentVM.HTotal )
        {
            diff = (long)s_VMTable[i].HTotal  - (long)s_CurrentVM.HTotal  ;
        }
        else
        {
            diff = (long)s_CurrentVM.HTotal  - (long)s_VMTable[i].HTotal  ;
        }
        if(diff>4)
        {
            continue ;
        }

        if((long)s_VMTable[i].VActive >= (long)s_CurrentVM.VActive )
        {
            diff = (long)s_VMTable[i].VActive  - (long)s_CurrentVM.VActive  ;
        }
        else
        {
            diff = (long)s_CurrentVM.VActive  - (long)s_VMTable[i].VActive  ;
        }
        if(diff>10)
        {
            continue ;
        }

        if((long)s_VMTable[i].VTotal >= (long)s_CurrentVM.VTotal )
        {
            diff = (long)s_VMTable[i].VTotal  - (long)s_CurrentVM.VTotal  ;
        }
        else
        {
            diff = (long)s_CurrentVM.VTotal  - (long)s_VMTable[i].VTotal  ;
        }
        if(diff>40)
        {
            continue ;
        }

        if(s_VMTable[i].ScanMode != s_CurrentVM.ScanMode)
        {
            continue ;
        }

        s_CurrentVM = s_VMTable[i] ;
        // printf(("Matched %d Result in loop 1: ", i)); DumpSyncInfo(pVTiming);
        return TRUE ;
    }


    for(i = 0 ; i < SizeofVMTable ; i++)
    {
        if( s_VMTable[i].PCLK >= s_CurrentVM.PCLK)
        {
            diff = s_VMTable[i].PCLK  -  s_CurrentVM.PCLK ;
        }
        else
        {
            diff = s_CurrentVM.PCLK - s_VMTable[i].PCLK   ;
        }
        diff *= 100 ;
        diff /= s_VMTable[i].PCLK ;

        if(diff > 3)
        {
            // over 3%
            continue ;
        }

        if(s_VMTable[i].HActive != s_CurrentVM.HActive)
        {
            continue ;
        }

        //if(s_VMTable[i].VActive != s_CurrentVM.VActive)
        //{
        //    continue ;
        //}

        if((long)s_VMTable[i].HTotal >=(long)s_CurrentVM.HTotal )
        {
            diff = (long)s_VMTable[i].HTotal  - (long)s_CurrentVM.HTotal  ;
        }
        else
        {
            diff = (long)s_CurrentVM.HTotal  - (long)s_VMTable[i].HTotal  ;
        }
        if(diff>4)
        {
            continue ;
        }

        if((long)s_VMTable[i].VActive >= (long)s_CurrentVM.VActive )
        {
            diff = (long)s_VMTable[i].VActive  - (long)s_CurrentVM.VActive  ;
        }
        else
        {
            diff = (long)s_CurrentVM.VActive  - (long)s_VMTable[i].VActive  ;
        }
        if(diff>10)
        {
            continue ;
        }

        if((long)s_VMTable[i].VTotal >=(long)s_CurrentVM.VTotal )
        {
            diff = (long)s_VMTable[i].VTotal  - (long)s_CurrentVM.VTotal  ;
        }
        else
        {
            diff = (long)s_CurrentVM.VTotal  - (long)s_VMTable[i].VTotal  ;
        }
        if(diff>40)
        {
            continue ;
        }
        s_CurrentVM = s_VMTable[i] ;
        // printf(("Matched %d Result in loop 2: ", i)); DumpSyncInfo(pVTiming);
        return TRUE ;
    }
#endif
    return FALSE ;
}

#define SIZE_OF_CSCOFFSET (REG_RX_CSC_RGBOFF - REG_RX_CSC_YOFF + 1)
#define SIZE_OF_CSCMTX (REG_RX_CSC_MTX33_H - REG_RX_CSC_MTX11_L + 1)
#define SIZE_OF_CSCGAIN (REG_RX_CSC_GAIN3V_H - REG_RX_CSC_GAIN1V_L + 1)

///////////////////////////////////////////////////////////
// video.h
///////////////////////////////////////////////////////////
void Video_Handler(void)
{
    // SYNC_INFO SyncInfo, NewSyncInfo ;
    BOOL bHDMIMode;
    BYTE uc ;

    if(VState == VSTATE_ModeDetecting)
    {
        printf(("Video_Handler, VState = VSTATE_ModeDetecting.\n"));
        // printf(("Video Mode Detecting ... , REG_RX_RST_CTRL = %02X -> ",(int)HDRX_RegRead(REG_RX_RST_CTRL)));
        // HDRX_RegWrite(REG_RX_RST_CTRL,HDRX_RegRead(REG_RX_RST_CTRL)& ~B_HDCPRST|B_EN_AUTOVDORST);
        // printf(("%02X\n",(int)HDRX_RegRead(REG_RX_RST_CTRL)));
        uc = HDRX_RegRead(0x9A) ;
        printf(("Video_Handler(): reg9A = %02X\n", (int)uc)) ;

        if( uc == 0xFF )
        {
            HDRX_RegWrite(REG_RX_RST_CTRL, B_CDRRST|B_EN_AUTOVDORST) ;
            HDRX_RegWrite(REG_RX_RST_CTRL, B_EN_AUTOVDORST) ;
            SwitchVideoState(VSTATE_SyncWait);
            return ;
        }
        ClearIntFlags(B_CLR_MODE_INT);

        bGetSyncInfo();

        bHDMIMode = IsHDMIRXHDMIMode();

        if(!bHDMIMode)
        {
            printf(("This is DVI Mode.\n"));
            NewAVIInfoFrameF = FALSE ;
        }

        // GetSyncInfo(&NewSyncInfo);

        // if(CompareSyncInfo(&NewSyncInfo,&SyncInfo))
        if(HDRX_RegRead(REG_RX_INTERRUPT1)&(B_SCDTOFF|B_PWR5VOFF))
        {
            SwitchVideoState(VSTATE_SyncWait);
        }
        else
        {
            // HDCP_Reset(); // even though in DVI mode, Tx also can set HDCP.

            SwitchVideoState(VSTATE_VideoOn);
        }

        return ;
    }
}

static void SetVideoInputFormatWithoutInfoFrame(BYTE bInMode)
{
    BYTE uc ;

    // printf(("SetVideoInputFormat: NewAVIInfoFrameF = %s, bInMode = %d",(NewAVIInfoFrameF==TRUE)?"TRUE":"FALSE",bInMode));
    // only set force input color mode selection under no AVI Info Frame case
    uc = HDRX_RegRead(REG_RX_CSC_CTRL);
    uc |= B_FORCE_COLOR_MODE ;
    bInputVideoMode &= ~F_MODE_CLRMOD_MASK ;
    // bInputVideoMode |=(bInMode)&F_MODE_CLRMOD_MASK ;

    switch(bInMode)
    {
    case F_MODE_YUV444:
        uc &= ~(M_INPUT_COLOR_MASK<<O_INPUT_COLOR_MODE);
        uc |= B_INPUT_YUV444 << O_INPUT_COLOR_MODE ;
        bInputVideoMode |= F_MODE_YUV444 ;
        break ;
    case F_MODE_YUV422:
        uc &= ~(M_INPUT_COLOR_MASK<<O_INPUT_COLOR_MODE);
        uc |= B_INPUT_YUV422 << O_INPUT_COLOR_MODE ;
        bInputVideoMode |= F_MODE_YUV422 ;
        break ;
    case F_MODE_RGB24:
        uc &= ~(M_INPUT_COLOR_MASK<<O_INPUT_COLOR_MODE);
        uc |= B_INPUT_RGB24 << O_INPUT_COLOR_MODE ;
        bInputVideoMode |= F_MODE_RGB24 ;
        break ;
    default:
        printf(("Invalid Color mode %d, ignore.\n", bInMode));
        return ;
    }
    HDRX_RegWrite(REG_RX_CSC_CTRL, uc);

}

static void SetColorimetryByMode(/*PSYNC_INFO pSyncInfo*/)
{
    // WORD HRes, VRes ;
    bInputVideoMode &= ~F_MODE_ITU709 ;
    // HRes = pVTiming->HActive ;
    // VRes = pVTiming->VActive ;
    // VRes *=(pSyncInfo->Mode & F_MODE_INTERLACE)?2:1 ;
    if((s_CurrentVM.HActive == 1920)||(s_CurrentVM.HActive == 1280 && s_CurrentVM.VActive == 720))
    {
        // only 1080p, 1080i, and 720p use ITU 709
        bInputVideoMode |= F_MODE_ITU709 ;
    }
    else
    {
        // 480i,480p,576i,576p,and PC mode use 601
        bInputVideoMode &= ~F_MODE_ITU709 ; // set mode as ITU601
    }
}

void SetVideoInputFormatWithInfoFrame(void)
{
    BYTE uc ;
    BOOL bAVIColorModeIndicated = FALSE ;
    BOOL bOldInputVideoMode = bInputVideoMode ;

    printf("SetVideoInputFormatWithInfoFrame(): ");

    uc = HDRX_RegRead(REG_RX_AVI_DB1);
    printf("REG_RX_AVI_DB1 %02X get uc %02X ",(int)REG_RX_AVI_DB1,(int)uc);

    prevAVIDB1 = uc ;
    bInputVideoMode &= ~F_MODE_CLRMOD_MASK ;

    switch((uc>>O_AVI_COLOR_MODE)&M_AVI_COLOR_MASK)
    {
    case B_AVI_COLOR_YUV444:
        printf("input YUV444 mode ");
        bInputVideoMode |= F_MODE_YUV444 ;
        break ;
    case B_AVI_COLOR_YUV422:
        printf(("input YUV422 mode "));
        bInputVideoMode |= F_MODE_YUV422 ;
        break ;
    case B_AVI_COLOR_RGB24:
        printf(("input RGB24 mode "));
        bInputVideoMode |= F_MODE_RGB24 ;
        break ;
    default:
        printf("Invalid input color mode, ignore.\n");
        return ; // do nothing.
    }

    if((bInputVideoMode & F_MODE_CLRMOD_MASK)!=(bOldInputVideoMode & F_MODE_CLRMOD_MASK))
    {
        printf("Input Video mode changed.");
    }

    uc = HDRX_RegRead(REG_RX_CSC_CTRL);
    uc &= ~B_FORCE_COLOR_MODE ; // color mode indicated by Info Frame.
    HDRX_RegWrite(REG_RX_CSC_CTRL, uc);

    printf("\r\n");
}

BOOL SetColorimetryByInfoFrame(void)
{
    BYTE uc ;
    BOOL bOldInputVideoMode = bInputVideoMode ;

    printf("SetColorimetryByInfoFrame: NewAVIInfoFrameF = %s ",NewAVIInfoFrameF?"TRUE":"FALSE");

    if(NewAVIInfoFrameF)
    {
        uc = HDRX_RegRead(REG_RX_AVI_DB2);
        uc &= M_AVI_CLRMET_MASK<<O_AVI_CLRMET ;
        if(uc ==(B_AVI_CLRMET_ITU601<<O_AVI_CLRMET))
        {
            printf("F_MODE_ITU601\r\n");
            bInputVideoMode &= ~F_MODE_ITU709 ;
            return TRUE ;
        }
        else if(uc ==(B_AVI_CLRMET_ITU709<<O_AVI_CLRMET))
        {
            printf("F_MODE_ITU709\r\n");
            bInputVideoMode |= F_MODE_ITU709 ;
            return TRUE ;
        }
        // if no uc, ignore
        if((bInputVideoMode & F_MODE_ITU709)!=(bOldInputVideoMode & F_MODE_ITU709))
        {
            printf("Input Video mode changed.");
            // SetVideoMute(ON); // turned off Video for input color format change .
        }
    }
    printf("\r\n");
    return FALSE ;
}

void SetColorSpaceConvert(void)
{
    BYTE uc, csc=0 ;
    BYTE filter = 0 ; // filter is for Video CTRL DN_FREE_GO, EN_DITHER, and ENUDFILT

    // printf(("Input mode is YUV444 "));
    switch(bOutputVideoMode&F_MODE_CLRMOD_MASK)
    {
    #if defined(SUPPORT_OUTPUTYUV444)
    case F_MODE_YUV444:
        // printf(("Output mode is YUV444\n"));
	    switch(bInputVideoMode&F_MODE_CLRMOD_MASK)
	    {
			case F_MODE_YUV444:
				// printf(("Input mode is YUV444\n"));
				csc = B_CSC_BYPASS ;
				break ;
			case F_MODE_YUV422:
				// printf(("Input mode is YUV422\n"));
				csc = B_CSC_BYPASS ;
				if(bOutputVideoMode & F_MODE_EN_UDFILT)// RGB24 to YUV422 need up/dn filter.
				{
					filter |= B_RX_EN_UDFILTER ;
				}

				if(bOutputVideoMode & F_MODE_EN_DITHER)// RGB24 to YUV422 need up/dn filter.
				{
					filter |= B_RX_EN_UDFILTER | B_RX_DNFREE_GO ;
				}

				break ;
			case F_MODE_RGB24:
				// printf(("Input mode is RGB444\n"));
				csc = B_CSC_RGB2YUV ;
				break ;
	    }
        break ;
    #endif

    #if defined(SUPPORT_OUTPUTYUV422)

    case F_MODE_YUV422:
	    switch(bInputVideoMode&F_MODE_CLRMOD_MASK)
	    {
	    case F_MODE_YUV444:
            // printf(("Input mode is YUV444\n"));
	        if(bOutputVideoMode & F_MODE_EN_UDFILT)
	        {
	            filter |= B_RX_EN_UDFILTER ;
	        }
	        csc = B_CSC_BYPASS ;
	        break ;
	    case F_MODE_YUV422:
            // printf(("Input mode is YUV422\n"));
            csc = B_CSC_BYPASS ;

            // if output is YUV422 and 16 bit or 565, then the dither is possible when
            // the input is YUV422 with 24bit input, however, the dither should be selected
            // by customer, thus the requirement should set in ROM, no need to check
            // the register value .
            if(bOutputVideoMode & F_MODE_EN_DITHER)// RGB24 to YUV422 need up/dn filter.
            {
                filter |= B_RX_EN_UDFILTER | B_RX_DNFREE_GO ;
            }
	    	break ;
	    case F_MODE_RGB24:
            // printf(("Input mode is RGB444\n"));
            if(bOutputVideoMode & F_MODE_EN_UDFILT)// RGB24 to YUV422 need up/dn filter.
            {
                filter |= B_RX_EN_UDFILTER ;
            }
            csc = B_CSC_RGB2YUV ;
	    	break ;
	    }
	    break ;
    #endif

    #if defined(SUPPORT_OUTPUTRGB)
    case F_MODE_RGB24:
        // printf(("Output mode is RGB24\n"));
	    switch(bInputVideoMode&F_MODE_CLRMOD_MASK)
	    {
	    case F_MODE_YUV444:
            // printf(("Input mode is YUV444\n"));
	        csc = B_CSC_YUV2RGB ;
	        break ;
	    case F_MODE_YUV422:
            // printf(("Input mode is YUV422\n"));
            csc = B_CSC_YUV2RGB ;
            if(bOutputVideoMode & F_MODE_EN_UDFILT)// RGB24 to YUV422 need up/dn filter.
            {
                filter |= B_RX_EN_UDFILTER ;
            }
            if(bOutputVideoMode & F_MODE_EN_DITHER)// RGB24 to YUV422 need up/dn filter.
            {
                filter |= B_RX_EN_UDFILTER | B_RX_DNFREE_GO ;
            }
	    	break ;
	    case F_MODE_RGB24:
            // printf(("Input mode is RGB444\n"));
            csc = B_CSC_BYPASS ;
	    	break ;
	    }
	    break ;
    #endif
    }


    #if defined(SUPPORT_OUTPUTYUV)
    // set the CSC associated registers
    if(csc == B_CSC_RGB2YUV)
    {
        // printf(("CSC = RGB2YUV "));
        if(bInputVideoMode & F_MODE_ITU709)
        {
            printf(("ITU709 "));

            if(bInputVideoMode & F_MODE_16_235)
            {
                printf((" 16-235\n"));
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_16_235,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU709_16_235,18);
            }
            else
            {
                printf((" 0-255\n"));
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_0_255,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU709_0_255,18);
            }
        }
        else
        {
            printf(("ITU601 "));
            if(bInputVideoMode & F_MODE_16_235)
            {
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_16_235,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU601_16_235,18);
                printf((" 16-235\n"));
            }
            else
            {
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_0_255,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_RGB2YUV_ITU601_0_255,18);
                printf((" 0-255\n"));
            }
        }
    }
    #endif

    #if defined(SUPPORT_OUTPUTRGB)
	if(csc == B_CSC_YUV2RGB)
    {
        printf(("CSC = YUV2RGB "));
        if(bInputVideoMode & F_MODE_ITU709)
        {
            printf(("ITU709 "));
            if(bOutputVideoMode & F_MODE_16_235)
            {
                printf(("16-235\n"));
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_16_235,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU709_16_235,18);
            }
            else
            {
                printf(("0-255\n"));
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_0_255,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU709_0_255,18);
            }
        }
        else
        {
            printf(("ITU601 "));
            if(bOutputVideoMode & F_MODE_16_235)
            {
                printf(("16-235\n"));
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_16_235,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU601_16_235,18);
            }
            else
            {
                printf(("0-255\n"));
                HDRX_RegWriteN(REG_RX_CSC_YOFF,bCSCOffset_0_255,3);
                HDRX_RegWriteN(REG_RX_CSC_MTX11_L,bCSCMtx_YUV2RGB_ITU601_0_255,18);
            }
        }

    }
	#endif // SUPPORT_OUTPUTRGB


    uc = HDRX_RegRead(REG_RX_CSC_CTRL);
    uc =(uc & ~M_CSC_SEL_MASK)|csc ;
    HDRX_RegWrite(REG_RX_CSC_CTRL,uc);

    // set output Up/Down Filter, Dither control

    uc = HDRX_RegRead(REG_RX_VIDEO_CTRL1);
    uc &= ~(B_RX_DNFREE_GO|B_RX_EN_DITHER|B_RX_EN_UDFILTER);
    uc |= filter ;
    HDRX_RegWrite(REG_RX_VIDEO_CTRL1, uc);
}


void SetDVIVideoOutput(void)
{
    // SYNC_INFO SyncInfo ;
    // GetSyncInfo(&SyncInfo);
    SetVideoInputFormatWithoutInfoFrame(F_MODE_RGB24);
    SetColorimetryByMode(/*&SyncInfo*/);
    SetColorSpaceConvert();
}

void SetNewInfoVideoOutput(void)
{
    SetVideoInputFormatWithInfoFrame();
    SetColorimetryByInfoFrame();
    SetColorSpaceConvert();
    // DumpHDMIRXReg();
}

void SetHDMIRXVideoOutputFormat(BYTE bOutputMapping, BYTE bOutputType, BYTE bOutputColorMode)
{
    BYTE uc ;
    SetVideoMute(ON);

    printf("SetHDMIRXVideoOutputFormat(%02X,%02X,%02X)\r\n",(int)bOutputMapping,(int)bOutputType,(int)bOutputColorMode);
    HDRX_RegWrite(REG_RX_VIDEO_CTRL1,bOutputType);
    HDRX_RegWrite(REG_RX_VIDEO_MAP,bOutputMapping);
    bOutputVideoMode&=~F_MODE_CLRMOD_MASK;

    bOutputVideoMode |= bOutputColorMode&F_MODE_CLRMOD_MASK ;
    uc = HDRX_RegRead(REG_RX_PG_CTRL2)& ~(M_OUTPUT_COLOR_MASK<<O_OUTPUT_COLOR_MODE);

    switch(bOutputVideoMode&F_MODE_CLRMOD_MASK)
    {
    case F_MODE_YUV444:
        uc |= B_OUTPUT_YUV444 << O_OUTPUT_COLOR_MODE ;
        break ;
    case F_MODE_YUV422:
        uc |= B_OUTPUT_YUV422 << O_OUTPUT_COLOR_MODE ;
        break ;
    }
    printf("write %02X %02X\r\n",(int)REG_RX_PG_CTRL2,(int)uc);
    HDRX_RegWrite(REG_RX_PG_CTRL2, uc);

    if(VState == VSTATE_VideoOn)
    {
        if(IsHDMIRXHDMIMode())
        {
            SetNewInfoVideoOutput();
        }
        else
        {
            SetDVIVideoOutput();
        }
        SetVideoMute(MuteByPKG);
    }

}


static void SetALLMute(void)
{
    BYTE uc ;

    uc = (bVideoOutputOption==VIDEO_ON)?B_TRI_ALL:(B_TRI_ALL|B_TRI_VIDEO);

    SetMUTE(B_VDO_MUTE_DISABLE,uc);
}

void SetVideoMute(BOOL bMute)
{
    BYTE uc ;

    if(bMute)
    {
        // 2009/11/04 added by jau-chih.tseng@ite.com.tw
        // implement the video gatting for video output.
		uc = HDRX_RegRead(REG_RX_CSC_CTRL);
		uc |= B_VDIO_GATTING | B_VIO_SEL ; // video data set to low.
		HDRX_RegWrite(REG_RX_CSC_CTRL, uc);

        uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
        uc &= ~(B_TRI_VDIO) ;
        // uc |= B_VDO_MUTE_DISABLE ;
        HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);
        //~jau-chih.tseng@ite.com.tw 2009/11/04

    }
    else
    {
        if(VState == VSTATE_VideoOn)
        {
            uc = HDRX_RegRead(REG_RX_VIDEO_CTRL1);
            HDRX_RegWrite(REG_RX_VIDEO_CTRL1,uc|B_565FFRST);
            HDRX_RegWrite(REG_RX_VIDEO_CTRL1,uc&(~B_565FFRST));

            if(HDRX_RegRead(REG_RX_VID_INPUT_ST)&B_AVMUTE)
            {
                uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
                uc &= ~(B_TRI_VDIO );
                uc |= B_VDO_MUTE_DISABLE ;
                HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);
            }
            else
            {
                uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
                uc &= ~B_VDO_MUTE_DISABLE ;
                // HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);

        		// enable video io gatting
        		// uc = HDRX_RegRead(REG_RX_TRISTATE_CTRL);
        		uc |= B_TRI_VDIO ;
        		HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);
        		printf("reg %02X <- %02X = %02X\r\n",(int)REG_RX_TRISTATE_CTRL,(int)uc, (int)HDRX_RegRead(REG_RX_TRISTATE_CTRL));
        		uc &= ~B_TRI_VDIO ;
        		HDRX_RegWrite(REG_RX_TRISTATE_CTRL, uc);
        		printf("reg %02X <- %02X = %02X\r\n",(int)REG_RX_TRISTATE_CTRL,(int)uc, (int)HDRX_RegRead(REG_RX_TRISTATE_CTRL));

        		uc = HDRX_RegRead(REG_RX_CSC_CTRL);
        		uc |= B_VDIO_GATTING|B_VIO_SEL ;
        		HDRX_RegWrite(REG_RX_CSC_CTRL, uc);
        		printf("reg %02X <- %02X = %02X\r\n",(int)REG_RX_CSC_CTRL,(int)uc,(int)HDRX_RegRead(REG_RX_CSC_CTRL));
        		uc &= ~B_VDIO_GATTING ;
        		HDRX_RegWrite(REG_RX_CSC_CTRL, uc);
        		printf("reg %02X <- %02X = %02X\r\n",(int)REG_RX_CSC_CTRL,(int)uc,(int)HDRX_RegRead(REG_RX_CSC_CTRL));
        	}

        }
    }
}

