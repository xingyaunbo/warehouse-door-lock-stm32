/***********************************Copy Left*****************************************
�ļ�����    face_app.h
�ļ�������  ����ʶ��Ӧ�ò�(����״̬��/OLED������ʾ)
����ʱ�䣺  2026/10/21
�޸�ʱ�䣺
�޸����ݣ�

***************************************************************************************/
#ifndef _FACE_APP_H_
#define _FACE_APP_H_

#ifdef __cplusplus
extern "C" {
#endif

void sFace_Init(void);          /* ����ģ���ʼ��(����1 115200) */
void FaceTask(void);            /* ��������(100ms���ڵ���) */
void FaceStartVerify(void);     /* ��������ʶ�� */
void FaceStartEnroll(void);     /* ��������¼�� */
void FaceDeleteAll(void);       /* ɾ��ģ������������ */

#ifdef __cplusplus
}
#endif

#endif
