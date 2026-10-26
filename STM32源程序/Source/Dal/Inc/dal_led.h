/***********************************Copy left*****************************************
文件名：    dal_led.c
文件描述：  LED的GPIO初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _DAL_LED_H_
#define _DAL_LED_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_led.h"

#define DAL_LED1_ON()               HAL_LED1_ON()              // PB9输出高电平
#define DAL_LED1_OFF()              HAL_LED1_OFF()             // PB9输出低电平

#define DAL_LED0_ON()               HAL_LED0_ON()              // PB8输出高电平
#define DAL_LED0_OFF()              HAL_LED0_OFF()             // PB8输出低电平



/****************************************************************
***函数名称：HAL_LedInit
***函数功能：LED的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_LedInit(void);

#ifdef __cplusplus
}
#endif

#endif

