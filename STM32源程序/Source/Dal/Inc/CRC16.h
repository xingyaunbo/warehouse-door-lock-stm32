/***********************************Copy left*****************************************
文件名：    Dal_CRC16.h
文件描述：  CRC校验计算
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _DAL_CRC16_H_
#define _DAL_CRC16_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

uint16_t Crc16(uint8_t *ptr, uint32_t len);

#define HIG_UINT16(a)   ( ((a)>>8) & 0xFF )
#define LOW_UINT16(a)   ( (a) & 0xFF )

#ifdef __cplusplus
}
#endif

#endif


