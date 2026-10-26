/***********************************Copy left*****************************************
文件名：   SysDoorStatus.h
文件描述：  
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _SYSDOORSTATUS_H_
#define _SYSDOORSTATUS_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "dal_relay.h"
#include "oled.h"


extern uint8_t sGetDoorStatus(void);
extern void sSetDoorStatus(uint8_t status);
void sSysDoorOpenTimeOut(void);

#ifdef __cplusplus
}
#endif

#endif


