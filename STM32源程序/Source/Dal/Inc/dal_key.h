/***********************************Copy left*****************************************
文件名：    dal_key.c
文件描述：  KEY的GPIO初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/

#ifndef _DAL_KEY_H_
#define _DAL_KEY_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_key.h"

typedef enum 
{
    DAL_KEY_UP = HAL_KEY_UP,          // 按键高电平
    DAL_KEY_DOWN = HAL_KEY_DOWN,      // 按键低电平
}DAL_KEY_STATE_E;           // 按键状态枚举   

#define DAL_KEY1_STASE()        HAL_KEY1_STASE()              // PA4输入状态
#define DAL_KEY2_STASE()        HAL_KEY2_STASE()              // PA5输入状态
#define DAL_KEY3_STASE()        HAL_KEY3_STASE()              // PA4输入状态
#define DAL_KEY4_STASE()        HAL_KEY4_STASE()              // PA5输入状态

/****************************************************************
***函数名称：DAL_KeyInit
***函数功能：KEY的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_KeyInit(void);

uint8_t Matrix_Key_Scan(void);

#ifdef __cplusplus
}
#endif

#endif

