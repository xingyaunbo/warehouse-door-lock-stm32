/***********************************Copy Left*****************************************
文件名：    dal_1302.c
文件描述：  DS1302初始化
创建时间：  2023/03/17
修改时间： 
修改内容；

***************************************************************************************/

#include "dal_1302.h"


/****************************************************************
***函数名称：DelayUs
***函数功能：us延时
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
static void DelayUs(unsigned short us)
{
    while (us--)
    {
        for (unsigned char i = 0; i < 15; i++)
        {
            //asm("nop");
			__NOP();
        }
    }
}


/****************************************************************
***函数名称：DAL_1302WriteByte
***函数功能：写一个字节
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
static void DAL_1302WriteByte(unsigned char byte)
{
    unsigned char i = 0;
    
    HAL_RTC_SDO_OUT();
    HAL_RTC_SCLK_LOW();
    
    for (i = 0; i < 8; i++)
    {
        HAL_RTC_SCLK_LOW();
        DelayUs(1);
        
        if (byte & 0x01)
        {
            HAL_RTC_SDO_HIGH();
        }
        else 
        {
            HAL_RTC_SDO_LOW();
        }
        
        DelayUs(1);
        HAL_RTC_SCLK_HIGH();
        byte >>= 1;
    }
}

/****************************************************************
***函数名称：DAL_1302WriteReg
***函数功能：指定地址写入一个字节
***输入参数：addr, 地址，写入的数据
***返回值：无
***备注：无
*****************************************************************/
void DAL_1302WriteReg(unsigned char addr, unsigned char data)
{
    unsigned char tempAddr = addr;
    unsigned char tempData = data;
    
    HAL_RTC_CS_LOW();
    DelayUs(2);
    HAL_RTC_SCLK_LOW();
    DelayUs(1);
    HAL_RTC_CS_HIGH();
    DelayUs(1);
    
    DAL_1302WriteByte(tempAddr);
    DAL_1302WriteByte(tempData);
    HAL_RTC_CS_LOW();
    DelayUs(2);
    HAL_RTC_SCLK_LOW();
    DelayUs(2);   
}

/****************************************************************
***函数名称：DAL_1302ReadReg
***函数功能：指定地址读取一个字节
***输入参数：addr, 地址
***返回值：读取的字节
***备注：无
*****************************************************************/
unsigned char DAL_1302ReadReg(unsigned char addr)
{
    unsigned char temp = addr;
    unsigned char data = 0;
    unsigned char i;
    
    HAL_RTC_CS_LOW();
    DelayUs(5);
    HAL_RTC_SCLK_LOW();
    DelayUs(3);
    HAL_RTC_CS_HIGH();
    DelayUs(3);
    
    DAL_1302WriteByte(temp);
    HAL_RTC_SDO_IN();
    DelayUs(2);
    
    for (i = 0; i < 8; i++)
    {
        DelayUs(2);
        data >>= 1;
        HAL_RTC_SCLK_HIGH();
        DelayUs(4);
        HAL_RTC_SCLK_LOW();
        DelayUs(14);
        
        if (HAL_RTC_SDO_READ())
        {
            data |= 0x80;
        }
    }
    
    DelayUs(2);
    HAL_RTC_CS_LOW();
    DelayUs(2);
    HAL_RTC_SDO_LOW();
    
    return data;
}
/****************************************************************
***函数名称：DAL_1303Init
***函数功能：1302初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_1302ReadTime(RTC_TIME_T *time)
{
    time->second = DAL_1302ReadReg(0x81);
    time->minute = DAL_1302ReadReg(0x83);
    time->hour = DAL_1302ReadReg(0x85);
    time->day = DAL_1302ReadReg(0x87);
    time->month = DAL_1302ReadReg(0x89);
    //time->date = DAL_1302ReadReg(0x8B);
    time->year = DAL_1302ReadReg(0x8D);
}

/****************************************************************
***函数名称：DAL_1302Init
***函数功能：1302初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_1302Init(void)
{
	uint8_t time_data = 0;
    HAL_1302Init();
    DelayUs(50000);
  
	time_data = DAL_1302ReadReg(0x81);
	if(time_data>=0x80)
	{
		 DAL_1302WriteReg(0x8E, 0x00);
		DAL_1302WriteReg(0x80, 0x00);
		DAL_1302WriteReg(0x82, 0x17);
		DAL_1302WriteReg(0x84, 0x17);
		DAL_1302WriteReg(0x86, 0x17);
		DAL_1302WriteReg(0x88, 0x03);
		DAL_1302WriteReg(0x8A, 0x05);
		DAL_1302WriteReg(0x8C, 0x23);
		DAL_1302WriteReg(0x8E, 0x80);
	}

}

/****************************************************************
***函数名称：DAL_1302SetTime
***函数功能：1302设置时间
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_1302SetTime(RTC_TIME_T *time)
{
    DAL_1302WriteReg(0x8E, 0x00);   //写使能
    DAL_1302WriteReg(0x80, time->second);   //秒
    DAL_1302WriteReg(0x82, time->minute);   //分
    DAL_1302WriteReg(0x84, time->hour);   //时
    DAL_1302WriteReg(0x86, time->day );   //日
    DAL_1302WriteReg(0x88, time->month );   //月
    DAL_1302WriteReg(0x8A, time->date);   //星期
    DAL_1302WriteReg(0x8C, time->year);   //年
    DAL_1302WriteReg(0x8E, 0x80);   //关使能
}






