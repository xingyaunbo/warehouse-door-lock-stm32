/***********************************Copy left*****************************************
�ļ�����    dal_face.c
�ļ�������  ����ʶ��ģ��Э���
            ֡��ʽ: 0xEF 0xAA + MsgID + LenH + LenL + Data + У��(��MsgID�����)
            Ӧ��֡: 0xEF 0xAA ... [5]=MsgID [6]=״̬ [7..8]=ID
����ʱ�䣺  2026/10/21
�޸�ʱ�䣺
�޸����ݣ�

***************************************************************************************/
#include "dal_face.h"
#include "hal_face.h"

static unsigned char FaceTxBuff[48];                       /* ���ͻ��� */
static volatile unsigned char FaceRxBuff[FACE_RX_BUFF_MAX]; /* ���ջ��� */
static volatile unsigned char FaceRxLen = 0;                /* ���ռ��� */
static volatile unsigned char FaceFrameStart = 0;          /* ֡���ձ�־ */
static volatile unsigned char FaceFrameFlag = 0;            /* ����֡��־ */
static unsigned char FaceFrameBuff[FACE_RX_BUFF_MAX];      /* ֡���� */
static unsigned char FaceFrameLen = 0;                      /* ֡���� */

/****************************************************************
***�������ƣ�FaceCheckXor
***�������ܣ����У��
***��ڲ�����buff-���� len-����
***����ֵ�����ֵ
*****************************************************************/
static unsigned char FaceCheckXor(unsigned char *buff, unsigned int len)
{
    unsigned char x = 0;
    unsigned int i;
    for (i = 0; i < len; i++)
    {
        x ^= buff[i];
    }
    return x;
}

/****************************************************************
***�������ƣ�USART1_IRQHandler
***�������ܣ�USART1�жϷ�����(��������ģ��Ӧ��֡)
***��ڲ�������
***����ֵ����
***��ע��֡ͷ0xEF��ʼ����,IDLE�����ж��ж�֡����
*****************************************************************/
void USART1_IRQHandler(void)
{
    unsigned char data = 0;
    unsigned char i = 0;

    if (LL_USART_IsActiveFlag_RXNE(USART1))
    {
        data = LL_USART_ReceiveData8(USART1);
        if (FaceFrameStart)
        {
            if (FaceRxLen < FACE_RX_BUFF_MAX)
            {
                FaceRxBuff[FaceRxLen++] = data;
            }
        }
        else if (data == 0xEF)                /* ֡ͷ */
        {
            FaceFrameStart = 1;
            FaceRxLen = 0;
            FaceRxBuff[FaceRxLen++] = data;
        }
    }

    if (LL_USART_IsActiveFlag_IDLE(USART1))
    {
        LL_USART_ClearFlag_IDLE(USART1);
        if (FaceFrameStart)
        {
            FaceFrameStart = 0;
            if (FaceRxLen >= 6 && FaceRxLen <= FACE_RX_BUFF_MAX)
            {
                for (i = 0; i < FaceRxLen; i++)
                {
                    FaceFrameBuff[i] = FaceRxBuff[i];
                }
                FaceFrameLen = FaceRxLen;
                FaceFrameFlag = 1;             /* ֪ͨӦ�ò�ȡ֡ */
            }
            FaceRxLen = 0;
        }
    }
}

/****************************************************************
***�������ƣ�Dal_FaceInit
***�������ܣ�����ģ���ʼ��
***��ڲ�����baud-������
***����ֵ����
*****************************************************************/
void Dal_FaceInit(unsigned int baud)
{
    HAL_FaceUsart1Init(baud);
}

/****************************************************************
***�������ƣ�Dal_FaceSendReset
***�������ܣ�ģ�鸴λ
***��ڲ�������
***����ֵ����
*****************************************************************/
void Dal_FaceSendReset(void)
{
    unsigned char i = 0;
    FaceTxBuff[i++] = 0xEF;
    FaceTxBuff[i++] = 0xAA;
    FaceTxBuff[i++] = 0x10;                  /* ��λ */
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = FaceCheckXor(&FaceTxBuff[2], 3);
    HAL_FaceSendData(FaceTxBuff, 6);
}

/****************************************************************
***�������ƣ�Dal_FaceSendVerify
***�������ܣ���������ʶ������(10s��ʱ)
***��ڲ�������
***����ֵ����
*****************************************************************/
void Dal_FaceSendVerify(void)
{
    unsigned char i = 0;
    FaceTxBuff[i++] = 0xEF;
    FaceTxBuff[i++] = 0xAA;
    FaceTxBuff[i++] = 0x12;                  /* ʶ�� */
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = 0x02;
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = 0x0A;                  /* ��ʱ10s */
    FaceTxBuff[i++] = FaceCheckXor(&FaceTxBuff[2], 5);
    HAL_FaceSendData(FaceTxBuff, 8);
}

/****************************************************************
***�������ƣ�Dal_FaceSendEnroll
***�������ܣ���������¼������(10s��ʱ)
***��ڲ�������
***����ֵ����
*****************************************************************/
void Dal_FaceSendEnroll(void)
{
    unsigned char i = 0;
    unsigned char j = 0;
    FaceTxBuff[i++] = 0xEF;
    FaceTxBuff[i++] = 0xAA;
    FaceTxBuff[i++] = 0x1D;                  /* ¼�� */
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = 0x23;                  /* ���ݳ��� */
    for (j = 0; j < 34; j++)                 /* �û���Ϣ(Ĭ��) */
    {
        FaceTxBuff[i++] = 0x00;
    }
    FaceTxBuff[i++] = 0x0A;                  /* ��ʱ10s */
    FaceTxBuff[i++] = FaceCheckXor(&FaceTxBuff[2], i - 2);
    HAL_FaceSendData(FaceTxBuff, 41);
}

/****************************************************************
***�������ƣ�Dal_FaceSendDeleteAll
***�������ܣ�ɾ����������
***��ڲ�������
***����ֵ����
*****************************************************************/
void Dal_FaceSendDeleteAll(void)
{
    unsigned char i = 0;
    FaceTxBuff[i++] = 0xEF;
    FaceTxBuff[i++] = 0xAA;
    FaceTxBuff[i++] = 0x21;                  /* ɾ��ȫ�� */
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = 0x00;
    FaceTxBuff[i++] = FaceCheckXor(&FaceTxBuff[2], 3);
    HAL_FaceSendData(FaceTxBuff, 6);
}

/****************************************************************
***�������ƣ�Dal_FaceGetFrame
***�������ܣ���ȡ����Ӧ��֡
***��ڲ�����buff-֡������� len-֡�����
***����ֵ��1-����֡ 0-��
*****************************************************************/
unsigned char Dal_FaceGetFrame(unsigned char *buff, unsigned char *len)
{
    unsigned char i = 0;
    if (FaceFrameFlag)
    {
        for (i = 0; i < FaceFrameLen; i++)
        {
            buff[i] = FaceFrameBuff[i];
        }
        *len = FaceFrameLen;
        FaceFrameFlag = 0;
        return 1;
    }
    return 0;
}
