/***********************************Copy Left*****************************************
文件名：    hal_usart3.c
文件描述：  串口3初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#include "hal_usart3.h"

#define USAR3_PLCK          36000000UL      // 串口3使用PCLK1为36MHz 串口1使用PCLK2为72MHz

static unsigned char *sendBufPtr = NULL;    // 发送数据指针
static unsigned int sendBufLen = 0;         // 发送数据长度

static unsigned char *recvBufPtr = NULL;    // 接收数据指针
static unsigned int recvBufLen = 0;         // 接收数据长度

/****************************************************************
***函数名称：HAL_Usart3Init
***函数功能：串口3初始化
***输入参数：baud, 波特率
***返回值：无
***备注：波特率计算公式为 baud = FCLK / (16 * USARTDIV)
*****************************************************************/
void HAL_Usart3Init(unsigned int baud)
{
    unsigned short mantissa;
    unsigned char fraction;
    float usartdiv;
    
    RCC->APB2ENR |= RCC_APB1ENR_USART3EN;       // USART3时钟使能
    //RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;         // 使能复用时钟
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;         // 使能PORTB时钟
    

	GPIOB->CRH &= ~GPIO_CRH_CNF10;               // 设置PB10推挽输出  	 
	GPIOB->CRH |= GPIO_CRH_MODE10;               // 设置PB8输出速度50MHz 
    GPIOB->CRH |= GPIO_CRH_CNF10_1;              // 设置PB10推挽输出  

    GPIOB->CRH &= ~GPIO_CRH_CNF11;               // 设置PB11输入 
    GPIOB->CRH |= GPIO_CRH_CNF11_0;              // 浮空输入
    GPIOB->CRH &= ~GPIO_CRH_MODE11;              // 输入模式     
    
    
    usartdiv = (float)(USAR3_PLCK / (16 * baud));
    mantissa = usartdiv;                        // 截取整数部分
    fraction = (usartdiv - mantissa) * 16;      // 截取小数部分
    USART3->BRR = (mantissa << 4) | fraction;   // 设置波特率
    
    USART3->CR1 |= USART_CR1_TXEIE ;            // 发送完成中断
    USART3->CR1 |= USART_CR1_TCIE;              // 接收完成中断
    USART3->CR1 |= USART_CR1_IDLEIE;            // 空闲中断
    USART3->CR1 |= USART_CR1_TE;                // 发送使能
    USART3->CR1 |= USART_CR1_RE;                // 接收使能
    USART3->CR1 |= USART_CR1_UE;                // 串口3使能
    
    NVIC_EnableIRQ(USART3_IRQn);                // 中断使能
}

/****************************************************************
***函数名称：HAL_Usart3SendByte
***函数功能：串口3发送数据
***输入参数：byte，发送一个字节
***返回值：无
***备注：无
*****************************************************************/
void HAL_Usart3SendByte(unsigned char byte)
{
    USART3->DR = byte;
}

/****************************************************************
***函数名称：HAL_Usart3SendData
***函数功能：串口3发送数据
***输入参数：data, 要发送的数据指针，
***返回值：无
***备注：无
*****************************************************************/
void HAL_Usart3SendData(unsigned char *data, unsigned int len)
{
    if (data != NULL)
    {
        
    }
}

/****************************************************************
***函数名称：USART3_IRQHandler
***函数功能：串口3中断处理函数
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void USART3_IRQHandler(void)
{
    if (USART3->SR & USART_SR_TC)               // 发送完成
    {
        USART3->SR |= USART_SR_TC;
    }
    
    if (USART3->SR & USART_SR_RXNE)             // 接收完成
    {
        USART3->SR |= USART_SR_RXNE;
    }
    
    if (USART3->SR & USART_SR_IDLE)             // 空闲中断
    {
        USART3->SR |= USART_SR_IDLE;
    }
}