
/***********************************Copy Left*****************************************
文件名：    eeprom.c
文件描述：  eeprom读写函数，适合24C01~24C16 
创建时间：  2023/03/02
修改时间： 
修改内容；

***************************************************************************************/
#include "eeprom.h"


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define EEPROM_TYPE    AT24C16         //EEPROM型号 


#define READ_WAIT_TIME      1          // 读一个字节等待时间
#define WRITE_WAIT_TIME     1          // 写一个字节等待时间

typedef enum 
{
    EEPROM_BUSY,
    EEPROM_IDLE,
}EEPROM_STATUS_E;           // EEPROM状态

static uint8_t status = EEPROM_IDLE;       // 状态标志
static uint8_t waitTime = 0;               // 等待时间
static uint16_t tempAddr = 0;              // 临时地址
static uint8_t tempLen = 0;                // 临时长度
static uint8_t bufIndex = 0;               // 缓存索引


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
        for (uint8_t i = 0; i < 10; i++)
        {
            //asm("nop");
			__NOP();
        }
    }
}
/****************************************************************
***函数名称：EEPROM_Init
***函数功能：EEPROM接口初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void EEPROM_Init(void)
{
    DAL_IIC_Init();
}
    
/****************************************************************
***函数名称：EEPROM_ReadOneByte
***函数功能：指定地址读出一个数据
***输入参数：readAddr, 读取的地址，byte, 读取字节缓存
***返回值：1，正在读取，0，读取完成
***备注：无
*****************************************************************/
static uint8_t EEPROM_ReadOneByte(uint16_t readAddr, uint8_t *byte)
{
    uint8_t temp = 0;
    
    if (waitTime > 0)                       // 延时读取
    {
        waitTime--;
        
        return 1;
    }
    
    DAL_IIC_Start();
    
    if (EEPROM_TYPE > AT24C16)
    {
        DAL_IIC_SendByte(0xA0);             // 发送命令
        DAL_IIC_WaitAck();
        DAL_IIC_SendByte(readAddr >> 8);    // 发送高地址
    }
    else
    {
        DAL_IIC_SendByte(0xA0 + ((readAddr / 256) << 1));       // 发送期间地址0xA0, 写数据
    }
    
    DAL_IIC_WaitAck();
    DAL_IIC_SendByte(readAddr % 256);       // 发送低地址
    DAL_IIC_WaitAck();
    
    DAL_IIC_Start();
    
    if (EEPROM_TYPE > AT24C16)
    {
        DAL_IIC_SendByte(0xA1);             // 进入接收模式
    }
    else 
    {
        DAL_IIC_SendByte(0xA1 + ((readAddr / 256) << 1));  // 进入接收模式
    }
    
    DAL_IIC_WaitAck();
    temp = DAL_IIC_ReadByte(0);
    DAL_IIC_Stop();                         // 产生一个停止条件
    *byte = temp;
    waitTime = READ_WAIT_TIME;
    
    return 0;
}
/****************************************************************
***函数名称：EEPROM_ReadOneByte
***函数功能：指定地址读出一个数据
***输入参数：readAddr, 读取的地址，byte, 读取字节缓存
***返回值：1，正在读取，0，读取完成
***备注：无
*****************************************************************/
uint8_t EEPROM_ReadContinuous(uint16_t readAddr, uint8_t *byte,uint8_t len)
{
//    uint8_t temp = 0;

    DAL_IIC_Start();
    
    if (EEPROM_TYPE > AT24C16)
    {
        DAL_IIC_SendByte(0xA0);             // 发送命令
        DAL_IIC_WaitAck();
        DAL_IIC_SendByte(readAddr >> 8);    // 发送高地址
    }
    else
    {
        DAL_IIC_SendByte(0xA0 + ((readAddr / 256) << 1));       // 发送期间地址0xA0, 写数据
    }
    
    DAL_IIC_WaitAck();
    DAL_IIC_SendByte(readAddr % 256);       // 发送低地址
    DAL_IIC_WaitAck();
    
    DAL_IIC_Start();
    
    if (EEPROM_TYPE > AT24C16)
    {
        DAL_IIC_SendByte(0xA1);             // 进入接收模式
    }
    else 
    {
        DAL_IIC_SendByte(0xA1 + ((readAddr / 256) << 1));  // 进入接收模式
    }
    
    DAL_IIC_WaitAck();
	while(len)
	{
		*byte = DAL_IIC_ReadByte(0);
		if (len == 1) DAL_IIC_Nack();//不发送ack，读结束
        else DAL_IIC_Ack();//发送ack
        byte++;
        len--;
	}
   
    DAL_IIC_Stop();                         // 产生一个停止条件
	DelayUs(200);
    //LL_mDelay(1);
    return 0;
}

/****************************************************************
***函数名称：EEPROM_WriteOneByte
***函数功能：指定地址写入一个数据
***输入参数： writeAddr，写入地址，byte，写入值
***返回值：1, 正在写入，0，写入完成
***备注：无
*****************************************************************/
uint8_t EEPROM_WriteOneByte(uint16_t writeAddr, uint8_t byte)
{
    if (waitTime > 0)
    {
        waitTime--;
        
        return 1;
    }
    
    DAL_IIC_Start();
    
    if (EEPROM_TYPE > AT24C16)
    {
        DAL_IIC_SendByte(0xA0);             // 发送命令
        DAL_IIC_WaitAck();
        DAL_IIC_SendByte(writeAddr >> 8);    // 发送高地址
    }
    else
    {
        DAL_IIC_SendByte(0xA0 + ((writeAddr / 256) << 1));       // 发送期间地址0xA0, 写数据
    }
    
    DAL_IIC_WaitAck();
    DAL_IIC_SendByte(writeAddr % 256);      // 发送低地址
    DAL_IIC_WaitAck();
    DAL_IIC_SendByte(byte);                 // 发送字节
    DAL_IIC_WaitAck();
    DAL_IIC_Stop();                         // 产生一个停止条件
   // waitTime = WRITE_WAIT_TIME;
    
    return 0;
}
/****************************************************************
***函数名称：EEPROM_WriteOneByte
***函数功能：指定地址写入一个数据
***输入参数： writeAddr，写入地址，byte，写入值
***返回值：1, 正在写入，0，写入完成
***备注：无
*****************************************************************/
uint8_t EEPROM_WriteContinuous(uint16_t writeAddr, uint8_t len ,uint8_t *buff)
{
	uint8_t i;

    DAL_IIC_Start();
    
    if (EEPROM_TYPE > AT24C16)
    {
        DAL_IIC_SendByte(0xA0);             // 发送命令
        DAL_IIC_WaitAck();
        DAL_IIC_SendByte(writeAddr >> 8);    // 发送高地址
    }
    else
    {
        DAL_IIC_SendByte(0xA0 + ((writeAddr / 256) << 1));       // 发送期间地址0xA0, 写数据
    }
    
    DAL_IIC_WaitAck();
    DAL_IIC_SendByte(writeAddr % 256);      // 发送低地址
    DAL_IIC_WaitAck();
	for(i=0;i<len;i++)
	{
		DAL_IIC_SendByte(buff[i]);                 // 发送字节
		DAL_IIC_WaitAck();
	}
    DAL_IIC_Stop();                         // 产生一个停止条件
    //waitTime = WRITE_WAIT_TIME;
	DelayUs(500);
	//LL_mDelay(1);
    
    return 0;
}
/****************************************************************
***函数名称：EEPROM_WriteData
***函数功能：指定地址写入一串数据
***输入参数： writeAddr，写入地址，writeData, 写入的数据，writeLen，写入长度
***返回值：1，正在写入，0，写入完成
***备注：只有返回0时，说明写入完成，因为写数据时，此函数比较周期调用，
因为EEPROM写入速度比较慢，建议调用此函数的时间超过1ms
*****************************************************************/
uint8_t EEPROM_WriteData(uint16_t writeAddr, uint8_t *writeBuf, uint8_t writeLen)
{   
    if (status == EEPROM_IDLE)      // EEPROM空闲
    {
        status = EEPROM_BUSY;
        tempAddr = writeAddr;
        tempLen = writeLen;
        bufIndex = 0; 
    }
    
    if (!EEPROM_WriteOneByte(tempAddr, *(writeBuf + bufIndex)))
    {
        bufIndex++;
        tempAddr++;
        tempLen--;
    }
    
    if (tempLen)
    {
        return 1;
    }
    else 
    {
        status = EEPROM_IDLE;
        
        return 0;
    }
}

/****************************************************************
***函数名称：EEPROM_ReadData
***函数功能：指定地址读取一串数据
***输入参数： readAddr，读取首地址，readBuf, 读取的缓存，readLen，读取长度
***返回值：1，正在写入，0，写入完成
***备注：只有返回0时，说明写入完成，因为读数据时，此函数比较周期调用，
因为EEPROM写入速度比较慢，建议调用此函数的时间超过1ms
*****************************************************************/
uint8_t EEPROM_ReadData(uint16_t readAddr, uint8_t *readBuf, uint8_t readLen)
{
    if (status == EEPROM_IDLE)           // EEPROM空闲
    {
        status = EEPROM_BUSY;
        tempAddr = readAddr;
        tempLen = readLen;
        bufIndex = 0; 
    }
    
    if (!EEPROM_ReadOneByte(tempAddr, readBuf + bufIndex))
    {
        bufIndex++;
        tempAddr++;
        tempLen--;
    }
   
    if (tempLen)
    {
        return 1;
    }
    else 
    {
        status = EEPROM_IDLE;
        
        return 0;
    }
}
 


