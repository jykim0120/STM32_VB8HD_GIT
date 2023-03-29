#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "..\common\coaxial.h"
#include "..\common\i2c.h"
#include "..\drivers\mdini2c.h"
#include "..\drivers\mdin3xx.h"
#include	"..\video\video.h"		//2		[ JYKim 		2015-5-7 ]


#define STRCMP(s1,s2)		(strcmp((char*)s1,(char*)s2))


#define	_addr			3
#define	_data			4

extern NCDEC_INFO			ncDec;


/***************************************************************************
	Global Variables Definition
*************************************************************************/
extern BYTE SerialRxCount;
extern BYTE g_bState;
extern CHAR SerialRxData[15];
extern MDIN_VIDEO_INFO	stVideo;
BYTE debug_flag = 0;
CHAR argv[5][10];


//////////////////////////////////////////////////////////////////////////////
enum
{
	debug_nvp1918c=0,
	debug_dump,
	debug_tw9960,
	debug_coax,
	debug_ccvc,
	debug_hdmirx,
	debug_mdin,
	debug_mdinf,
	debug_mdindump,
	debug_mdintuning,
	debug_mdintuning1,
	debug_mdintuning2,
	nvp6114a_dump,			//2		[ JYKim 		2015-4-29 ]
	nvp6114a_debug_init,		//2		[ JYKim 		2015-4-29 ]
	nvp6114a_debug,			//2		[ JYKim 		2015-4-29 ]
	nvp6114a_switch,			//2		[ JYKim 		2015-5-7 ]
	nvp6124b_init,
	nvp6124b_init2,
	mdin340_init,
	nvp6124b_debug,
	mdin380_test,				//2		[ JYKim 		2015-5-7 ]
	mdin_csc,
	nvp6124b_ch,
	uart_test1,
	uart_test2,
	mirror_test,
	multi_ch_test,

	nextune_id,
	nextune_fw,

	cmd_list,
	help,
	debug,
	cmd_exit
};

#define DebugMAXNum (debug + 2)

typedef struct _TCommand
{
   char *CmdStr;   	// 명령 문자열
   char cmdnum;
} TCommand;

static TCommand Cmds[DebugMAXNum+4] =
{
	{ "dec"		, debug_nvp1918c},
	{ "dd"		, debug_dump},
	{ "tw" 		, debug_tw9960},
	{ "co"		, debug_coax},
	{ "so"		, debug_ccvc},
	{ "hrx"		, debug_hdmirx},
	{ "md"		, debug_mdin},
	{ "mdf"		, debug_mdinf},
	{ "mdd"		, debug_mdindump},
	{ "mdt"		, debug_mdintuning},
	{ "mdg"		, debug_mdintuning1},
	{ "mdc" 	, debug_mdintuning2},
	{ "nd" 		, nvp6114a_dump},				//2		[ JYKim 		2015-4-29 ]
	{ "ni" 		, nvp6114a_debug_init},			//2		[ JYKim 		2015-4-29 ]
	{ "nn" 		, nvp6114a_debug},			//2		[ JYKim 		2015-4-29 ]
	{ "nsw" 	, nvp6114a_switch},			//2		[ JYKim 		2015-4-29 ]
	{ "nii" 	, nvp6124b_init},		
	{ "ni2" 	, nvp6124b_init2},		
	{ "m3i" 	, mdin340_init},				
	{ "nv" 		, nvp6124b_debug},			//2		[ JYKim 		2015-4-29 ]	
	{ "mt" 		, mdin380_test},			//2		[ JYKim 		2015-4-29 ]
	{ "mc"		, mdin_csc},			//2		[ JYKim 		2015-4-29 ]
	{ "ch"		, nvp6124b_ch},			//2		[ JYKim 		2015-4-29 ]
	{ "ut1"		, uart_test1},			//2		[ JYKim 		2015-4-29 ]
	{ "ua"		, uart_test2},			//2		[ JYKim 		2015-4-29 ]
	{ "mr"		, mirror_test},			//2		[ JYKim 		2015-4-29 ]
	{ "mul"		, multi_ch_test},			//2		[ JYKim 		2015-4-29 ]
	
	{ "#PRODUCT", nextune_id},
	{ "#FIRMWARE", nextune_fw},
	{ "ls"		, cmd_list},
	{ "help"	, help},
	{ "debug"	, debug},
	{ (char*)NULL, 0  }
};
///////////////////////////////////////////////////////////////////////////////
/// Function prototype
//////////////////////////////////////////////////////////////////////////////
static SHORT parse_args( void );
static CHAR KeyControl(BYTE cnt,BYTE argc);
static CHAR KeydebugControl(BYTE cnt,BYTE argc);

static void uart2_command_list_debug(void);

static void command_debug(void);
static void standby_com( void );

void debug_main( void );


extern BYTE	CAMERA_CH_STATUS;

/*************************************************************************
	parse_args
*************************************************************************/
static SHORT parse_args( void )
{
	int argc = 0, i, len;

    char *tok;

	static const char *delim = " \f\n\r\t\v";

	for ( tok = strtok( SerialRxData, delim );
		  tok;
		  tok = strtok( NULL, delim ) )
    {
		len = strlen(tok);
		for(i = 0; i < len; i++)
			argv[argc][i] = tok[i];

		argc++;
    }

    return argc;
}

///////////////////////////////////////////////////////////////////////////////

//	debug main
///////////////////////////////////////////////////////////////////////////////

void debug_main(void)
{
	BYTE i,strcheck,argc;

	i = 0;
	argc = 0;

	if( g_bState )
	{
		argc = parse_args();

		if(argc)
		{
			for( i = 0; i < DebugMAXNum; i++ )
			{
				if( STRCMP( argv[0], Cmds[i].CmdStr ) == 0 )
				{
					if(debug_flag)
					{
						KeydebugControl( i, argc );
					}
					else
					{
						KeyControl( i, argc );
					}
					break;
				}
				else if (i == (DebugMAXNum-1) )						//2		[ JYKim 		2015-11-20 ]
				{
					printf("[PROMPT]# %s		<-- Input error",argv[0]);
					standby_com();
				}
			}
		}
		else														//2		[ JYKim 		2015-11-20 ]  NULL or ENTER KEY 
		{
			standby_com();
		}
		
		g_bState = 0;
		SerialRxCount = 0;
		memset( SerialRxData, 0, sizeof(SerialRxData));
		memset(argv[0],0,sizeof(argv[0]));
		memset(argv[1],0,sizeof(argv[0]));
		memset(argv[2],0,sizeof(argv[0]));
		memset(argv[3],0,sizeof(argv[0]));
		memset(argv[4],0,sizeof(argv[0]));
	}
}

static CHAR KeydebugControl(BYTE cnt,BYTE argc)
{
	CHAR *endptr;
	WORD unit;

	switch(cnt)
	{
		case debug_dump:
				if( argc == 2 )
				{
					WORD j, bank;
					BYTE val;

				    bank = strtol( argv[1],&endptr, 16);
					NVP1918C_RegWrite( 0xFF, bank);
					printf("\r\n===================================================");
					printf("\r\n             BANK 0x0%x REGISTER DUMP              ",bank);
					printf("\r\n---------------------------------------------------");
					printf("\r\n    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
					printf("\r\n---------------------------------------------------");
					for(j=0;j<256;j++)
					{
						if((j%16)==0)
						{
							if(j==0) printf("\r\n0%x| ",j);
							else 	 printf("\r\n%x| ",j);
						}
						val = NVP1918C_RegRead( j );
						if(val < 0x10)
							printf("0%x ",val);
						else
							printf("%x ",val);

					}
					printf("\r\n");
				}
				else
				{
					printf("Example : dd s.a bank\r\n");
				}
				standby_com();
			break;

		case debug_nvp1918c:
			switch( argc )
			{
				case 2: //read
					printf("\r\nRdata : 0x%x\r\n",NVP1918C_RegRead(strtol( argv[1], &endptr, 16)));
					break;
				case 3: //write
					NVP1918C_RegWrite(strtol( argv[1],&endptr, 16), strtol( argv[2],&endptr, 16));
					printf("\n\rRdata : 0x%x\r\n",NVP1918C_RegRead(strtol( argv[1],&endptr, 16)));
					break;
				default :
					printf("\n\rERROR\r\n");
					break;
			}
			standby_com();
			break;
		case debug_tw9960:
			switch( argc )
			{
				case 2: //read
					printf("\r\nRdata : 0x%x\r\n",TW9960_RegRead(strtol( argv[1], &endptr, 16)));
					break;
				case 3: //write
					TW9960_RegWrite(strtol( argv[1],&endptr, 16), strtol( argv[2],&endptr, 16));
					printf("\n\rRdata : 0x%x\r\n",TW9960_RegRead(strtol( argv[1],&endptr, 16)));
					break;
				default :
					printf("\n\rERROR\r\n");
					break;

			}
			standby_com();
			break;

		case debug_coax:

			if( argc == 2 )
			{
				switch( strtol( argv[1],&endptr, 16) )
				{
					case 0: pelco_command(PELCO_CMD_SET);	break;
					case 1: pelco_command(PELCO_CMD_UP);	break;
					case 2: pelco_command(PELCO_CMD_DOWN);	break;
					case 3: pelco_command(PELCO_CMD_LEFT);	break;
					case 4: pelco_command(PELCO_CMD_RIGHT);	break;
					case 5: pelco_command(PELCO_CMD_OSD);	break;
					case 6: pelco_command(PELCO_CMD_RESET);	break;
					case 152: pelco_command(PELCO_CMD_COMET_OFF);	break;
					case 153: pelco_command(PELCO_CMD_COMET_ON);	break;					

					case 256: pelco_coax_mode();printf("\r\nCOAX SET!!\r\n ");	break;
				}
			}
			else
			{
				printf("\n\rERROR\r\n");
			}
			standby_com();
			break;

		case debug_ccvc:

			if( argc == 2 )
			{
				WORD ccvc_val;
				ccvc_val = strtol( argv[1],&endptr, 16);

				if(ccvc_val >= 100)
				{
					samsung_coax_mode();printf("\r\nCCVC SET!!\r\n ");
				}
				else
				{
					samsung_coax_command(ccvc_val);
				}

			}
			else
			{
				printf("\n\rERROR\r\n");
			}
			standby_com();
			break;

		case debug_hdmirx:
			switch( argc )
			{
				case 2:	//read
					printf("\r\nRdata : 0x%x\r\n",HDRX_RegRead(strtol( argv[1], &endptr, 16)));
					break;
				case 3:	//write
					HDRX_RegWrite(strtol( argv[1],&endptr, 16), strtol( argv[2],&endptr, 16));
					printf("\n\rRdata : 0x%x\r\n",HDRX_RegRead(strtol( argv[1],&endptr, 16)));
					break;
				default :
					printf("\n\rERROR\r\n");
					break;

			}
			standby_com();
			break;
		case debug_mdin:
			switch( argc )
			{
				case 3:	//read
					MDINHIF_RegRead(strtol( argv[1],&endptr, 16),strtol( argv[2],&endptr, 16),&unit);
					printf("\n\rRdata : 0x%04x\r\n",unit);
					break;
				case 4:	//write
					MDINHIF_RegWrite(strtol( argv[1],&endptr, 16),
									 strtol( argv[2],&endptr, 16),
									 strtol( argv[3],&endptr, 16));
					MDINHIF_RegRead(strtol( argv[1],&endptr, 16),strtol( argv[2],&endptr, 16),&unit);
					printf("\n\rRdata : 0x%04x\r\n",unit);
					break;
				default :
					printf("\n\rERROR\r\n");
					break;
			}
			standby_com();
			break;

		case debug_mdinf:
			switch( argc )
			{
				case 6:	//write
					MDINHIF_RegField(strtol( argv[1],&endptr, 16),
									 strtol( argv[2],&endptr, 16),
									 strtol( argv[3],&endptr, 16),
									 strtol( argv[4],&endptr, 16),
									 strtol( argv[5],&endptr, 16));
					printf("\r\n Write OK \r\n");
					break;
				default :
					printf("\n\rERROR\r\n");
				break;

			}
			standby_com();
			break;
		case debug_mdindump:
		{
			WORD unit,j;
//			BYTE val;

			if(argc == 3)
			{
				printf("\r\n===================================================");
				printf("\r\n                 REGISTER DUMP                     ");
				printf("\r\n---------------------------------------------------");
				for(j=0;j<256;j++)
				{
					if((j%16)==0)
					{
						if(j==0) printf("\r\n0%x| ",j);
						else 	 printf("\r\n%x| ",j);
					}

					MDINHIF_RegRead(strtol( argv[1],&endptr, 16),strtol( argv[2],&endptr, 16)+ j,&unit);

					if(unit < 0x10)
						printf("000%x ",unit);
					else if(unit < 0x100)
						printf("00%x ",unit);
					else if(unit < 0x1000)
						printf("0%x ",unit);
					else
						printf("%x ",unit);

				}
			}
			printf("\r\n");
			standby_com();
		}
			break;
		
		case debug_mdintuning:
			switch( argc )
			{
				case 1:	
					printf("brightness = %02x\r\n"	,stVideo.stOUT_m.brightness);
					printf("contrast = %02x\r\n"	,stVideo.stOUT_m.contrast);
					printf("saturation = %02x\r\n"	,stVideo.stOUT_m.saturation);
					printf("hue = %02x\r\n"			,stVideo.stOUT_m.hue);
				break;
				case 5:	
					MDIN3xx_AHDTuning(strtol( argv[1],&endptr, 16),
									 strtol( argv[2],&endptr, 16),
									 strtol( argv[3],&endptr, 16),
									 strtol( argv[4],&endptr, 16));
				break;
			}
			standby_com();
			break;
			
		case debug_mdintuning1:
			switch( argc )
			{
				case 2: 
					DEMO_SetBWExtension(strtol( argv[1],&endptr, 16));
				break;
			}
			standby_com();
			break;

		case debug_mdintuning2:
			switch( argc )
			{
				case 2: 
					MDIN3xx_SetSrcVideoCSC_test(strtol( argv[1],&endptr, 16));
				break;
			}
			standby_com();
			break;

		case nvp6114a_dump:
				if( argc == 2 )
				{
					WORD j, bank;
					BYTE val;

				    bank = strtol( argv[1],&endptr, 16);
					AHD_RegWrite( 0xFF, bank);
					printf("\r\n===================================================");
					printf("\r\n             BANK 0x0%x REGISTER DUMP              ",bank);
					printf("\r\n---------------------------------------------------");
					printf("\r\n    00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
					printf("\r\n---------------------------------------------------");
					for(j=0;j<256;j++)
					{
						if((j%16)==0)
						{
							if(j==0) printf("\r\n0%x| ",j);
							else 	 printf("\r\n%x| ",j);
						}
						val = AHD_RegRead( j );
						if(val < 0x10)
							printf("0%x ",val);
						else
							printf("%x ",val);

					}
					printf("\r\n");
				}
				else
				{
					printf("Example : nv bank\r\n");
				}
				standby_com();
			break;
		case nvp6114a_debug_init:
			printf("initial__1080p\r\n");
			ncDec.rx.resolution = NCRESOL_1920X1080P;
			Write_Config(&ncDec);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			MDIN3xx_EnableDeint3DNR(&stVideo,OFF);		

			printf(" NVP6124B RESET COMPLETE \r\n");
			standby_com();
			break;

		case nvp6114a_debug:
			{
				WORD bank;
					
				bank = strtol( argv[1],&endptr, 16);
				AHD_RegWrite( 0xFF, bank);
					
				switch( argc )
				{
					case _addr: //read
						printf("\r\nR Rdata : 0x%x\r\n",AHD_RegRead(strtol( argv[2], &endptr, 16)));
						break;
						
					case _data: //write
						AHD_RegWrite(strtol( argv[2],&endptr, 16), strtol( argv[3],&endptr, 16));
						printf("\n\rW Rdata : 0x%x\r\n",AHD_RegRead(strtol( argv[2],&endptr, 16)));
						break;
						
					default :
						printf("\n\rERROR\r\n");
						break;
				}
				standby_com();
			}
			break;

		case nvp6114a_switch:
			if (ncDec.rx.resolution == NCRESOL_1920X1080P)
			{
				printf("\r\n 720p");
				ncDec.rx.resolution = NCRESOL_1280X720P;
			}
			else
			{
				printf("\r\n 1080p");
				ncDec.rx.resolution = NCRESOL_1920X1080P;
			}
			Write_Config(&ncDec);
			MDIN3xx_Initial(&ncDec);
			DecoderInitial(&ncDec);
			MDIN3xx_EnableDeint3DNR(&stVideo,OFF);		

			printf(" NVP6114A RESET COMPLETE \r\n");
			standby_com();
			break;

		case nvp6124b_init:
			printf("debug_nvp6124b_initial__1080p\r\n");
			ncDec.rx.resolution = NCRESOL_1920X1080P;
//			Write_Config(&ncDec);
//			MDIN3xx_Initial(&ncDec);
//			DecoderInitial(&ncDec);
			KJY_AHD_VARIABLE_INIT();

			standby_com();
			break;


		case nvp6124b_init2:
			printf("nvp6124b_initial2\r\n");
			ncDec.rx.resolution = NCRESOL_1920X1080P;
			Write_Config(&ncDec);
			MDIN3xx_Initial(&ncDec);
//			DecoderInitial(&ncDec);
			standby_com();
			break;

		case mdin340_init:
			printf("CreateMDIN340VideoInstance_init\r\n");
			CreateMDIN340VideoInstance();
			standby_com();
			break;

		case nvp6124b_debug:
			{
				WORD bank;
					
				bank = strtol( argv[1],&endptr, 16);
				AHD_RegWrite( 0xFF, bank);
					
				switch( argc )
				{
					case _addr: //read
						printf("\r\nR Rdata : 0x%x\r\n",AHD_RegRead(strtol( argv[2], &endptr, 16)));
						break;
						
					case _data: //write
						AHD_RegWrite(strtol( argv[2],&endptr, 16), strtol( argv[3],&endptr, 16));
						printf("\n\rW Rdata : 0x%x\r\n",AHD_RegRead(strtol( argv[2],&endptr, 16)));
						break;
						
					default :
						printf("\n\rERROR\r\n");
						break;
				}
				standby_com();
			}
			break;

		case mdin380_test:
			{
				WORD tmp;
				tmp = strtol( argv[1],&endptr, 16);

				MDIN3xx_SetOutTestPattern(tmp);
			}
		
			standby_com();
			break;

		case mdin_csc:
			{
				WORD tmp;
				tmp = strtol( argv[1],&endptr, 16);

				MDIN3xx_SetSrcVideoCSC_test(tmp);
			}
			standby_com();
			break;

		case nvp6124b_ch:
			{
				WORD tmp;
				
				switch( argc )
				{
					case 1: //read
							printf("READ CAMERA_CH_STATUS : %d\r\n",CAMERA_CH_STATUS);
							printf("READ MDIN340__CH : %d\r\n",kjy_ahd_ch_status());
							break;
						
					case 2: //write
							tmp = strtol( argv[1],&endptr, 16);
							kjy_ahd_ch_ctrl(tmp);
							break;
				}
			}
			standby_com();
			break;

		case uart_test1:
			kjy__puts_test1();
			standby_com();
			break;

		case uart_test2:
			{
				BYTE tmp1,tmp2;
				
				#if 1
				tmp1 = strtol( argv[1],&endptr, 16);
				tmp2 = strtol( argv[2],&endptr, 16);
				printf("\r\n");
				#else
				switch( argc )
				{
					case 1: //_datga1
								tmp1 = strtol( argv[1],&endptr, 16);
						break;
						
					case 2: //_data2
							tmp2 = strtol( argv[2],&endptr, 16);
						break;
						
					default :
						printf("\n\rERROR\r\n");
						break;
				}
				#endif
				KJY_UART2_BYTE5_TX(tmp1,tmp2);
				standby_com();
			}
			break;
		
		case mirror_test:
			{
				WORD tmp;
				tmp = strtol( argv[1],&endptr, 16);

				MDIN3xx_EnableMirrorH(&stVideo, tmp);
				MDIN3xx_EnableMirrorV(&stVideo, tmp);

			}
			standby_com();
			break;

		case multi_ch_test:
			{
				WORD tmp;
				tmp = strtol( argv[1],&endptr, 16);

//				MDIN3xx_EnableMirrorH(&stVideo, tmp);
				stVideo.st4CH_x.view = tmp ;//	MDIN_4CHVIEW_ALL;

				MDIN3xx_SetDisplay4CH(&stVideo, tmp);
			}
			standby_com();
			break;

		
			

		case cmd_list:
			uart2_command_list_debug();
			standby_com();
			break;


		case help:
			command_debug();
			standby_com();
			break;



		default :
			printf("Fail!!\r\n ");
			standby_com();
			break;
	}
	return TRUE;
}

static CHAR KeyControl(BYTE cnt,BYTE argc)
{
	switch( Cmds[cnt].cmdnum )
	{
		#if 0
		case debug:
			command_debug();
			debug_flag = 1;
			break;

		case nextune_id :
			break;
		case nextune_fw :
			break;
		#endif
		
		default :
			standby_com();
			break;
	}
	return TRUE;
}


static void standby_com( void )
{
	if(debug_flag==0)				//2 [JYKIM-2023-1-25]	
	{
		command_debug();
		debug_flag = 1;
	}
	
	printf("\r\n");
	printf("[PROMPT]# ");
	fflush(stdout);
}

static void uart2_command_list_debug(void)
{
	printf("\r\n");

	printf(" ______________________________________________ \r\n");
	printf("|                                              |\r\n");
	printf("|------- UART2_STM -> MSTAR__COMMAND_LIST -----|\r\n");
	printf("|______________________________________________|\r\n");
	printf("|                                              |\r\n");
	printf("| ua [31] [0-4]  | CAM STATUS 0~4              |\r\n");
	printf("|                |                             |\r\n");
	printf("| ls             | STM->MSTAR__COMMAND_LIST    |\r\n");
	printf("|______________________________________________|\r\n");
}


static void command_debug(void)
{
	printf("\r\n");
	printf(" ___________________________________ \r\n");
	printf("|                                   |\r\n");
	printf("|------------ COMMAND --------------|\r\n");
	printf("|___________________________________|\r\n");
	printf("|                                   |\r\n");
	printf("| dec [A] [D]    | NVP1918C         |\r\n");
	printf("| dd  [B]        | DUMP NVP1918C    |\r\n");
	printf("| tw  [A] [D]    | TW9960           |\r\n");
	printf("| co  [NUM]      | COAX COMMAND     |\r\n");
	printf("| so  [NUM]      | CCVC COMMAND     |\r\n");
	printf("| hrx [NUM]      | HDMI RECV        |\r\n");
	printf("| md  [ID][A][D] | MDIN             |\r\n");
	printf("| mdd [ID][Add]  | MDIN dump        |\r\n");
	printf("| nd  [B]        | NVP6114A dump    |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| ni             | NVP6114A init    |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| nii            | NVP6124B init    |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| ni2            | NVP6124B init2   |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| m3i            | MDIN340  init    |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| nn  [B] [A] [D]| NVP6114A debug   |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| nsw            | NVP6114A switch  |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| nv  [B] [A] [D]| NVP6124B debug   |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| mt  [NUM]      | MDIN PATTERN0~9  |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| mc  [NUM]      | MDIN CSC 0~2     |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| ch  [NUM]      | NV6124B CH 0~3   |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| ua [1] [2]     | UART2 DATA PUTS  |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| ut1            | UART2 PUTS TEST1 |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| mr  [1]        | MIRROR TEST1     |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| mul [1]        | MULTI CH TEST1   |\r\n");					//2		[ JYKim 		2015-4-30 ]
	printf("| ls             | UART2 CMD LIST   |\r\n");					//2		[ JYKim 		2015-4-30 ]	
	printf("| help           | DESC             |\r\n");
	printf("|___________________________________|\r\n");
}


