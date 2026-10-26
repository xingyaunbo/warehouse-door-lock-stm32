/***********************************Copy Left*****************************************
文件名：    hal_1302.c
文件描述：  DS1302初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#include "hal_1302.h"


/****************************************************************
***函数名称：HAL_1302Init
***函数功能：1302 GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_1302Init(void)
{   
//    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;      // 使能PORTC时钟	      	
//	   	 
//	GPIOC->CRH &= ~GPIO_CRH_CNF13;            // 设置PC13推挽输出    
//    //GPIOC->CRH |= GPIO_CRH_CNF13_0;
//    
//	GPIOC->CRH &= ~GPIO_CRH_MODE13;            // 设置PC13输出速度50MHz 
//    GPIOC->CRH |= GPIO_CRH_MODE13;              
//    
//    GPIOC->CRH &= ~GPIO_CRH_CNF14;             // 设置PC14推挽输出    
//    //GPIOC->CRH |= GPIO_CRH_CNF14_0;
//    
//	GPIOC->CRH &= ~GPIO_CRH_MODE14;            // 设置PC14输出速度50MHz 
//    GPIOC->CRH |= GPIO_CRH_MODE14;            
//    
//    GPIOC->CRH &= ~GPIO_CRH_CNF15;             // 设置PC15推挽输出    
//   // GPIOC->CRH |= GPIO_CRH_CNF15_0;
//    
//	GPIOC->CRH &= ~GPIO_CRH_MODE15;            // 设置PC15输出速度50MHz 
//    GPIOC->CRH |= GPIO_CRH_MODE15; 
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

    GPIO_InitStruct.Pin = HAL_1302_SDA | HAL_1302_SCL | HAL_1302_CS;       
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;             
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;         
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(HAL_1302_PRO, &GPIO_InitStruct);
	
	LL_GPIO_ResetOutputPin(HAL_1302_PRO, HAL_1302_CS);
	LL_GPIO_SetOutputPin(HAL_1302_PRO, HAL_1302_SCL | HAL_1302_SDA);
}






