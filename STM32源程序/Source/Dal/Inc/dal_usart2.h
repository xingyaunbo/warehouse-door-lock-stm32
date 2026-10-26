/***********************************Copy left*****************************************
文件名：    dal_usart.h
文件描述：  串口2初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _DAL_USART2_H_
#define _DAL_USART2_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_usart2.h"
#include <stdio.h>
#include <stdarg.h>

#define RECV_MAX_LEN            255 
#define RECV_DATA_HEAD          0xF2
#define SEND_DATA_HEAD          0xF1

#define Rs485SendOn()    RS485_Send_ON()     
#define Rs485SendOff()   RS485_Send_OFF() 	

typedef enum 
{
    DAL_USART2_BAUD_9600        = HAL_USART2_BAUD_9600, 
    DAL_USART2_BAUD_14400       = HAL_USART2_BAUD_14400, 
    DAL_USART2_BAUD_19200       = HAL_USART2_BAUD_19200, 
    DAL_USART2_BAUD_38400       = HAL_USART2_BAUD_38400, 
    DAL_USART2_BAUD_57600       = HAL_USART2_BAUD_57600, 
    DAL_USART2_BAUD_76800       = HAL_USART2_BAUD_76800, 
    DAL_USART2_BAUD_115200      = HAL_USART2_BAUD_115200, 
} DAL_USART2_BADU_E;            // USART3串口波特率枚举

/****************************************************************
***函数名称：DAL_Usart2Init
***函数功能：串口2初始化
***输入参数：baud, 波特率
***返回值：无
***备注：无
*****************************************************************/
void DAL_Usart2Init(unsigned int baud);

/****************************************************************
***函数名称：DAL_Usart2SendByte
***函数功能：串口2发送数据
***输入参数：byte，发送一个字节
***返回值：无
***备注：无
*****************************************************************/
extern void DAL_Usart2SendByte(unsigned char byte);

extern uint8_t sReadUart2RxFlag(void);
extern void sSetRX_FLAG(uint8_t flag);
extern void sReadUart2RxBuff(uint8_t* buff);
extern void sDal_Usart2SendData(uint8_t* buff, uint16_t len);

extern void DEBUG_uart2(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif

