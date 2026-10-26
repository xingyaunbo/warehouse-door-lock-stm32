/***********************************Copy left*****************************************
文件名：    hal_relay.h
文件描述：  KEY的GPIO初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_RELAY_H_
#define _HAL_RELAY_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define DOOR_RELAY_GPIO    GPIOA
#define DOOR_RELAY_PIN     LL_GPIO_PIN_8

void HAL_Relay_Init(void);

#ifdef __cplusplus
}
#endif

#endif

