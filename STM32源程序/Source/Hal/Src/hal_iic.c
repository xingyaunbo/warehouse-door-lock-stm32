/***********************************Copy Left*****************************************
文件名：    hal_iic.c
文件描述：  EEPROM的GPIO初始化
创建时间：  2023/03/02
修改时间： 
修改内容；

***************************************************************************************/
#include "hal_iic.h"

#define DELAY_US            10

/****************************************************************
***函数名称：HAL_IIC_Init
***函数功能：IIC的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_IIC_Init(void)
{		
	
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);

    GPIO_InitStruct.Pin = HAL_IIC_SCL | HAL_IIC_SDA ;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(HAL_IIC_PRO, &GPIO_InitStruct);
}


/****************************************************************
***函数名称：DelayUs
***函数功能：延时us
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
static void DelayUs(uint16_t us)
{
    while (us--)
    {
        for (uint8_t i = 0; i < DELAY_US; i++)
        {
            //asm("nop");
			__NOP();
        }
    }
}

/****************************************************************
***函数名称：DAL_IIC_Init
***函数功能：IIC驱动初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_IIC_Init(void)
{
    HAL_IIC_Init();
}

/****************************************************************
***函数名称：DAL_IIC_Start
***函数功能：产生IIC起始信号
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_IIC_Start(void)
{
    HAL_SDA_OUT();  // SDA输出
    HAL_SDA_HIGH();
    HAL_SCL_HIGH();
    DelayUs(4);
    HAL_SDA_LOW();
    DelayUs(4);
    HAL_SCL_LOW();  //钳住I2C总线，准备发送或接收数据 
}	  

/****************************************************************
***函数名称：DAL_IIC_Stop
***函数功能：产生IIC停止信号
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_IIC_Stop(void)
{
    HAL_SDA_OUT();  // SDA输出
    HAL_SCL_LOW();
    HAL_SDA_LOW();
    DelayUs(4);
    HAL_SCL_HIGH();
    HAL_SDA_HIGH(); //发送I2C总线结束信号
    DelayUs(4);
}

/****************************************************************
***函数名称：DAL_IIC_WaitAck
***函数功能：等待应答信号到来
***输入参数：无
***返回值：1，接收应答失败
           0，接收应答成功
***备注：无
*****************************************************************/
uint8_t DAL_IIC_WaitAck(void)
{    
    uint8_t errTime = 0;
    
    HAL_SDA_IN();       //SDA设置为输入  
    HAL_SDA_HIGH();
    DelayUs(1);
    HAL_SCL_HIGH();
    DelayUs(1);
    
    while (HAL_READ_SDA())
    {
        errTime++;
        
        if (errTime > 250)
        {
            DAL_IIC_Stop();
            
            return 1;
        }
    }
    
    HAL_SCL_LOW();     //时钟输出0 	
    DelayUs(5);
    return 0;
} 

/****************************************************************
***函数名称：DAL_IIC_Ack
***函数功能：产生ACK应答
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_IIC_Ack(void)
{ 
    HAL_SCL_LOW();
    HAL_SDA_OUT();
    HAL_SDA_LOW();
    DelayUs(2);
    HAL_SCL_HIGH();
    DelayUs(2);
    HAL_SCL_LOW();
}

/****************************************************************
***函数名称：DAL_IIC_Nack
***函数功能：不产生ACK应答		
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_IIC_Nack(void)
{
    HAL_SCL_LOW();
    HAL_SDA_OUT();
    HAL_SDA_HIGH();
    DelayUs(2);
    HAL_SCL_HIGH();
    DelayUs(2);
    HAL_SCL_LOW();
}		

/****************************************************************
***函数名称：DAL_IIC_SendByte
***函数功能：发送一个字节		
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_IIC_SendByte(uint8_t byte)
{                         
    uint8_t bit = 0;
    
    HAL_SDA_OUT();
    HAL_SCL_LOW();  //拉低时钟开始数据传输
    
    for (uint8_t i = 0; i < 8; i++)
    {
        bit = (byte & 0x80) >> 7;
        
        if (bit)
        {
            HAL_SDA_HIGH();
        }
        else 
        {
            HAL_SDA_LOW();
        }
        
        byte <<= 1;
        DelayUs(2); //对TEA5767这三个延时都是必须的
        HAL_SCL_HIGH();
        DelayUs(2);
        HAL_SCL_LOW();
        DelayUs(2);
    }
    
} 	    

/****************************************************************
***函数名称：DAL_IIC_ReadByte
***函数功能：读1个字节，ack=1时，发送ACK，ack=0，发送nACK   		
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
uint8_t DAL_IIC_ReadByte(uint8_t ack)
{
    uint8_t recv = 0;
    
    HAL_SDA_IN(); //SDA设置为输入
    
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_SCL_LOW();
        DelayUs(2);
        HAL_SCL_HIGH();
        recv <<= 1;
        
        if (HAL_READ_SDA())
        {
            recv++;
        }
        
        DelayUs(2);
    }
    
//    if (ack)
//    {
//        DAL_IIC_Ack();  // 发送ACK
//    }
//    else 
//    {
//        DAL_IIC_Nack();  // 不发送ACK
//    }
    
    return recv;
}


























