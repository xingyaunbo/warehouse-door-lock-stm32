#ifndef HAL_BLE_C__
#define HAL_BLE_C__




#include  "hal_ble.h"



void  sHal_Ble_init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    GPIO_InitStruct.Pin = BLE_AT_EN_PIN | BLE_SLEEP_EN_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(BLE_AT_EN_GPIO, &GPIO_InitStruct);

    LL_GPIO_ResetOutputPin(BLE_AT_EN_GPIO, BLE_AT_EN_PIN | BLE_SLEEP_EN_PIN);
}




#endif






