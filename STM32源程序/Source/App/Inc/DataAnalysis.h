#ifndef _DATA_ANALYSIS_H_
#define _DATA_ANALYSIS_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "rc522.h"
#include "usart2_rs485.h"
#include "task.h"
#include "eeprom_readwrite.h"

typedef enum
{
	DATA_CONTENT_ERROR = 1,
	DATA_LEN_ERROR,
	
}_ERROR_TYPE;

extern void sData_Anlaysis(void);
		 
#ifdef __cplusplus
}
#endif

#endif



