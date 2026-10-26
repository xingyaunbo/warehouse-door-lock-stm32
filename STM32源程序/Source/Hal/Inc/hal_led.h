/***********************************Copy Left*****************************************
文件名：    hal_led.c
文件描述：  LED的GPIO初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_LED_H_
#define _HAL_LED_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define BUZZER_PRO     	GPIOA
#define BUZZER_PIN 		LL_GPIO_PIN_11

#define LED1_PRO     	GPIOA
#define LED1_PIN 		LL_GPIO_PIN_12

#define HAL_LED1_ON()         LED1_PRO->ODR |= LED1_PIN                //高电平
#define HAL_LED1_OFF()        LED1_PRO->ODR &= ~LED1_PIN               

#define HAL_BUZZER_ON()         LL_GPIO_SetOutputPin(BUZZER_PRO, BUZZER_PIN)// BUZZER_PRO->ODR |= BUZZER_PIN                //高电平
#define HAL_BUZZER_OFF()        LL_GPIO_ResetOutputPin(BUZZER_PRO, BUZZER_PIN)//BUZZER_PRO->ODR &= ~BUZZER_PIN     
#define HAL_BUZZER_TOGGLE()     LL_GPIO_TogglePin(BUZZER_PRO, BUZZER_PIN)//蜂鸣器取反

/****************************************************************
***函数名称：HAL_LedInit
***函数功能：LED的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_LedInit(void);

#ifdef __cplusplus
}
#endif

#endif

