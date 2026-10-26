/***********************************Copy Left*****************************************
文件名：    rtc_time.h
文件描述：  RTC时间任务
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "dal_1302.h"

extern RTC_TIME_T SetTime;
extern  RTC_TIME_T ReadTime;

/****************************************************************
***函数名称：RtcTimeInit
***函数功能：RTC时间初始胡
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/

void sSetbSetRtcTimeFlag(uint32_t num);
uint32_t sGetbSetRtcTimeFlag(void);
void RtcTimeInit(void);
void RtcTimeTask(void);

#ifdef __cplusplus
}
#endif

#endif
