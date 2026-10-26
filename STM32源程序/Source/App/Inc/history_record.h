#ifndef _HISTORY_RECORD_H_
#define _HISTORY_RECORD_H_	 

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "task.h"
#include "eeprom_readwrite.h"
#include "dal_usart2.h"

#define STROGE_HISTORY_EEPROM_ADDDR      0X05	
#define STROGE_HISTORY_EEPROM_LEN        0X02

#define STROGE_HISTORY_MAX_NUM        0X08
 
void sSetHistoryRecordAddr(uint16_t  addr);
uint16_t sGetHistoryRecordAddr(void);
void sSetReadHistoryRecordFlag(uint16_t  addr);
uint8_t sGetReadHistoryRecordFlag(void);
void sReadHistoryRecode(void);//读取历史记录
void sSetClaerHistoryRecordFlag(uint16_t  addr);
uint8_t sGetClaerHistoryRecordFlag(void);
void sHistoryRecodeChk(void);
		 
#ifdef __cplusplus
}
#endif

#endif



