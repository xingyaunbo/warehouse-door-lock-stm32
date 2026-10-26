/***********************************Copy left*****************************************
文件名：    usart2_communication.h
文件描述：  串口2通信
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/

#ifndef _USART2_COMMUNICATION_H_
#define _USART2_COMMUNICATION_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "task.h"
#include "dal_usart2.h"
#include "SysDoorStatus.h"
#include "key_control.h"
#include "eeprom.h"


#define QUERY_CMD       0X03
#define CONTROL_CMD     0X06
#define SET_CMD         0x09

typedef struct
{
	uint8_t DeviceAdd;
	uint8_t FunctionCode;
	uint8_t ControlAdd;
	uint8_t DataLen;
	uint8_t *ControlData;
	
}_USART_RX_INFO;


/****************************************************************
***函数名称：UsartCommunicationInit
***函数功能：串口2通信初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void Usart2Rs485Init(void);


void Rs485SendData(void);

#ifdef __cplusplus
}
#endif

#endif
