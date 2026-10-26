/***********************************Copy left*****************************************
文件名：    hal_usart.h
文件描述：  串口3初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_USART3_H_
#define _HAL_USART3_H_	 

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

typedef enum 
{
    HAL_USART3_BAUD_9600        = 9600, 
    HAL_USART3_BAUD_14400       = 14400, 
    HAL_USART3_BAUD_19200       = 19200, 
    HAL_USART3_BAUD_38400       = 38400, 
    HAL_USART3_BAUD_57600       = 57600, 
    HAL_USART3_BAUD_76800       = 76800, 
    HAL_USART3_BAUD_115200      = 115200, 
} HAL_USART3_BADU_E;            // USART3串口波特率枚举

/****************************************************************
***函数名称：HAL_Usart3Init
***函数功能：串口3初始化
***输入参数：baud, 波特率
***返回值：无
***备注：波特率计算公式为 baud = FCLK / (16 * USARTDIV)
*****************************************************************/
void HAL_Usart3Init(unsigned int baud);

/****************************************************************
***函数名称：HAL_Usart3SendByte
***函数功能：串口3发送数据
***输入参数：byte，发送一个字节
***返回值：无
***备注：无
*****************************************************************/
void HAL_Usart3SendByte(unsigned char byte);

#ifdef __cplusplus
}
#endif

#endif


