/*
 * coaxial.h
 *
 *  Created on: 2014. 1. 22.
 *      Author: jwpark
 */

#ifndef COAXIAL_H_
#define COAXIAL_H_

#ifndef		__MDINTYPE_H__
#include	"..\drivers\mdintype.h"
#endif

#if defined(__NVP1918C_RX__)

	#define PEL_D0	0x70
	#define PEL_D1	0x71
	#define PEL_D2	0x72
	#define PEL_D3	0x73
	#define PEL_OUT	0x5F

	#define PEL_CTEN	0x5C
	#define PEL_TXST1	0x57
	#define EVEN_LINE	0x7F

	#define COAX_CH_SEL	0x8B
	#define COAX_BAUD	0x50
	#define COAX_RBAUD	0x51
	#define PEL_BAUD	0x52
	#define BL_TXST1	0x53
	#define BL_RXST1	0x55
	#define BL_HSP01	0x5D
	#define BL_HSP02	0x5E
	#define PACKET_MODE	0x5B
	#define TX_START	0x59
	#define TX_BYTE_LEN	0x5A

	#define SAM_D0	0x60
	#define SAM_OUT	0x59

#else

	#define PEL_D0	0xA0
	#define PEL_D1	0xA1
	#define PEL_D2	0xA2
	#define PEL_D3	0xA3
	#define PEL_OUT	0x8F

	#define PEL_CTEN	0x8C
	#define PEL_TXST1	0x87
	#define EVEN_LINE	0xAF

	#define COAX_CH_SEL	0xBB
	#define COAX_BAUD	0x80
	#define COAX_RBAUD	0x81
	#define PEL_BAUD	0x82
	#define BL_TXST1	0x83
	#define BL_RXST1	0x85
	#define BL_HSP01	0x8D
	#define BL_HSP02	0x8E
	#define PACKET_MODE	0x8B
	#define TX_START	0x89
	#define TX_BYTE_LEN	0x8A

	#define SAM_D0	0x90
	#define SAM_OUT	0x89

#endif


#define SS_CMD_SET			0
#define SS_CMD_UP			1
#define SS_CMD_DOWN			2
#define SS_CMD_LEFT			3
#define SS_CMD_RIGHT		4
#define SS_CMD_OSD			5
#define SS_CMD_PTZ_UP		6
#define SS_CMD_PTZ_DOWN		7
#define SS_CMD_PTZ_LEFT		8
#define SS_CMD_PTZ_RIGHT	9
#define SS_CMD_IRIS_OPEN	10
#define SS_CMD_IRIS_CLOSE	11
#define SS_CMD_FOCUS_NEAR	12
#define SS_CMD_FOCUS_FAR	13
#define SS_CMD_ZOOM_WIDE	14
#define SS_CMD_ZOOM_TELE	15
#define SS_CMD_SCAN_SR		16
#define SS_CMD_SCAN_ST		17
#define SS_CMD_PRESET1		18
#define SS_CMD_PRESET2		19
#define SS_CMD_PRESET3		20
#define SS_CMD_PTN1_SR		21
#define SS_CMD_PTN1_ST		22
#define SS_CMD_PTN2_SR		23
#define SS_CMD_PTN2_ST		24
#define SS_CMD_PTN3_SR		25
#define SS_CMD_PTN3_ST		26

#define PELCO_CMD_RESET			0
#define PELCO_CMD_SET			1
#define PELCO_CMD_UP			2
#define PELCO_CMD_DOWN			3
#define PELCO_CMD_LEFT			4
#define PELCO_CMD_RIGHT			5
#define PELCO_CMD_OSD			6
#define PELCO_CMD_IRIS_OPEN		7
#define PELCO_CMD_IRIS_CLOSE	8
#define PELCO_CMD_FOCUS_NEAR	9
#define PELCO_CMD_FOCUS_FAR		10
#define PELCO_CMD_ZOOM_WIDE		11
#define PELCO_CMD_ZOOM_TELE		12
#define PELCO_CMD_SCAN_SR		13
#define PELCO_CMD_SCAN_ST		14
#define PELCO_CMD_PRESET1		15
#define PELCO_CMD_PRESET2		16
#define PELCO_CMD_PRESET3		17
#define PELCO_CMD_PTN1_SR		18
#define PELCO_CMD_PTN1_ST		19
#define PELCO_CMD_PTN2_SR		20
#define PELCO_CMD_PTN2_ST		21
#define PELCO_CMD_PTN3_SR		22
#define PELCO_CMD_PTN3_ST		23
#define PELCO_CMD_RUN			24
#define PELCO_CMD_COMET_OFF		152
#define PELCO_CMD_COMET_ON		153


void pelco_coax_mode( void );
BYTE pelco_command(BYTE command);
BYTE samsung_coax_command(BYTE command);
void samsung_coax_mode( void );
#endif /* COAXIAL_H_ */
