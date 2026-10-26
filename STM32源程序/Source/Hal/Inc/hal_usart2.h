/***********************************Copy left*****************************************
文件名：    hal_usart.h
文件描述：  串口2初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_USART2_H_
#define _HAL_USART2_H_	 

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "CRC16.h"
#define RS485_Send_OFF()           LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_1);
#define RS485_Send_ON() 		   LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_1);


typedef enum 
{
    HAL_USART2_BAUD_9600        = 9600, 
    HAL_USART2_BAUD_14400       = 14400, 
    HAL_USART2_BAUD_19200       = 19200, 
    HAL_USART2_BAUD_38400       = 38400, 
    HAL_USART2_BAUD_57600       = 57600, 
    HAL_USART2_BAUD_76800       = 76800, 
    HAL_USART2_BAUD_115200      = 115200, 
} HAL_USART2_BADU_E;            // USART3串口波特率枚举

/****************************************************************
***函数名称：HAL_Usart2Init
***函数功能：串口2初始化
***输入参数：baud, 波特率
***返回值：无
***备注：波特率计算公式为 baud = FCLK / (16 * USARTDIV)
*****************************************************************/
void HAL_Usart2Init(unsigned int baud);

/****************************************************************
***函数名称：HAL_Usart3SendByte
***函数功能：串口2发送数据
***输入参数：byte，发送一个字节
***返回值：无
***备注：无
*****************************************************************/
void HAL_Usart2SendByte(unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif


