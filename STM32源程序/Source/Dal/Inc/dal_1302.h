/***********************************Copy Left*****************************************
文件名：    hal_1302.h
文件描述：  LED的GPIO初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _DAL_1302_H_
#define _DAL_1302_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_1302.h"
    
typedef struct 
{
    unsigned char second; 
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned char date;
    unsigned char year;
}RTC_TIME_T;

/****************************************************************
***函数名称：DAL_1303Init
***函数功能：1302初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_1302Init(void);
void DAL_1302SetTime(RTC_TIME_T *time);

/****************************************************************
***函数名称：DAL_1303Init
***函数功能：1302初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_1302ReadTime(RTC_TIME_T *time);
    
#ifdef __cplusplus
}
#endif

#endif

