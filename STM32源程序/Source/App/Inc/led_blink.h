/***********************************Copy left*****************************************
文件名：    led_blink.h
文件描述：  led应用
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _LED_BLINK_H_
#define _LED_BLINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dal_led.h"
extern uint8_t sGetBuzzerStatus(void);
extern void sSetBuzzerStatus(uint8_t status);
/****************************************************************
***函数名称：LedBlink
***函数功能：LED闪灯
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void LedBlink(void);
void sBuzzerLink(void);

#ifdef __cplusplus
}
#endif

#endif 
