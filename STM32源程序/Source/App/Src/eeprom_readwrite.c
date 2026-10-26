
/***********************************Copy Left*****************************************
文件名：    eeprom_readwrite.c
文件描述：  eeprom读写测试，
创建时间：  2023/03/02
修改时间： 
修改内容；

***************************************************************************************/
#include "eeprom_readwrite.h"
#include "task.h"
#include "string.h"
#include "history_record.h"

#define WRITE_ADDR          100     // 写入地址
#define BUF_LEN             20
#define WAIT_TIME           100

static uint8_t readFlag = 1;
	   
static uint8_t writeFlag = 0;
	 
static uint8_t writeBuf[BUF_LEN] = {'E', 'E', 'P', 'R', 'O', 'M', 'T', 'E', 'S', 'T'};
static uint8_t readBuf[BUF_LEN];
	   
static uint8_t byte = 0x00;
static uint8_t ReadWriteMode = 0;   //1：写数据   2：读数据   0：空闲



_INFORMATION stInformation[IC_MAX_NUM];

void sReadSysData(void);


void sSetReadWriteMode(uint8_t flag)
{
	ReadWriteMode = flag;
}

uint8_t sGetReadAndWriteMode(void)
{
	return ReadWriteMode;
}

/****************************************************************
***函数名称：WriteData
***函数功能：写入数据
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void WriteData(void)
{
    if (readFlag)
    {
        return;
    }
    
    if (byte >= 50)         // 写入50次停止测试
    {
        return;
    }

    for (uint8_t i = 0; i < BUF_LEN / 2; i += 2)
    {
        writeBuf[i + 9] = byte + i;
    }
        
    if (!EEPROM_WriteData(WRITE_ADDR, writeBuf, BUF_LEN))
    {
        memset(readBuf, 0x00, BUF_LEN);
        writeFlag = 0;
        readFlag = 1;
        
    }
}

/****************************************************************
***函数名称：ReadData
***函数功能：读取数据
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void ReadData(void)
{   
    if (writeFlag)
    {
        return;
    }
    
    if (!EEPROM_ReadData(WRITE_ADDR, readBuf, 3))
    {
        writeFlag = 1;
        readFlag = 0;
        byte++;
    }
    
}


/****************************************************************
***函数名称：sReadSysData
***函数功能：开机上电后初次读取数据
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void sReadSysData(void)
{
//	static unsigned char index = 0;
	static unsigned char readAddr = 0x20;
	uint8_t ReadBuff[32];
	uint8_t WritdBuff[5];
	uint32_t password = 0;
	uint16_t historyRecordAddr = 0;
	
	
	EEPROM_ReadContinuous(0x00, ReadBuff, 3); //00-03   存储密码
	password += (ReadBuff[0] << 16)
				+(ReadBuff[1] << 8)
				+ReadBuff[2];
	if( (password <= 0) || (password>0x999999) )//默认密码为123456
	{
		password = 0x123456;
		WritdBuff[0] = (password & 0xff0000 )>> 16;
		WritdBuff[1] = (password & 0xff00 ) >> 8;
		WritdBuff[2] = password & 0xff;
		EEPROM_WriteContinuous(0x00,3,WritdBuff);  
	}
	sSetPasswordValue(password); //赋值新密码
	

	EEPROM_ReadContinuous(STROGE_HISTORY_EEPROM_ADDDR, ReadBuff, STROGE_HISTORY_EEPROM_LEN); //05-06   历史记录存储的位置
	
	historyRecordAddr = (ReadBuff[0]<<8)+ReadBuff[1];
	
	if ( ((historyRecordAddr <= 0x100) && (historyRecordAddr>=0x1E0)) || (historyRecordAddr % 0x20 != 0 ))   //一共可以存储8条历史记录
	{

		historyRecordAddr = 0x100;
		WritdBuff[0] = (historyRecordAddr & 0xff00 )>> 8;
		WritdBuff[1] = historyRecordAddr & 0xff;
		
		EEPROM_WriteContinuous(0x05,2,WritdBuff);  //
	}

	sSetHistoryRecordAddr(historyRecordAddr); //获取一条历史记录所存储的位置
	
	for(uint8_t i=0;i<6;i++)
	{
		EEPROM_ReadContinuous(readAddr, ReadBuff, 16);
		EEPROM_ReadContinuous(readAddr+0x10, &ReadBuff[16], 16);
		memcpy(&stInformation[i].year_h,ReadBuff,20);
		readAddr += 0x20;
	}
}

/****************************************************************
***函数名称：EEPROM_ReadWriteInit
***函数功能：EEPROM读写
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void EEPROM_ReadWrite(void)
{
//	if()
//	{
//		
//	}
}


/****************************************************************
***函数名称：ReadData
***函数功能：读取数据
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void ReadWriteDataTask(void)
{
	/*****测试调试EEPROM运行状态*****/

    WriteData();
    ReadData();
	/************/
	
}

