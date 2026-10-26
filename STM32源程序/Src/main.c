/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license, the
  * "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "task.h"
#include "hal_system.h"
#include "led_blink.h"
#include "key_control.h"
#include "eeprom_readwrite.h"
#include "oled.h"
#include "SysDoorStatus.h"
#include "rc522.h"
#include "DataAnalysis.h"
#include "history_record.h"
#include "rtc_time.h"
#include "dal_ble.h"
#include "face_app.h"
#include "dal_esp8266.h"
#include "onenet_app.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	LL_GPIO_AF_Remap_SWJ_NOJTAG();

	SystemClock_Config();

	/****************�����ʼ��***********************/

	DAL_LedInit();
	HAL_Relay_Init();
	DAL_KeyInit();
	Dal_Esp8266Init();		/* ESP8266 WiFiģ��(����2 ��OneNET��) */
	EEPROM_Init();
	DAL_1302Init();
	LL_mDelay(20);
	sBle_Gpio_init();
	sReadSysData();
	OLED_Init();
	OLED_Clear();
	OneNet_WaitOnline();
	sOpenSysDisplay();
	MFRC522Init();
	sFace_Init();		/* ����ģ���ʼ��(����1) */


	/****************Ӧ�ó�ʼ��***********************/
	TaskInit();

	TaskRegister(LedBlink, TASK_TIME_1000MS);           /* LED���� */
	TaskRegister(KeyControl, TASK_TIME_250MS);           /* ����ɨ�輰���ݷ��� */
	TaskRegister(RtcTimeTask, TASK_TIME_970MS);         /* RTCʱ������ */
	TaskRegister(sSysDoorOpenTimeOut, TASK_TIME_100MS); /* �̵������ţ���ʱ�رղ��� */
	TaskRegister(sBuzzerLink, TASK_TIME_100MS); /* ��������������ʱ�رղ��� */
	TaskRegister(MFRC55Request, TASK_TIME_500MS);        /*RC522ˢ����ѯ*/
	TaskRegister(sData_Anlaysis, TASK_TIME_500MS);      /*���ݷ���(NFC)*/
	TaskRegister(sHistoryRecodeChk, TASK_TIME_1000MS);      /*��ȡ��ʷ��¼��־*/
	TaskRegister(FaceTask, TASK_TIME_100MS);		/* ����ʶ������ */
	TaskRegister(OneNetTask, TASK_TIME_100MS);		/* ��ƽ̨�ϱ��·����� */

	TaskStart();

	while (1)
	{

	}
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
