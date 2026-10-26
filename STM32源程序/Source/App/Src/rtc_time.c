/***********************************Copy Left*****************************************
文件名：    rtc_time.c
文件描述：  DS1302时间读取
创建时间：  2023/03/17
修改时间： 
修改内容；

***************************************************************************************/

#include "dal_1302.h"
#include "task.h"
#include "string.h"
#include <stdio.h>
#include <stdarg.h>
#include "oled.h"
#include "key_control.h"

RTC_TIME_T ReadTime;
RTC_TIME_T SetTime;
uint8_t bSetRtcTimeFlag = 0;


void sSetbSetRtcTimeFlag(uint32_t num)
{
	bSetRtcTimeFlag = num;
}
uint32_t sGetbSetRtcTimeFlag(void)
{
	return bSetRtcTimeFlag;
}



/****************************************************************
***函数名称：LedBlink
***函数功能：LED闪灯
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
//void RtcTimeTask(void)
//{
//	char strBuff[50] = {0};
//	uint8_t readFlag = 0;
//	static uint8_t formertime = 0;
//	static uint8_t timecount = 0;
//	//char OutBuff[128];
//	
//    DAL_1302ReadTime(&ReadTime);
//	if(timecount == 0)
//	{
//		timecount = 1;
//		OLED_ShowString(104, 2, (uint8_t*)":", 16);
//	}
//	else
//	{
//		timecount =0;
//		OLED_ShowString(104, 2, (uint8_t*)" ", 16);
//	}
//	
//	if(bSetRtcTimeFlag)  //如果有设置时间请求
//	{
//		bSetRtcTimeFlag = 0;
//		readFlag = 1;
//		DAL_1302SetTime(&SetTime);
//	}
//	if( (formertime != ReadTime.minute) || (readFlag == 1) ) //保证每分钟刷一次数据
//	{
//		readFlag = 0;
//		formertime = ReadTime.minute;
//		sprintf(strBuff,"20%02x/%02x/%02x %02x %02x"
//				,ReadTime.year
//				,ReadTime.month
//				,ReadTime.day
//				,ReadTime.hour
//				,ReadTime.minute);//将时间数据转换出来
//		OLED_ShowString(0, 2, (uint8_t*)strBuff, 16);
//	}
//	
//}
void RtcTimeTask(void)
{
	if(bsetRtcTime == 0)
	{
		DAL_1302ReadTime(&ReadTime);
		if(bSetRtcTimeFlag)  //如果有设置时间请求
		{
			bSetRtcTimeFlag = 0;
			DAL_1302SetTime(&SetTime);
		}
	/* 页2-3改作模式菜单区, 不再显示时间 */
	}
	else
	{
	}
	
//	if( bsetRtcTime == 1)
//	{
//		if(readFlag)
//		{
//			
//		}
//		sprintf(strBuff, "/%02x/%02x  %02x:%02x:"
//			        //, ReadTime.year
//			        , ReadTime.month
//			        , ReadTime.day
//			        , ReadTime.hour
//			        , ReadTime.minute); //将时间数据转换出来
//		OLED_ShowString(24, 2, (uint8_t*)strBuff, 12);
//		
//	}
//	else if(bsetRtcTime == 2)
//	{
//		
//	}
}

/****************************************************************
***函数名称：RtcTimeInit
***函数功能：RTC时间初始胡
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void RtcTimeInit(void)
{
    TaskRegister(RtcTimeTask, TASK_TIME_1000MS);         // 任务注册
}



