/***********************************Copy Left*****************************************
�ļ�����    face_app.c
�ļ�������  ����ʶ��Ӧ�ò�(����״̬��/OLED������ʾ)
����ʱ�䣺  2026/10/21
�޸�ʱ�䣺
�޸����ݣ�

***************************************************************************************/
#include "face_app.h"
#include "dal_face.h"
#include "oled.h"
#include "SysDoorStatus.h"
#include "led_blink.h"
#include "dal_usart2.h"
#include "onenet_app.h"

/*---------------- �����ֿ�����(oledfont.h Hzk��) ----------------*/
#define HZ_REN     15     /* �� */
#define HZ_LIAN    16     /* �� */
#define HZ_SHI     17     /* ʶ */
#define HZ_BIE     18     /* �� */
#define HZ_ZHONG   19     /* �� */
#define HZ_CHENG   20     /* �� */
#define HZ_GONG    21     /* �� */
#define HZ_SHI2    22     /* ʧ */
#define HZ_BAI     23     /* �� */
#define HZ_LU      24     /* ¼ */
#define HZ_RU      25     /* �� */
#define HZ_WEI     26     /* δ */
#define HZ_ZHU     27     /* ע */
#define HZ_CE      28     /* �� */
#define HZ_QING    29     /* �� */
#define HZ_CHU     30     /* �� */
#define HZ_YI      31     /* �� */
#define HZ_CUN     32     /* �� */
#define HZ_ZAI     33     /* �� */
#define HZ_CHAO    39     /* �� */
#define HZ_SHI3    40     /* ʱ */

/*---------------- ����״̬ ----------------*/
typedef enum
{
    FACE_ST_IDLE = 0,       /* ���� */
    FACE_ST_VERIFY,         /* ʶ����(�ȴ�Ӧ��) */
    FACE_ST_ENROLL,         /* ¼����(�ȴ�Ӧ��) */
    FACE_ST_CLEAR,          /* ɾ����(�ȴ�Ӧ��) */
    FACE_ST_SHOW,           /* �����ʾ���� */
} FACE_STATE_E;

#define FACE_VERIFY_TIMEOUT   120     /* ʶ��Ӧ��ʱ 120*100ms=12s */
#define FACE_ENROLL_TIMEOUT   150     /* ¼��Ӧ��ʱ 150*100ms=15s */
#define FACE_CLEAR_TIMEOUT    50      /* ɾ��Ӧ��ʱ 50*100ms=5s  */
#define FACE_SHOW_TIME        30      /* �����ʾ���� 30*100ms=3s  */

static FACE_STATE_E sFaceState = FACE_ST_IDLE;
static unsigned char sFaceTimeOutCnt = 0;
static unsigned char sFaceShowTimeCnt = 0;

/****************************************************************
***�������ƣ�FaceShowMsg
***�������ܣ�OLED��6/7����ʾ������Ϣ(x��40�� ���5��16x16����)
***��ڲ�����idx ������������  cnt ���ָ���
***����ֵ����
***��ע��
*****************************************************************/
static void FaceShowMsg(const unsigned char *idx, unsigned char cnt)
{
    unsigned char i;
    OLED_ShowString(40, 6, (uint8_t*)"          ", 16);    /* ����Ϣ�� */
    for(i = 0; i < cnt; i++)
    {
        OLED_ShowCHinese(40 + i * 16, 6, idx[i]);
    }
}

/****************************************************************
***�������ƣ�FaceShowResult
***�������ܣ���ʾ���������SHOW����״̬
***��ڲ�����idx ������������  cnt ���ָ���
***����ֵ����
***��ע��
*****************************************************************/
static void FaceShowResult(const unsigned char *idx, unsigned char cnt)
{
    FaceShowMsg(idx, cnt);
    sFaceShowTimeCnt = 0;
    sFaceState = FACE_ST_SHOW;
}

/****************************************************************
***�������ƣ�FaceFrameProcess
***�������ܣ���������ģ��Ӧ��֡
***��ڲ�����buff Ӧ��֡  len ֡����
***����ֵ����
***��ע��֡��ʽ EF AA 00 00 xx [5]=MsgID [6]=״̬ [7..8]=ID
*****************************************************************/
static void FaceFrameProcess(unsigned char *buff, unsigned char len)
{
    static const unsigned char msgOk[]      = {HZ_SHI, HZ_BIE, HZ_CHENG, HZ_GONG};                /* ʶ��ɹ� */
    static const unsigned char msgFail[]    = {HZ_SHI, HZ_BIE, HZ_SHI2, HZ_BAI};                 /* ʶ��ʧ�� */
    static const unsigned char msgNoReg[]   = {HZ_REN, HZ_LIAN, HZ_WEI, HZ_ZHU, HZ_CE};           /* ����δע�� */
    static const unsigned char msgEnOk[]    = {HZ_LU, HZ_RU, HZ_CHENG, HZ_GONG};                  /* ¼��ɹ� */
    static const unsigned char msgEnTo[]       = {HZ_LU, HZ_RU, HZ_CHAO, HZ_SHI3};          /* ¼�볬ʱ */
static const unsigned char msgEnFail[] = {HZ_LU, HZ_RU, HZ_SHI2, HZ_BAI};                    /* ¼��ʧ�� */
    static const unsigned char msgExist[]  = {HZ_YI, HZ_CUN, HZ_ZAI};                             /* �Ѵ��� */
    static const unsigned char msgClOk[]    = {HZ_QING, HZ_CHU, HZ_CHENG, HZ_GONG};               /* ����ɹ� */
    unsigned int faceId = 0;

    if(len < 9)                          /* Ӧ��֡����9�ֽ� */
    {
        return;
    }

    switch(buff[5])
    {
        case 0x12:                       /* ʶ��Ӧ�� */
            if((buff[6] == 0x00) && ((((unsigned int)buff[7] << 8) | buff[8]) < 101))
            {
                faceId = ((unsigned int)buff[7] << 8) | buff[8];
                sSetDoorStatus(1);       /* ���� */
                sSetBuzzerStatus(1);     /* ����һ�� */
                DEBUG_uart2("����ʶ��ɹ� ID=%d\r\n", faceId);
                OneNet_ReportAuth(1, 1); FaceShowResult(msgOk, 4);
            }
            else if((buff[6] == 0x08) && (buff[7] == 0x18))
            {
                sSetBuzzerStatus(2);    /* ������ */
                DEBUG_uart2("����δע��\r\n");
                OneNet_ReportAuth(1, 0); FaceShowResult(msgNoReg, 5);
            }
            else
            {
                sSetBuzzerStatus(2);
                DEBUG_uart2("����ʶ��ʧ��\r\n");
                OneNet_ReportAuth(1, 0); FaceShowResult(msgFail, 4);
            }
            break;

        case 0x1D:                       /* ¼��Ӧ�� */
            if(buff[6] == 0x00)
            {
                faceId = ((unsigned int)buff[7] << 8) | buff[8];
                DEBUG_uart2("����¼��ɹ� ID=%d\r\n", faceId);
                OneNet_FaceCountAdd(); FaceShowResult(msgEnOk, 4);
            }
            else if(buff[6] == 0x0A)     /* �������������Ѵ��� */
            {
                DEBUG_uart2("�������Ѵ���\r\n");
                FaceShowResult(msgExist, 3);
            }
            else                         /* 0x0D¼�볬ʱ(δ��⵽����)�� */
            {
                if(buff[6] == 0x0D)     /* ��ʱ:�Ѳɼ���δ��⵽���� */
                {
                    DEBUG_uart2("����¼�볬ʱ\r\n");
                    FaceShowResult(msgEnTo, 4);
                }
                else                    /* ���������� */
                {
                    DEBUG_uart2("����¼��ʧ��\r\n");
                    FaceShowResult(msgEnFail, 4);
                }
            }
            break;

        case 0x21:                       /* ɾ��ȫ��Ӧ�� */
            DEBUG_uart2("����������ɹ�\r\n");
            OneNet_FaceCountSet(0); FaceShowResult(msgClOk, 4);
            break;

        default:
            break;
    }
}

/****************************************************************
***�������ƣ�FaceWaitTimeout
***�������ܣ���Ӧ��ʱ����
***��ڲ�����idx ������������  cnt ���ָ���
***����ֵ����
***��ע��
*****************************************************************/
static void FaceWaitTimeout(const unsigned char *idx, unsigned char cnt)
{
    sSetBuzzerStatus(2);                 /* ������ */
    FaceShowResult(idx, cnt);
}

/****************************************************************
***�������ƣ�FaceTask
***�������ܣ���������(100ms����) ״̬������
***��ڲ�������
***����ֵ����
***��ע��
*****************************************************************/
void FaceTask(void)
{
    unsigned char frame[FACE_RX_BUFF_MAX];
    unsigned char len = 0;

    switch(sFaceState)
    {
        case FACE_ST_IDLE:
            break;

        case FACE_ST_VERIFY:
            if(Dal_FaceGetFrame(frame, &len))
            {
                FaceFrameProcess(frame, len);
            }
            else if(++sFaceTimeOutCnt >= FACE_VERIFY_TIMEOUT)
            {
                static const unsigned char msgFail[] = {HZ_SHI, HZ_BIE, HZ_SHI2, HZ_BAI};
                DEBUG_uart2("����ʶ��ʱ\r\n");
                OneNet_ReportAuth(1, 0); FaceWaitTimeout(msgFail, 4);
            }
            break;

        case FACE_ST_ENROLL:
            if(Dal_FaceGetFrame(frame, &len))
            {
                FaceFrameProcess(frame, len);
            }
            else if(++sFaceTimeOutCnt >= FACE_ENROLL_TIMEOUT)
            {
                static const unsigned char msgEnFail[] = {HZ_LU, HZ_RU, HZ_SHI2, HZ_BAI};
                DEBUG_uart2("����ģ����Ӧ��\r\n");
                FaceWaitTimeout(msgEnFail, 4);
            }
            break;

        case FACE_ST_CLEAR:
            if(Dal_FaceGetFrame(frame, &len))
            {
                FaceFrameProcess(frame, len);
            }
            else if(++sFaceTimeOutCnt >= FACE_CLEAR_TIMEOUT)
            {
                static const unsigned char msgClFail[] = {HZ_QING, HZ_CHU, HZ_SHI2, HZ_BAI};
                DEBUG_uart2("���������ʱ\r\n");
                FaceWaitTimeout(msgClFail, 4);
            }
            break;

        case FACE_ST_SHOW:
            if(++sFaceShowTimeCnt >= FACE_SHOW_TIME)
            {
                sFaceShowTimeCnt = 0;
                OLED_ShowString(40, 6, (uint8_t*)"          ", 16);   /* ����Ϣ�� */
                sFaceState = FACE_ST_IDLE;
            }
            break;

        default:
            sFaceState = FACE_ST_IDLE;
            break;
    }
}

/****************************************************************
***�������ƣ�sFace_Init
***�������ܣ�����ģ���ʼ��
***��ڲ�������
***����ֵ����
***��ע������1 PA9/PA10 115200
*****************************************************************/
void sFace_Init(void)
{
    Dal_FaceInit(115200);
}

/****************************************************************
***�������ƣ�FaceStartVerify
***�������ܣ���������ʶ��
***��ڲ�������
***����ֵ����
***��ע��
*****************************************************************/
void FaceStartVerify(void)
{
    static const unsigned char msg[] = {HZ_REN, HZ_LIAN, HZ_SHI, HZ_BIE, HZ_ZHONG};   /* ����ʶ���� */
    if(sFaceState != FACE_ST_IDLE)
    {
        return;
    }
    Dal_FaceSendVerify();
    FaceShowMsg(msg, 5);
    sFaceTimeOutCnt = 0;
    sFaceState = FACE_ST_VERIFY;
}

/****************************************************************
***�������ƣ�FaceStartEnroll
***�������ܣ���������¼��
***��ڲ�������
***����ֵ����
***��ע��
*****************************************************************/
void FaceStartEnroll(void)
{
    static const unsigned char msg[] = {HZ_REN, HZ_LIAN, HZ_LU, HZ_RU, HZ_ZHONG};    /* ����¼���� */
    if(sFaceState != FACE_ST_IDLE)
    {
        return;
    }
    Dal_FaceSendEnroll();
    FaceShowMsg(msg, 5);
    sFaceTimeOutCnt = 0;
    sFaceState = FACE_ST_ENROLL;
}

/****************************************************************
***�������ƣ�FaceDeleteAll
***�������ܣ�ɾ��ģ������������
***��ڲ�������
***����ֵ����
***��ע��
*****************************************************************/
void FaceDeleteAll(void)
{
    static const unsigned char msg[] = {HZ_REN, HZ_LIAN, HZ_QING, HZ_CHU, HZ_ZHONG}; /* ��������� */
    if(sFaceState != FACE_ST_IDLE)
    {
        return;
    }
    Dal_FaceSendDeleteAll();
    FaceShowMsg(msg, 5);
    sFaceTimeOutCnt = 0;
    sFaceState = FACE_ST_CLEAR;
}
