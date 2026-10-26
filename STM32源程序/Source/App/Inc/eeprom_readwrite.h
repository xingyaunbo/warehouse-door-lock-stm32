/***********************************Copy Left*****************************************
文件名：    eeprom_readwrite.h
文件描述：  eeprom读写测试，
创建时间：  2023/03/02
修改时间： 
修改内容；

***************************************************************************************/

#ifndef _EEPROM_READWRITE_H_
#define _EEPROM_READWRITE_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "eeprom.h"
#include "SysDoorStatus.h"
#include "key_control.h"


#define IC_MAX_NUM     5

#define STORGE_LEN     20


typedef struct
{
	uint8_t year_h;   //年 高位
	uint8_t year_l;   //年低位
	uint8_t month;   //月
	uint8_t day;   //日
	uint8_t hour;   //时
	uint8_t minute;   //分
	uint8_t IC_id[4];  //IC卡ID
	uint8_t flag;      //是否有效标志位
	uint8_t NumID[9];  //学号 姓名

}_INFORMATION;


extern _INFORMATION stInformation[];

/****************************************************************
***函数名称：EEPROM_ReadWriteInit
***函数功能：EEPROM读写测试初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void sSetReadWriteMode(uint8_t flag);
uint8_t sGetReadAndWriteMode(void);
void EEPROM_ReadWriteInit(void);
void ReadWriteDataTask(void);
void sReadSysData(void);
    
#ifdef __cplusplus
}
#endif

#endif
