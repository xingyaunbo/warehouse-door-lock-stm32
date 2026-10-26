/***********************************Copy Left*****************************************
文件名：    eeprom.h
文件描述：  eeprom读写函数，适合24C01~24C16 
创建时间：  2023/03/02
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _EEPROM_H_
#define _EEPROM_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_iic.h"

/****************************************************************
***函数名称：EEPROM_Init
***函数功能：EEPROM接口初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void EEPROM_Init(void);

/****************************************************************
***函数名称：EEPROM_WriteData
***函数功能：指定地址写入一串数据
***输入参数： writeAddr，写入地址，writeData, 写入的数据，writeLen，写入长度
***返回值：1，正在写入，0，写入完成
***备注：只有返回0时，说明写入完成，因为写数据时，此函数比较周期调用，
因为EEPROM写入速度比较慢，建议调用此函数的时间超过10ms
*****************************************************************/
uint8_t EEPROM_WriteData(uint16_t writeAddr, uint8_t *writeBuf, uint8_t writeLen);

/****************************************************************
***函数名称：EEPROM_ReadData
***函数功能：指定地址读取一串数据
***输入参数： readAddr，读取首地址，readBuf, 读取的缓存，readLen，读取长度
***返回值：1，正在写入，0，写入完成
***备注：只有返回0时，说明写入完成，因为读数据时，此函数比较周期调用，
因为EEPROM写入速度比较慢，建议调用此函数的时间超过10ms
*****************************************************************/
uint8_t EEPROM_ReadData(uint16_t readAddr, uint8_t *readBuf, uint8_t readLen);

uint8_t EEPROM_WriteOneByte(uint16_t writeAddr, uint8_t byte);
uint8_t EEPROM_WriteContinuous(uint16_t writeAddr, uint8_t len ,uint8_t *buff);
uint8_t EEPROM_ReadContinuous(uint16_t readAddr, uint8_t *byte,uint8_t len);

#ifdef __cplusplus
}
#endif

#endif















