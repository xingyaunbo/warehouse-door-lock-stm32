
/***********************************Copy left*****************************************
文件名：    dal_relay.h
文件描述：  
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _DAL_RELAY_H_
#define _DAL_RELAY_H_	 

#ifdef __cplusplus
extern "C" 
{
#endif

#include "hal_relay.h"
#include "stdint.h"
#define OpenDoor()     LL_GPIO_SetOutputPin(DOOR_RELAY_GPIO,DOOR_RELAY_PIN)  

#define CloseDoor()  	LL_GPIO_ResetOutputPin(DOOR_RELAY_GPIO,DOOR_RELAY_PIN)

void DAL_Relay_Init(void);
void sCloseDoor(void);
void sOpenDoor(void);

#ifdef __cplusplus
}
#endif

#endif
