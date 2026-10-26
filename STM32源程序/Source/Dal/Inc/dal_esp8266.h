/***********************************Copy Left*****************************************
�ļ����ƣ�  dal_esp8266.h
�ļ�������  ESP8266 WiFiģ������(USART2 PA2/PA3 115200)
            OneNET MQTT���� ������AT״̬��
����ʱ�䣺  2026/10/22

***************************************************************************************/
#ifndef _DAL_ESP8266_H_
#define _DAL_ESP8266_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

void Dal_Esp8266Init(void);                     /* ģ���ʼ��(����2 115200) */
void Esp8266Machine(void);                       /* ״̬�� 100ms���� */
uint8_t Esp8266IsOnline(void);
uint8_t Esp8266GetStage(void);                  /* 0:idle 1:wifi 2:online */                   /* �ƶ��Ƿ����� */
uint8_t Esp8266PublishReq(const char *json);     /* ���󷢲��ϱ����� ����1���� */
uint8_t Esp8266ReplyReq(const char *json);       /* ���󷢲���������Ӧ�� ����1���� */
uint8_t Esp8266FetchSetCmd(char *json);         /* ȡ��ƽ̨�·�����������JSON */

#ifdef __cplusplus
}
#endif

#endif
