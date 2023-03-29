#include	<stdio.h>
#include	<string.h>
#include	<stdarg.h>
#include	"..\video\video.h"
#include 	"..\common\common.h"

typedef enum {
	MSG_PRODUCT=0,
	MSG_FIRMWARE,
	MSG_DECR,
	MSG_DECW,
	MSG_DECRG,
	MSG_DECWG,
} 	MSG_DEC;


static ROMDATA BYTE CMD_PRODUCT[]  = "#PRODUCT";
static ROMDATA BYTE CMD_FIRMWARE[] = "#FIRMWARE";
static ROMDATA BYTE CMD_DECR[]     = "#RI2C?";
static ROMDATA BYTE CMD_DECW[]     = "#RI2C=";
static ROMDATA BYTE CMD_DECWG[]    = "#RI2S=";
static ROMDATA BYTE CMD_DECRG[]    = "#RI2S?";

#define MAX_COMPARE_LEN	6

static ROMDATA BYTE SEND_PRODUCT[]  = "#NVP1918C\r";
static ROMDATA BYTE SEND_FIRMWARE[] = "#2014.03.11\r";
static ROMDATA BYTE SEND_WOK[]      = "#OK\r";
static ROMDATA BYTE SEND_ROK[]      = "#data="; 

extern BYTE Serial2RxCount, Serial2RxData[50];


void RxCommand(PNCDEC_MSG pMSG)
{
	if(strncmp(Serial2RxData,CMD_DECR,MAX_COMPARE_LEN) ==0)
	{
		Serial2RxCount = 0;
		pMSG->cmd = MSG_DECR;
		memcpy(pMSG->buf,&Serial2RxData[6],strlen(Serial2RxData)-strlen(CMD_DECR));  // Cupy number include "CR"
	}
	else if(strncmp(Serial2RxData,CMD_DECW, MAX_COMPARE_LEN) ==0)
	{
		Serial2RxCount = 0;
		pMSG->cmd = MSG_DECW;
		memcpy(pMSG->buf,&Serial2RxData[6],strlen(Serial2RxData)-strlen(CMD_DECW));  // Cupy number include "CR"
	}
	else if(strncmp(Serial2RxData,CMD_DECWG, MAX_COMPARE_LEN) ==0)
	{
		Serial2RxCount = 0;
		pMSG->cmd = MSG_DECWG;
		memcpy(pMSG->buf,&Serial2RxData[6],strlen(Serial2RxData)-strlen(CMD_DECWG));  // Cupy number include "CR"
	}
	else if(strncmp(Serial2RxData,CMD_DECRG, MAX_COMPARE_LEN) ==0)
	{
		Serial2RxCount = 0;
		pMSG->cmd = MSG_DECRG;
		memcpy(pMSG->buf,&Serial2RxData[6],strlen(Serial2RxData)-strlen(CMD_DECRG));  // Cupy number include "CR"
	}
	else if(strncmp(Serial2RxData,CMD_PRODUCT, MAX_COMPARE_LEN) == 0)
	{
		Serial2RxCount = 0;
		pMSG->cmd = MSG_PRODUCT;
	}
	else if(strncmp(Serial2RxData,CMD_FIRMWARE, MAX_COMPARE_LEN) ==0)
	{
		Serial2RxCount = 0;
		pMSG->cmd = MSG_FIRMWARE;
	}
}

void TxCommand(PNCDEC_MSG pMSG)
{
	if(pMSG->cmd == MSG_DECR)
	{
		BYTE i,j;
		BYTE data[3],str[10];
		CHAR *endptr;
		
		i=0;j=0;
		while (pMSG->buf[i] != '\r')				//2 [JYKIM-2023-1-4]	hex : 0xD  (carriage feed)
		{
			str[0] = pMSG->buf[i];
			str[1] = pMSG->buf[i+1];
			data[j] = strtol( str, &endptr, 16);
			i+=2;
			j++;
		}
		memset(str,0x00,sizeof(str));
		sprintf(str,"%s%02x\r",SEND_ROK,NVP1918C_RegRead(data[1]));
		printf("string is = %s\r\n",str);
		Serial_PutString(str);
	}
	else if(pMSG->cmd == MSG_DECW)
	{
		BYTE i,j;
		BYTE data[4],str[2];  //data[0]:S.A data[1]:addr, data[2]:data
		CHAR *endptr;

		i=0;j=0;
		while (pMSG->buf[i] != '\r')
		{
			str[0] = pMSG->buf[i];
			str[1] = pMSG->buf[i+1];
			data[j] = strtol( str, &endptr, 16);
			i+=2;
			j++;
		}
		NVP1918C_RegWrite(data[1], data[2]);
		Serial_PutString(SEND_WOK);
	}
	else if(pMSG->cmd == MSG_DECWG)
	{
		BYTE i,j;
		BYTE data[19],str[2];  //data[0]:S.A data[1]:addr, data[2]:data
		CHAR *endptr;
		i=0;j=0;

		Serial_PutString(SEND_WOK);
		
		while (pMSG->buf[i] != '\r')
		{
			str[0] = pMSG->buf[i];
			str[1] = pMSG->buf[i+1];
			data[j] = strtol( str, &endptr, 16);
			i+=2;	j++;
		}

		for(i = 0; i < 16; i++)  // write 16 commands
			NVP1918C_RegWrite(data[1]+i, data[2+i]);
		
		
	}
	else if(pMSG->cmd == MSG_DECRG)
	{
		BYTE i,j;
		BYTE data[3],readdata[16];
		BYTE str[50];
		CHAR *endptr;
		WORD checksum = 0;
		
		i=0;j=0;
		
		while (pMSG->buf[i] != '\r')
		{
			str[0] = pMSG->buf[i];
			str[1] = pMSG->buf[i+1];
			data[j] = strtol( str, &endptr, 16);
			i+=2;
			j++;
		}
		j=0;
		memset(str,0x00,sizeof(str));
		sprintf(&str[j],"%s",SEND_ROK);
		for(i = 0 ; i < 16; i++)
		{
			readdata[i] = NVP1918C_RegRead(data[1]+i);
			sprintf(&str[6+j],"%02x",readdata[i]);
			checksum += readdata[i];
			j+=2;
		}
		sprintf(&str[6+j],"%02x\r",checksum&0xFF);  //checksum
		
		Serial_PutString(str);
		//printf("str = %s, len = %d\r\n",str, strlen(str));
	}
	else if(pMSG->cmd == MSG_PRODUCT)
	{
		Serial_PutString(SEND_PRODUCT);
	}
	else if(pMSG->cmd == MSG_FIRMWARE)
	{
		Serial_PutString(SEND_FIRMWARE);
	}
}


