/***********************************Copy Left*****************************************
文件名：    hal_key.c
文件描述：  KEY的GPIO初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#include "hal_key.h"


/****************************************************************
***函数名称：HAL_KeyInit
***函数功能：KEY的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void sHAL_Matrix_Key_Init(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 |LL_GPIO_PIN_6|LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10 | LL_GPIO_PIN_11 |LL_GPIO_PIN_12|LL_GPIO_PIN_13 ;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
}






