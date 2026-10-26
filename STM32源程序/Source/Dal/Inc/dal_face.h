/***********************************Copy left*****************************************
�ļ�����    dal_face.h
�ļ�������  ����ʶ��ģ��Э���(֡ͷ0xEF 0xAA ���У��)
����ʱ�䣺  2026/10/21
�޸�ʱ�䣺
�޸����ݣ�

***************************************************************************************/
#ifndef _DAL_FACE_H_
#define _DAL_FACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define FACE_RX_BUFF_MAX  64      /* ����֡��󳤶� */

void Dal_FaceInit(unsigned int baud);                                  /* ��ʼ��(������) */
void Dal_FaceSendReset(void);                                         /* ģ�鸴λ */
void Dal_FaceSendVerify(void);                                        /* ����ʶ��(10s��ʱ) */
void Dal_FaceSendEnroll(void);                                        /* ����¼��(10s��ʱ) */
void Dal_FaceSendDeleteAll(void);                                     /* ɾ���������� */
unsigned char Dal_FaceGetFrame(unsigned char *buff, unsigned char *len); /* ȡ����Ӧ��֡ */

#ifdef __cplusplus
}
#endif

#endif
