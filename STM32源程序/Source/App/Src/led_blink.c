/***********************************Copy Left*****************************************
文件名：    led_blink.c
文件描述：  led应用
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/

#include "led_blink.h"
#include "main.h"


uint8_t BuzzerStatus = 0;

uint8_t sGetBuzzerStatus(void)
{
	return BuzzerStatus;
}

void sSetBuzzerStatus(uint8_t status)
{
	BuzzerStatus = status ;
}

/****************************************************************
***函数名称：LedBlink
***函数功能：LED闪灯
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void LedBlink(void)
{
	LL_GPIO_TogglePin(GPIOA, LL_GPIO_PIN_12);//LED0_PIN | LED1_PIN);//两个灯一起闪烁
}

void sBuzzerLink(void)
{
	static uint8_t buzzerCountTime = 0;
	
	if( BuzzerStatus == 1)
	{
		if(buzzerCountTime%2)
		{
			HAL_BUZZER_TOGGLE();
		}
		buzzerCountTime++;
		
		if(buzzerCountTime>=7)
		{
			HAL_BUZZER_OFF();
			buzzerCountTime = 0;
			BuzzerStatus = 0;
			
		}
	}
	else if( BuzzerStatus == 2)
	{
		HAL_BUZZER_ON();
		buzzerCountTime++;
		if( buzzerCountTime>=8)
		{
			buzzerCountTime = 0;
			BuzzerStatus = 0;
			HAL_BUZZER_OFF();
		}
	}
//	else
//	{
//		BuzzerStatus = 0;
//		buzzerCountTime = 0;
//		HAL_BUZZER_OFF();
//	}
}


