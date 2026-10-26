/***********************************Copy left*****************************************
�ļ�����    hal_face.h
�ļ�������  ����ʶ��ģ�鴮��(USART1)Ӳ���� PA9-TX PA10-RX
����ʱ�䣺  2026/10/21
�޸�ʱ�䣺
�޸����ݣ�

***************************************************************************************/
#ifndef _HAL_FACE_H_
#define _HAL_FACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void HAL_FaceUsart1Init(unsigned int baud);                     /* ����1��ʼ�� */
void HAL_FaceSendData(unsigned char *data, unsigned int len);   /* �������� */

#ifdef __cplusplus
}
#endif

#endif
