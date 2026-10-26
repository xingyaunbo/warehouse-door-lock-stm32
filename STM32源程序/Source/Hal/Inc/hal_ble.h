#ifndef HAL_BLE_H__
#define HAL_BLE_H__


#include "main.h"



#define BLE_AT_EN_GPIO    		GPIOA
#define BLE_AT_EN_PIN     		LL_GPIO_PIN_1

#define BLE_SLEEP_EN_GPIO    	GPIOA
#define BLE_SLEEP_EN_PIN     	LL_GPIO_PIN_0

extern void  sHal_Ble_init(void);


#endif



