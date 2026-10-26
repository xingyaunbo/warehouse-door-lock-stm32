/***********************************Copy left*****************************************
文件名：    usart3_communication.h
文件描述：  串口3通信
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/

#ifndef _USART3_COMMUNICATION_H_
#define _USART3_COMMUNICATION_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "task.h"
#include "dal_usart3.h"

/****************************************************************
***函数名称：UsartCommunicationInit
***函数功能：串口3通信初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void UsartCommunicationInit(void);

#ifdef __cplusplus
}
#endif

#endif
