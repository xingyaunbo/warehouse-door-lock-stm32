/***********************************Copy Left*****************************************
�ļ����ƣ�  onenet_app.h
�ļ�������  OneNET��ƽ̨Ӧ�ò�(��ģ�������ϱ�/�·�ִ��)
����ʱ�䣺  2026/10/22

***************************************************************************************/
#ifndef _ONENET_APP_H_
#define _ONENET_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ��ģ������ */
#define ONENET_AUTH_FACE      1     /* ��֤��ʽ: ���� */
#define ONENET_AUTH_NFC       2     /* NFC */
#define ONENET_AUTH_PASSWORD  3     /* ���� */
#define ONENET_AUTH_REMOTE    4     /* Զ�� */

#define ONENET_LOCK_OPEN      0     /* ����״̬: �� */
#define ONENET_LOCK_CLOSE     1     /* �� */

void OneNetTask(void);
void OneNet_WaitOnline(void);                    /* wait cloud online at boot */                          /* ��ƽ̨���� 100ms */
void OneNet_ReportAuth(uint8_t type, uint8_t ok);  /* ��֤�¼��ϱ� */
void OneNet_ReportLock(uint8_t state);             /* ����״̬�ϱ� */
void OneNet_FaceCountAdd(void);                   /* ��������+1���ϱ� */
void OneNet_FaceCountSet(uint8_t cnt);            /* ���������������ϱ� */

#ifdef __cplusplus
}
#endif

#endif
