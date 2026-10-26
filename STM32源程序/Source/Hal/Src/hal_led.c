/***********************************Copy Left*****************************************
文件名：    hal_led.c
文件描述：  LED的GPIO初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#include "hal_led.h"


/****************************************************************
***函数名称：HAL_LedInit
***函数功能：LED的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_LedInit(void)
{ 
//    //RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;      // 使能PORTB时钟	    
//  	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;      // 使能PORTB时钟
//	   	 
//	GPIOA->CRH &= ~GPIO_CRH_CNF11;            // 设置PB8推挽输出  	 
//	GPIOA->CRH |= GPIO_CRH_MODE11;            // 设置PB8输出速度50MHz 
//    //GPIOA->ODR |= GPIO_ODR_ODR11;             // PB8输出高电平

//    GPIOC->CRH &= ~GPIO_CRH_CNF12;            // 设置PB9推挽输出  	 
//	GPIOC->CRH |= GPIO_CRH_MODE12;            // 设置PB9输出速度50MHz 
//    //GPIOC->ODR |= GPIO_ODR_ODR12;             // PB9输出高电平

	
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = LED1_PIN | BUZZER_PIN ;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(LED1_PRO, &GPIO_InitStruct);    
}






