#include "history_record.h"
#include "eeprom_readwrite.h"
#include "stm32f1xx_ll_utils.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>

uint8_t zhangsanBuff[4]={0xBA,0X44,0X04,0XB1};
uint8_t lisiBuff[4]={0X61,0XCF,0X05,0X4B};
uint8_t wangwuBuff[4]={0X0A,0X50,0X18,0XB1};

uint16_t HistoryRecordAddr = 0;

uint8_t ReadHistoryRecordFlag = 0;

uint8_t ClaerHistoryRecordFlag = 0;


void sSetHistoryRecordAddr(uint16_t  addr)
{
	HistoryRecordAddr = addr;
}

uint16_t sGetHistoryRecordAddr(void)
{
	return HistoryRecordAddr;
}

void sSetReadHistoryRecordFlag(uint16_t  addr)
{
	ReadHistoryRecordFlag = addr;
}

uint8_t sGetReadHistoryRecordFlag(void)
{
	return ReadHistoryRecordFlag;
}

void sSetClaerHistoryRecordFlag(uint16_t  addr)
{
	ClaerHistoryRecordFlag = addr;
}

uint8_t sGetClaerHistoryRecordFlag(void)
{
	return ClaerHistoryRecordFlag;
}


void sReadHistoryRecode(void)//读取历史记录
{
	uint8_t i;
	uint8_t ReadBuff[20];
	char strBuff[50] = {0};
	char OutBuff[1024];
	uint16_t ReadAddr = 0x100;

	uint8_t count= 0;
	_INFORMATION *p_Information;
	
	if ( ReadHistoryRecordFlag )
	{
		ReadHistoryRecordFlag = 0;
		memset(OutBuff,0,1024);
		for(i = 0;i<STROGE_HISTORY_MAX_NUM;i++)
		{
			EEPROM_ReadContinuous(ReadAddr, ReadBuff, 16);
			EEPROM_ReadContinuous(ReadAddr+0x10, &ReadBuff[16], 4);
			//EEPROM_ReadContinuous(ReadAddr, ReadBuff, 20); //   历史记录存储的位置
			p_Information = (_INFORMATION*)ReadBuff;
			if(p_Information->flag == 0xAA)
			{
				count++;
				sprintf(strBuff,"历史第%d条:",count);//将时间数据转换出来
				strncat(OutBuff,strBuff,10);
				sprintf(strBuff,"%02x%02x年%02x月%02x日%02x时%02x分;"
				,p_Information->year_h
				,p_Information->year_l
				,p_Information->month
				,p_Information->day
				,p_Information->hour
				,p_Information->minute);//将时间数据转换出来
				strncat(OutBuff,strBuff,23);
				memset(strBuff,0,50);
				sprintf(strBuff,"IC卡：%02X%02X%02X%02X;\r\n"
				,p_Information->IC_id[0]
				,p_Information->IC_id[1]
				,p_Information->IC_id[2]
				,p_Information->IC_id[3]);
				strncat(OutBuff,strBuff,15);//将卡号数据转换出来
//				memset(strBuff,0,50);
//				if( (p_Information->IC_id[0] == zhangsanBuff[0]) 
//					&&(p_Information->IC_id[1] == zhangsanBuff[1])
//					&&(p_Information->IC_id[2] == zhangsanBuff[2])
//					&&(p_Information->IC_id[3] == zhangsanBuff[3]))
//				{
//					sprintf(strBuff,"名字:熊 大;\r\n");   
//				}
//				else if( (p_Information->IC_id[0] == lisiBuff[0]) 
//					&&(p_Information->IC_id[1] == lisiBuff[1])
//					&&(p_Information->IC_id[2] == lisiBuff[2])
//					&&(p_Information->IC_id[3] == lisiBuff[3]))
//				{
//					sprintf(strBuff,"名字:熊 二;\r\n");   
//				}
//				else if( (p_Information->IC_id[0] == wangwuBuff[0]) 
//					&&(p_Information->IC_id[1] == wangwuBuff[1])
//					&&(p_Information->IC_id[2] == wangwuBuff[2])
//					&&(p_Information->IC_id[3] == wangwuBuff[3]))
//				{
//					sprintf(strBuff,"名字:光头强;\r\n");  
//				}
//				else
//				{
//					sprintf(strBuff,"名字:未命名;\r\n");  
//				}
//				

//				strncat(OutBuff,strBuff,14);//将名字转换出来
				
				//DEBUG_uart2("历史记录第%d条:%s\r\n",count,OutBuff);
			}
			
			ReadAddr += 0x20;//读取的索引地址增加0x20;
			
		}
		DEBUG_uart2("%s",OutBuff);
		if(count == 0)
		{
			DEBUG_uart2("系统中没有刷卡记录");
		}
		
	}
}

void sClaerHistoryRecode(void)//清除历史记录
{
	uint8_t i;
	uint16_t addr = 0x10A;//只需要清空有效标志位
	
	if(ClaerHistoryRecordFlag)
	{
		ClaerHistoryRecordFlag  = 0;
		for(i = 0;i<STROGE_HISTORY_MAX_NUM;i++)
		{
			EEPROM_WriteOneByte(addr+i*0x20,0);
			LL_mDelay(1);
			DEBUG_uart2("成功清除第%d历史记录\r\n",i+1);
			//EEPROM_WriteContinuous(StorageAddr,16,&p_Information.year_h);//存储刷卡记录
		}
		
	}
}

void sHistoryRecodeChk(void)
{
	sReadHistoryRecode();
	sClaerHistoryRecode();
}

