#ifndef _DAL_KEY_H_
#define _DAL_KEY_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_spi.h"

/****************************************************************
***函数名称：DAL_SPI1_Init
***函数功能：SPI1初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_SPI1_Init(void);

/****************************************************************
***函数名称：DAL_SPI1_ReadWriteByte
***函数功能：SPI1发送一个字节
***输入参数：byte, 要发送的字节
***返回值：读取到的字节
***备注：
*****************************************************************/
unsigned char DAL_SPI1_ReadWriteByte(unsigned char byte);

		 
#ifdef __cplusplus
}
#endif

#endif



