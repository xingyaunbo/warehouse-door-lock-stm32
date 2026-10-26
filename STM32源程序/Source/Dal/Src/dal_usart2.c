/***********************************Copy Left*****************************************
�ļ����ƣ�  dal_usart2.c
�ļ�������  ����2���ݲ�(USART2����ESP8266 WiFiģ��ռ��)
            ����ԭ�ӿڹ��ϲ����� �����ж�����dal_esp8266.c
����ʱ�䣺  2023/03/01
�޸�ʱ�䣺  2026/10/22
�޸����ݣ�  USART2��ΪESP8266ר�� �������DEBUG_uart2�ر�

***************************************************************************************/
#include "dal_usart2.h"
#include "string.h"

uint8_t UART2_RX_BUFF[50];
uint8_t UART2_RX_LEN = 0;
uint8_t RX_FLAG = 0;

uint8_t sReadUart2RxFlag(void)
{
	return RX_FLAG;
}

void sSetRX_FLAG(uint8_t flag)
{
	RX_FLAG = flag;
}

void sReadUart2RxBuff(uint8_t* buff)
{
	memcpy(buff, UART2_RX_BUFF, UART2_RX_LEN);
}

void DAL_Usart2Init(unsigned int baud)
{
	HAL_Usart2Init(baud);
}

void DAL_Usart2SendByte(unsigned char byte)
{
	HAL_Usart2SendByte(byte);
}

void sDal_Usart2SendData(uint8_t* buff, uint16_t len)
{
	uint16_t i = 0;
	for (i = 0; i < len; i++)
	{
		USART2->DR = buff[i];
		while (!LL_USART_IsActiveFlag_TXE(USART2));
	}
}

/* USART2��ESP8266ռ�� ��������ر�(��ʵ�� ������ȾAT��·) */
void DEBUG_uart2(const char *fmt, ...)
{
	(void)fmt;
}
