/***********************************Copy left*****************************************
文件名：    hal_key.c
文件描述：  KEY的GPIO初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_KEY_H_
#define _HAL_KEY_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


typedef enum 
{
    HAL_KEY_UP = 0,         // 按键高电平
    HAL_KEY_DOWN = 1,       // 按键低电平
}HAL_KEY_STATE_E;           // 按键状态枚举

#define HAL_KEY1_STASE()      (GPIOA->IDR & GPIO_IDR_IDR4)                // PA4输入状态
#define HAL_KEY2_STASE()      (GPIOA->IDR & GPIO_IDR_IDR5)                // PA5输入状态
#define HAL_KEY3_STASE()      (GPIOA->IDR & GPIO_IDR_IDR6)                // PA6输入状态
#define HAL_KEY4_STASE()      (GPIOA->IDR & GPIO_IDR_IDR7)                // PA7输入状态

/****************************************************************
***函数名称：HAL_KeyInit
***函数功能：KEY的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_KeyInit(void);

void sHAL_Matrix_Key_Init(void);

#ifdef __cplusplus
}
#endif

#endif

