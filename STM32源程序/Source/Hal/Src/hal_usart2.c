/***********************************Copy Left*****************************************
文件名：    hal_usart3.c
文件描述：  串口3初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#include "hal_usart2.h"
#define USAR2_PLCK          36000000UL      // 串口3使用PCLK1为36MHz 串口1使用PCLK2为72MHz

/****************************************************************
***函数名称：HAL_Usart3Init
***函数功能：串口3初始化
***输入参数：baud, 波特率
***返回值：无
***备注：波特率计算公式为 baud = FCLK / (16 * USARTDIV)
*****************************************************************/
void HAL_Usart2Init(unsigned int baud)
{
	
	    unsigned short mantissa;
    unsigned char fraction;
    float usartdiv;
	
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
  
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration  
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  PA1   ------> RS485_DE 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_1;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  USART_InitStruct.BaudRate = baud;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  LL_USART_Init(USART2, &USART_InitStruct);
    
  //LL_USART_ConfigSyncMode(USART2);
  LL_USART_EnableIT_RXNE(USART2);
  LL_USART_EnableIT_IDLE(USART2);
  LL_USART_Enable(USART2);

}

/****************************************************************
***函数名称：HAL_Usart2SendByte
***函数功能：串口2发送数据
***输入参数：byte，发送一个字节
***返回值：无
***备注：无
*****************************************************************/
void HAL_Usart2SendByte(unsigned char byte)
{
    USART2->DR = byte;
}

/****************************************************************
***函数名称：HAL_Usart2SendData
***函数功能：串口2发送数据
***输入参数：data, 要发送的数据指针，
***返回值：无
***备注：无
*****************************************************************/
void HAL_Usart2SendData(unsigned char *data, unsigned int len)
{
    if (data != NULL)
    {
        
    }
}




