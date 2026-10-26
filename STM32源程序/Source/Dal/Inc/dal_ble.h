#ifndef DAL_BLE_H__
#define DAL_BLE_H__


#include  "hal_ble.h"



extern void  sBle_Gpio_init(void);

#define BLE_AT_ENABLE()     	 LL_GPIO_SetOutputPin(BLE_AT_EN_GPIO, BLE_AT_EN_PIN);
#define BLE_AT_DISENABLE()       LL_GPIO_ResetOutputPin(BLE_AT_EN_GPIO, BLE_AT_EN_PIN);


#define BLE_SLEEP_ENABLE()    	 LL_GPIO_SetOutputPin(BLE_SLEEP_EN_GPIO, BLE_SLEEP_EN_PIN);
#define BLE_SLEEP_DISENABLE()    LL_GPIO_ResetOutputPin(BLE_SLEEP_EN_GPIO, BLE_SLEEP_EN_PIN);












#endif


