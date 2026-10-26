/***********************************Copy left*****************************************
�ļ�����    hal_face.c
�ļ�������  ����ʶ��ģ�鴮��(USART1)Ӳ���� PA9-TX PA10-RX 115200
����ʱ�䣺  2026/10/21
�޸�ʱ�䣺
�޸����ݣ�

***************************************************************************************/
#include "hal_face.h"

/****************************************************************
***�������ƣ�HAL_FaceUsart1Init
***�������ܣ�USART1��ʼ��(PA9-TX PA10-RX)
***��ڲ�����baud-������
***����ֵ����
***��ע������ʹ��RXNE+IDLE�ж�,�жϷ�������dal_face.c��
*****************************************************************/
void HAL_FaceUsart1Init(unsigned int baud)
{
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

    /* PA9 USART1_TX ����������� */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PA10 USART1_RX �������� */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);

    USART_InitStruct.BaudRate = baud;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    LL_USART_Init(USART1, &USART_InitStruct);

    LL_USART_EnableIT_RXNE(USART1);
    LL_USART_EnableIT_IDLE(USART1);
    LL_USART_Enable(USART1);
}

/****************************************************************
***�������ƣ�HAL_FaceSendData
***�������ܣ���������һ֡����
***��ڲ�����data-���� len-����
***����ֵ����
*****************************************************************/
void HAL_FaceSendData(unsigned char *data, unsigned int len)
{
    unsigned int i;
    for (i = 0; i < len; i++)
    {
        while (LL_USART_IsActiveFlag_TXE(USART1) == RESET);
        LL_USART_TransmitData8(USART1, data[i]);
    }
    while (LL_USART_IsActiveFlag_TC(USART1) == RESET);
}
