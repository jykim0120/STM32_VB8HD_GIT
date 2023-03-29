/*
 * kjy_ctrl.h
 *
 *  Created on: 2023. 1. 30.
 *      Author: Administrator
 */

#ifndef KJY_CTRL_H_
#define KJY_CTRL_H_




void KJY_DATA_SEND_GPIO_CHECK(void);

void KJY_LED_R_INIT(void);
void KJY_LED_R_CLEAR(void);
void KJY_LED_R_REFRESH(void);
void KJY_LED_R_CTRL(void);


void KJY_TRIGGER_CTRL(void);
void KJY_TRIGGER_FLAG_INIT(BYTE _ch_data);
void KJY_TRIGGER_FLAG_CLEAR(void);

void KJY_AUTO_POWER(BYTE dat);
void KJY_AUTO_SCAN(BYTE dat);
void KJY_TRIGGER_IN(BYTE dat);
void KJY_TRIGGER_TIME(BYTE dat);
void KJY_TRIGGER_PRIORITY(BYTE dat);

void KJY_AUTO_SCAN_INIT(void);
void KJY_AUTO_SCAN_CLEAR(void);
void KJY_AUTO_SCAN_CTRL(void);

void KJY_CAM_SCANTIME(BYTE dat);

void KJY_MENU_PROCESS(BYTE dat);

void KJY_CAM1_MIRROR(BYTE dat);
void KJY_CAM2_MIRROR(BYTE dat);
void KJY_CAM3_MIRROR(BYTE dat);
void KJY_CAM4_MIRROR(BYTE dat);
void KJY_CURRENT_CHANNEL_MIRROR_CTRL();

void KJY_OSD(BYTE dat);
void KJY_CAM_STATUS_PROCESS(void);





#endif /* KJY_CTRL_H_ */
