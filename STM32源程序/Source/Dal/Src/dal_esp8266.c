/***********************************Copy Left*****************************************
�ļ����ƣ�  dal_esp8266.c
�ļ�������  ESP8266 WiFiģ������(USART2 PA2/PA3)
            AT������״̬��: ��WiFi -> ��OneNET MQTT -> ���� -> ����/����
����ʱ�䣺  2026/10/22

***************************************************************************************/
#include "dal_esp8266.h"
#include "hal_usart2.h"
#include <string.h>
#include <stdio.h>

/*---------------- �û������� ----------------*/
#define WIFI_SSID            "12345678"      /* WiFi���� */
#define WIFI_PASSWORD        "12345678"      /* WiFi���� */
#define ONENET_PID           "vleFpcs6C0"    /* OneNET��ƷID */
#define ONENET_DEV           "face"          /* OneNET�豸���� */
#define ONENET_SERVER        "mqtts.heclouds.com"
#define ONENET_PORT          1883
#define ONENET_TOKEN         "version=2018-10-31&res=products%2FvleFpcs6C0%2Fdevices%2Fface&et=2108271184&method=md5&sign=f54aIEYO9xxrk5ll54J8JA%3D%3D"

/*---------------- ���������� ----------------*/
#define ESP_RX_BUFF_MAX       256             /* ����֡���� */
#define ESP_AT_CMD_MAX        256             /* ATָ��� */
#define ESP_PUB_BUFF_MAX      160             /* ����JSON���� */
#define ESP_SET_BUFF_MAX      160             /* �·�JSON���� */

/*---------------- ģ��״̬ ----------------*/
typedef enum
{
    ESP_ST_AT = 0,       /* AT���� */
    ESP_ST_ATE0,         /* �رջ��� */
    ESP_ST_CWMODE,       /* STAģʽ */
    ESP_ST_CWDHCP,       /* ����DHCP */
    ESP_ST_CWJAP,
    ESP_ST_MQTTCLEAN,    /* ������MQTT����(��λ��ģ��δ�ϵ�ʱ����) */        /* ����WiFi */
    ESP_ST_USERCFG,      /* MQTT�û����� */
    ESP_ST_MQTTCONN,     /* ����MQTT������ */
    ESP_ST_SUB_REPLY,    /* �����ϱ�Ӧ������ */
    ESP_ST_SUB_SET,      /* ���������·����� */
    ESP_ST_ONLINE,       /* ���� */
    ESP_ST_PUB_CMD,      /* ����ָ��׶� */
    ESP_ST_PUB_DATA,     /* �������ݽ׶� */
} ESP_STATE_E;

static ESP_STATE_E sEspState = ESP_ST_AT;
static unsigned char sEspTimeout = 0;         /* ��ʱ���� 100ms��λ */
static unsigned char sEspRetry = 0;           /* ���Լ��� */
static volatile unsigned char sEspOkFlag = 0;      /* �յ�����Ӧ�� */
static volatile unsigned char sEspDownFlag = 0;     /* ���߱�־ 1WiFi�� 2MQTT�� */
static const char *sEspExpect = NULL;        /* ����Ӧ�� */
static const char *sEspExpectAlt = NULL;     /* ��������Ӧ�� */

static char EspAtCmd[ESP_AT_CMD_MAX];
static char EspPubBuff[ESP_PUB_BUFF_MAX];
static volatile unsigned char EspSetFlag = 0;      /* �յ��·����� */
static char EspSetBuff[ESP_SET_BUFF_MAX];

/*---------------- ���ջ���(�ж�д ��ѭ����) ----------------*/
static volatile unsigned char EspRxBuff[ESP_RX_BUFF_MAX];
static volatile unsigned char EspRxLen = 0;
static volatile unsigned char EspFrameFlag = 0;    /* ����֡��� */
static unsigned char EspFrameBuff[ESP_RX_BUFF_MAX];
static volatile unsigned char EspFrameLen = 0;

/****************************************************************
***�������ƣ�  EspSendString
***�������ܣ�  �����ַ���(��ѯTXE)
***��ڲ�����  str �ַ���
***����ֵ��  ��
***��ע��
*****************************************************************/
static void EspSendString(const char *str)
{
    while (*str != '\0')
    {
        while (!LL_USART_IsActiveFlag_TXE(USART2));
        USART2->DR = (unsigned char)(*str++);
    }
}

/****************************************************************
***�������ƣ�  EspGoto
***�������ܣ�  �л�״̬�����Ͷ�ӦATָ��
***��ڲ�����  st Ŀ��״̬
***����ֵ��  ��
***��ע��
*****************************************************************/
static void EspGoto(ESP_STATE_E st)
{
    sEspState = st;
    sEspTimeout = 0;
    sEspRetry = 0;
    sEspOkFlag = 0;
    sEspDownFlag = 0;
    sEspExpect = NULL;
    sEspExpectAlt = NULL;
    switch (st)
    {
        case ESP_ST_AT:
            strcpy(EspAtCmd, "AT\r\n");
            sEspExpect = "OK";
            break;
        case ESP_ST_ATE0:
            strcpy(EspAtCmd, "ATE0\r\n");
            sEspExpect = "OK";
            break;
        case ESP_ST_CWMODE:
            strcpy(EspAtCmd, "AT+CWMODE=1\r\n");
            sEspExpect = "OK";
            break;
        case ESP_ST_CWDHCP:
            strcpy(EspAtCmd, "AT+CWDHCP=1,1\r\n");
            sEspExpect = "OK";
            break;
        case ESP_ST_CWJAP:
            sprintf(EspAtCmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", WIFI_SSID, WIFI_PASSWORD);
            sEspExpect = "WIFI GOT IP";
            sEspExpectAlt = "ERROR";      /* ģ�����������WiFi�᷵��ERROR ֱ�Ӽ��� */
            break;
        case ESP_ST_MQTTCLEAN:
            strcpy(EspAtCmd, "AT+MQTTCLEAN=0\r\n");
            sEspExpect = "OK";
            sEspExpectAlt = "ERROR";      /* δ����ʱ����ERROR ֱ������ */
            break;
        case ESP_ST_USERCFG:
            sprintf(EspAtCmd, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",
                    ONENET_DEV, ONENET_PID, ONENET_TOKEN);
            sEspExpect = "OK";
            break;
        case ESP_ST_MQTTCONN:
            sprintf(EspAtCmd, "AT+MQTTCONN=0,\"%s\",%d,1\r\n", ONENET_SERVER, ONENET_PORT);
            sEspExpect = "+MQTTCONNECTED:0,1";
            break;
        case ESP_ST_SUB_REPLY:
            strcpy(EspAtCmd, "AT+MQTTSUB=0,\"$sys/" ONENET_PID "/" ONENET_DEV "/thing/property/post/reply\",0\r\n");
            sEspExpect = "OK";
            break;
        case ESP_ST_SUB_SET:
            strcpy(EspAtCmd, "AT+MQTTSUB=0,\"$sys/" ONENET_PID "/" ONENET_DEV "/thing/property/set\",0\r\n");
            sEspExpect = "OK";
            break;
        default:                          /* ONLINE��ָ�� */
            return;
    }
    EspSendString(EspAtCmd);
}

/****************************************************************
***�������ƣ�  EspProcessFrame
***�������ܣ�  ����һ֡ATӦ��/URC
***��ڲ�����  frame ֡����(�Ѽӽ�����)
***����ֵ��  ��
***��ע��    ƥ������Ӧ�� �������URC�������·�
*****************************************************************/
static void EspProcessFrame(char *frame)
{
    char *p;

    if (strstr(frame, "WIFI DISCONNECT") != NULL)
    {
        sEspDownFlag = 1;
    }
    if (strstr(frame, "+MQTTDISCONNECTED") != NULL)
    {
        sEspDownFlag = 2;
    }

    /* �����·�: +MQTTSUBRECV:0,"$sys/.../thing/property/set",99,{json} */
    if (strstr(frame, "+MQTTSUBRECV") != NULL)
    {
        p = strstr(frame, "thing/property/set\"");
        if (p != NULL)
        {
            p = strchr(p, '{');
            if (p != NULL)
            {
                strncpy(EspSetBuff, p, ESP_SET_BUFF_MAX - 1);
                EspSetBuff[ESP_SET_BUFF_MAX - 1] = '\0';
                EspSetFlag = 1;
            }
        }
    }

    /* ����Ӧ��ƥ�� */
    if (sEspExpect != NULL && strstr(frame, sEspExpect) != NULL)
    {
        sEspOkFlag = 1;
    }
    else if (sEspExpectAlt != NULL && strstr(frame, sEspExpectAlt) != NULL)
    {
        sEspOkFlag = 1;
    }
}

/****************************************************************
***�������ƣ�  Esp8266Machine
***�������ܣ�  AT״̬��(100ms����)
***��ڲ�����  ��
***����ֵ��  ��
***��ע��
*****************************************************************/
void Esp8266Machine(void)
{
    /* 1.��������֡ */
    if (EspFrameFlag)
    {
        EspFrameFlag = 0;
        EspFrameBuff[EspFrameLen] = '\0';
        EspProcessFrame((char *)EspFrameBuff);
    }

    /* 2.״̬��ʱ���ƽ� */
    switch (sEspState)
    {
        case ESP_ST_AT:
        case ESP_ST_ATE0:
        case ESP_ST_CWMODE:
        case ESP_ST_CWDHCP:
            if (sEspOkFlag)
            {
                EspGoto((ESP_STATE_E)(sEspState + 1));
            }
            else if (++sEspTimeout >= 20)          /* 2s��ʱ */
            {
                sEspTimeout = 0;
                if (++sEspRetry >= 5)
                {
                    sEspRetry = 0;                 /* ģ�������� �������� */
                }
                EspSendString(EspAtCmd);
            }
            break;

        case ESP_ST_CWJAP:
            if (sEspOkFlag)
            {
                EspGoto(ESP_ST_MQTTCLEAN);
            }
            else if (++sEspTimeout >= 150)         /* 15s */
            {
                sEspTimeout = 0;
                if (++sEspRetry >= 3)
                {
                    EspGoto(ESP_ST_AT);           /* �������� */
                }
                else
                {
                    EspSendString(EspAtCmd);
                }
            }
            break;

        case ESP_ST_MQTTCLEAN:
            if (sEspOkFlag)
            {
                EspGoto(ESP_ST_USERCFG);
            }
            else if (++sEspTimeout >= 20)          /* 2s */
            {
                sEspTimeout = 0;
                if (++sEspRetry >= 3)
                {
                    EspGoto(ESP_ST_CWJAP);          /* �������� */
                }
                else
                {
                    EspSendString(EspAtCmd);
                }
            }
            break;

        case ESP_ST_USERCFG:
            if (sEspOkFlag)
            {
                EspGoto(ESP_ST_MQTTCONN);
            }
            else if (++sEspTimeout >= 20)
            {
                sEspTimeout = 0;
                if (++sEspRetry >= 3)
                {
                    EspGoto(ESP_ST_CWJAP);
                }
                else
                {
                    EspSendString(EspAtCmd);
                }
            }
            break;

        case ESP_ST_MQTTCONN:
            if (sEspOkFlag)
            {
                EspGoto(ESP_ST_SUB_REPLY);
            }
            else if (++sEspTimeout >= 100)         /* 10s */
            {
                sEspTimeout = 0;
                if (++sEspRetry >= 3)
                {
                    EspGoto(ESP_ST_MQTTCLEAN);
                }
                else
                {
                    EspSendString(EspAtCmd);
                }
            }
            break;

        case ESP_ST_SUB_REPLY:
        case ESP_ST_SUB_SET:
            if (sEspOkFlag)
            {
                EspGoto((ESP_STATE_E)(sEspState + 1));   /* SUB_SET��ONLINE */
            }
            else if (++sEspTimeout >= 20)
            {
                sEspTimeout = 0;
                if (++sEspRetry >= 3)
                {
                    EspGoto(ESP_ST_MQTTCONN);
                }
                else
                {
                    EspSendString(EspAtCmd);
                }
            }
            break;

        case ESP_ST_ONLINE:
            if (sEspDownFlag == 1)
            {
                EspGoto(ESP_ST_CWJAP);             /* WiFi���� */
            }
            else if (sEspDownFlag == 2)
            {
                EspGoto(ESP_ST_MQTTCLEAN);           /* MQTT���� */
            }
            break;

        case ESP_ST_PUB_CMD:
            if (sEspOkFlag)                        /* �յ�">"��ʾ�� */
            {
                sEspState = ESP_ST_PUB_DATA;
                sEspTimeout = 0;
                sEspOkFlag = 0;
                sEspExpect = "+MQTTPUB:";
                sEspExpectAlt = NULL;
                EspSendString(EspPubBuff);         /* ����JSON���� */
            }
            else if (++sEspTimeout >= 10)          /* 1sδ�յ�">" ���� */
            {
                EspGoto(ESP_ST_ONLINE);
            }
            break;

        case ESP_ST_PUB_DATA:
            if (sEspOkFlag)                        /* �յ��������URC */
            {
                EspGoto(ESP_ST_ONLINE);
            }
            else if (++sEspTimeout >= 30)          /* 3s���� */
            {
                EspGoto(ESP_ST_ONLINE);
            }
            break;

        default:
            EspGoto(ESP_ST_AT);
            break;
    }
}

/****************************************************************
***�������ƣ�  Dal_Esp8266Init
***�������ܣ�  ģ���ʼ��
***��ڲ�����  ��
***����ֵ��  ��
***��ע��    ģ��δ�ϵ����ʱ��״̬���Զ�����
*****************************************************************/
void Dal_Esp8266Init(void)
{
    HAL_Usart2Init(115200);
    EspGoto(ESP_ST_AT);
}

/****************************************************************
***�������ƣ�  Esp8266IsOnline
***�������ܣ�  ��ѯ�ƶ�����״̬
***��ڲ�����  ��
***����ֵ��  1���� 0����
***��ע��
*****************************************************************/
/****************************************************************
***�������ƣ�  Esp8266GetStage
***�������ܣ�  ��ȡ���ӽ׶�
***��ڲ�����  ��
***����ֵ��  0δ��WiFi 1WiFi���� 2�ƶ�����
***��ע��
*****************************************************************/
uint8_t Esp8266GetStage(void)
{
    if (sEspState >= ESP_ST_ONLINE)
    {
        return 2;                          /* ����(�������׶�) */
    }
    if (sEspState >= ESP_ST_MQTTCLEAN)
    {
        return 1;                          /* WiFi���� */
    }
    return 0;                              /* δ��WiFi */
}

uint8_t Esp8266IsOnline(void)
{
    return (sEspState == ESP_ST_ONLINE) ? 1 : 0;
}

/****************************************************************
***�������ƣ�  EspPubStart
***�������ܣ�  ����һ�η���(post��set_reply����)
***��ڲ�����  json �غ� topicSetReply 1Ϊset_reply����
***����ֵ��  1���� 0æ
***��ע��
*****************************************************************/
static uint8_t EspPubStart(const char *json, uint8_t topicSetReply)
{
    if (sEspState != ESP_ST_ONLINE)
    {
        return 0;
    }
    strncpy(EspPubBuff, json, ESP_PUB_BUFF_MAX - 1);
    EspPubBuff[ESP_PUB_BUFF_MAX - 1] = '\0';
    sprintf(EspAtCmd, "AT+MQTTPUBRAW=0,\"$sys/" ONENET_PID "/" ONENET_DEV "/thing/property/%s\",%d,0,0\r\n",
            topicSetReply ? "set_reply" : "post", (int)strlen(EspPubBuff));
    sEspState = ESP_ST_PUB_CMD;
    sEspTimeout = 0;
    sEspRetry = 0;
    sEspOkFlag = 0;
    sEspDownFlag = 0;
    sEspExpect = ">";
    sEspExpectAlt = NULL;
    EspSendString(EspAtCmd);
    return 1;
}

/****************************************************************
***�������ƣ�  Esp8266PublishReq
***�������ܣ�  �����ϱ�����
***��ڲ�����  json �ϱ�JSON
***����ֵ��  1���� 0æ
***��ע��
*****************************************************************/
uint8_t Esp8266PublishReq(const char *json)
{
    return EspPubStart(json, 0);
}

/****************************************************************
***�������ƣ�  Esp8266ReplyReq
***�������ܣ�  ��������������Ӧ��
***��ڲ�����  json Ӧ��JSON
***����ֵ��  1���� 0æ
***��ע��
*****************************************************************/
uint8_t Esp8266ReplyReq(const char *json)
{
    return EspPubStart(json, 1);
}

/****************************************************************
***�������ƣ�  Esp8266FetchSetCmd
***�������ܣ�  ȡ��ƽ̨�·�����������JSON
***��ڲ�����  json �������
***����ֵ��  1���·� 0��
***��ע��
*****************************************************************/
uint8_t Esp8266FetchSetCmd(char *json)
{
    if (EspSetFlag == 0)
    {
        return 0;
    }
    EspSetFlag = 0;
    strcpy(json, EspSetBuff);
    return 1;
}

/****************************************************************
***�������ƣ�  USART2_IRQHandler
***�������ܣ�  ����2�жϷ�����(ESP8266Ӧ���ռ�)
***��ڲ�����  ��
***����ֵ��  ��
***��ע��    RXNE���ֽ� IDLE��֡
*****************************************************************/
void USART2_IRQHandler(void)
{
    unsigned char data = 0;

    if (LL_USART_IsActiveFlag_RXNE(USART2))
    {
        data = LL_USART_ReceiveData8(USART2);
        if (EspRxLen < (ESP_RX_BUFF_MAX - 1))
        {
            EspRxBuff[EspRxLen++] = data;
        }
    }

    if (LL_USART_IsActiveFlag_IDLE(USART2))
    {
        LL_USART_ClearFlag_IDLE(USART2);
        if (EspRxLen > 0)
        {
            memcpy((void *)EspFrameBuff, (const void *)EspRxBuff, EspRxLen);
            EspFrameLen = EspRxLen;
            EspFrameFlag = 1;
            EspRxLen = 0;
        }
    }
}
