/***********************************Copy Left*****************************************
�ļ����ƣ�  onenet_app.c
�ļ�������  OneNET��ƽ̨Ӧ�ò�
            �ϱ�: ��֤���(AuthType/AuthResult/FaceCount) ��״̬�仯(LockState)
            �·�: Face_Clear(������) Password_Set(������) RemoteUnlock(Զ�̿���)+set_replyӦ��
����ʱ�䣺  2026/10/22

***************************************************************************************/
#include "onenet_app.h"
#include "dal_esp8266.h"
#include "face_app.h"
#include "key_control.h"
#include "SysDoorStatus.h"
#include "led_blink.h"
#include "eeprom.h"
#include <string.h>
#include <stdio.h>
#include "oled.h"
#include "stm32f1xx_ll_utils.h"

/*---------------- ���ϱ���־ ----------------*/
static uint8_t sPendAuth = 0;        /* ��֤�¼����ϱ� */
static uint8_t sPendAuthType = 0;    /* ��֤��ʽ */
static uint8_t sPendAuthOk = 0;      /* ��֤��� */
static uint8_t sPendLock = 0;        /* ����״̬���ϱ� */
static uint8_t sPendLockState = 1;   /* ����״̬ */
static uint8_t sPendCount = 0;       /* �����������ϱ� */
static uint8_t sFaceCount = 0;       /* �������� */
static uint32_t sReportId = 1;       /* �ϱ���Ϣid */

/*---------------- ��̬����(��������SysTick�ж� ����ռ��ջ) ----------------*/
static char sJsonBuff[160];
static char sSetBuff[160];
static char sReplyBuff[64];

/****************************************************************
***�������ƣ�  OneNet_ReportAuth
***�������ܣ�  �Ǽ�һ����֤�¼�(���ϱ�)
***��ڲ�����  type ��֤��ʽ  ok ��֤���
***����ֵ��  ��
***��ע��    ���ӿ���ʱ��OneNetTaskͳһ����
*****************************************************************/
void OneNet_ReportAuth(uint8_t type, uint8_t ok)
{
    sPendAuth = 1;
    sPendAuthType = type;
    sPendAuthOk = (ok != 0) ? 1 : 0;
}

/****************************************************************
***�������ƣ�  OneNet_ReportLock
***�������ܣ�  �Ǽ�����״̬�仯(���ϱ�)
***��ڲ�����  state ����״̬
***����ֵ��  ��
***��ע��
*****************************************************************/
void OneNet_ReportLock(uint8_t state)
{
    sPendLock = 1;
    sPendLockState = state;
}

/****************************************************************
***�������ƣ�  OneNet_FaceCountAdd
***�������ܣ�  ��������+1���Ǽ��ϱ�
***��ڲ�����  ��
***����ֵ��  ��
***��ע��
*****************************************************************/
void OneNet_FaceCountAdd(void)
{
    if (sFaceCount < 100)
    {
        sFaceCount++;
    }
    sPendCount = 1;
}

/****************************************************************
***�������ƣ�  OneNet_FaceCountSet
***�������ܣ�  ���������������Ǽ��ϱ�
***��ڲ�����  cnt ����
***����ֵ��  ��
***��ע��
*****************************************************************/
void OneNet_FaceCountSet(uint8_t cnt)
{
    sFaceCount = cnt;
    sPendCount = 1;
}

/****************************************************************
***�������ƣ�  OnenetSetProcess
***�������ܣ�  ������ƽ̨�������ò���set_reply
***��ڲ�����  json �·�JSON
***����ֵ��  ��
***��ע��    �����ַ������� ������cJson
*****************************************************************/
static void OnenetSetProcess(const char *json)
{
    const char *p;
    char idBuff[8];
    uint8_t idLen = 0;
    uint8_t i;
    uint32_t pwd = 0;
    uint8_t pwdBuff[3];

    /* ��ȡ�·�id����Ӧ�� */
    p = strstr(json, "\"id\":\"");
    if (p != NULL)
    {
        p += 6;
        while ((p[idLen] >= '0') && (p[idLen] <= '9') && (idLen < 7))
        {
            idBuff[idLen] = p[idLen];
            idLen++;
        }
    }
    idBuff[idLen] = '\0';
    if (idLen == 0)
    {
        strcpy(idBuff, "0");
    }

    /* Face_Clear ������� */
    if (strstr(json, "\"Face_Clear\":true") != NULL)
    {
        FaceDeleteAll();                       /* ��face_app״̬����ɾ��ָ�� */
    }

    /* Password_Set ��������(6λ����) */
    p = strstr(json, "\"Password_Set\":\"");
    if (p != NULL)
    {
        p += 16;
        for (i = 0; i < 6; i++)
        {
            if ((p[i] < '0') || (p[i] > '9'))
            {
                break;
            }
        }
        if (i == 6)
        {
            for (i = 0; i < 6; i++)
            {
                pwd = pwd * 16 + (uint32_t)(p[i] - '0');
            }
            sSetPasswordValue(pwd);
            pwdBuff[0] = (uint8_t)(pwd >> 16);
            pwdBuff[1] = (uint8_t)(pwd >> 8);
            pwdBuff[2] = (uint8_t)(pwd);
            EEPROM_WriteContinuous(0x00, 3, pwdBuff);
        }
    }

    /* RemoteUnlock Զ�̿��� */
    if (strstr(json, "\"RemoteUnlock\":true") != NULL)
    {
        sSetDoorStatus(1);
        sSetBuzzerStatus(1);
        OneNet_ReportAuth(ONENET_AUTH_REMOTE, 1);   /* �ϱ�Զ�̽��� */
    }

    /* Ӧ��set_reply */
    snprintf(sReplyBuff, sizeof(sReplyBuff), "{\"id\":\"%s\",\"code\":200,\"msg\":\"success\"}", idBuff);
    Esp8266ReplyReq(sReplyBuff);
}

/****************************************************************
***�������ƣ�  OneNetTask
***�������ܣ�  ��ƽ̨����(100ms)
***��ڲ�����  ��
***����ֵ��  ��
***��ע��    ����AT״̬�� �����·�����ϱ��¼�
*****************************************************************/
/****************************************************************
***�������ƣ�  OneNet_WaitOnline
***�������ܣ�  �����ȴ�WiFi����ƽ̨����(��ʾ״̬ �ɹ������ϵͳ)
***��ڲ�����  ��
***����ֵ��  ��
***��ע��    ��ȴ�30s ��ʱ�Խ���ϵͳ(��̨��������)
*****************************************************************/
void OneNet_WaitOnline(void)
{
    uint16_t t = 0;
    uint8_t stage = 0;
    uint8_t lastStage = 0xFF;

    OLED_ShowString(0, 2, (uint8_t *)"WiFi : ", 16);
    OLED_ShowString(0, 5, (uint8_t *)"Cloud: ", 16);
    OLED_ShowString(56, 2, (uint8_t *)"Link...", 16);
    OLED_ShowString(56, 5, (uint8_t *)"Link...", 16);

    while (t < 400)                       /* 400*100ms=40s */
    {
        Esp8266Machine();
        stage = Esp8266GetStage();
        if (stage != lastStage)
        {
            if (stage == 1)
            {
                OLED_ShowString(56, 2, (uint8_t *)"OK     ", 16);
            }
            else if (stage == 2)
            {
                OLED_ShowString(56, 2, (uint8_t *)"OK     ", 16);
                OLED_ShowString(56, 5, (uint8_t *)"OK     ", 16);
            }
            lastStage = stage;
        }
        if (stage == 2)
        {
            LL_mDelay(800);               /* ͣ����ʾ���ӳɹ� */
            OLED_Clear();                 /* ���� ������ϵͳ�����ص� */
            return;
        }
        LL_mDelay(100);
        t++;
    }
    if (stage == 0)                       /* WiFiδ���� */
    {
        OLED_ShowString(56, 2, (uint8_t *)"Fail   ", 16);
        OLED_ShowString(56, 5, (uint8_t *)"       ", 16);
    }
    else                                 /* WiFi���� MQTTʧ�� */
    {
        OLED_ShowString(56, 5, (uint8_t *)"Fail   ", 16);
    }
    LL_mDelay(1000);
    OLED_Clear();                         /* ���� ������ϵͳ�����ص� */
}

void OneNetTask(void)
{
    Esp8266Machine();

    /* �����·����� */
    if (Esp8266FetchSetCmd(sSetBuff))
    {
        OnenetSetProcess(sSetBuff);
    }

    /* �����ϱ����� */
    if (Esp8266IsOnline())
    {
        if (sPendAuth)
        {
            snprintf(sJsonBuff, sizeof(sJsonBuff),
                "{\"id\":\"%lu\",\"params\":{\"AuthType\":{\"value\":%d},\"AuthResult\":{\"value\":%s},\"FaceCount\":{\"value\":%d}}}",
                (unsigned long)sReportId, sPendAuthType, sPendAuthOk ? "true" : "false", sFaceCount);
            sReportId++;
            if (Esp8266PublishReq(sJsonBuff))
            {
                sPendAuth = 0;
            }
        }
        else if (sPendLock)
        {
            snprintf(sJsonBuff, sizeof(sJsonBuff),
                "{\"id\":\"%lu\",\"params\":{\"LockState\":{\"value\":%d}}}",
                (unsigned long)sReportId, sPendLockState);
            sReportId++;
            if (Esp8266PublishReq(sJsonBuff))
            {
                sPendLock = 0;
            }
        }
        else if (sPendCount)
        {
            snprintf(sJsonBuff, sizeof(sJsonBuff),
                "{\"id\":\"%lu\",\"params\":{\"FaceCount\":{\"value\":%d}}}",
                (unsigned long)sReportId, sFaceCount);
            sReportId++;
            if (Esp8266PublishReq(sJsonBuff))
            {
                sPendCount = 0;
            }
        }
    }
}
