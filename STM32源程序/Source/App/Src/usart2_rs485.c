/***********************************Copy left*****************************************
文件名：    usart3_communication.h
文件描述：  串口3通信
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#include "usart2_rs485.h"
#include "stm32f1xx_ll_gpio.h"
void sUart2DataAnalysis(void);
/****************************************************************
***函数名称：UsartCommunicationInit
***函数功能：串口3通信初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void Usart2Rs485Init(void)
{
    DAL_Usart2Init(DAL_USART2_BAUD_115200);
    DAL_Usart2SendByte(0xAA);
}

/****************************************************************
***函数名称：UsartCommunicationInit
***函数功能：串口3通信初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void Rs485SendData(void)
{
#if 0    //心跳包数据测试
	uint16_t crcvalue = 0;
	uint8_t buff[7]={1,2,3,4,5};

	crcvalue = Crc16(buff, 5) ;
	buff[6]=HIG_UINT16(crcvalue);
	buff[5]=LOW_UINT16(crcvalue);
	sDal_Usart2SendData(buff,7);
#endif
	//sUart2DataAnalysis();   //与RC522一起解析
	
}


