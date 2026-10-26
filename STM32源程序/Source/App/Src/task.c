/***********************************Copy Left*****************************************
文件名：    task.c
文件描述：  任务注册
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#include "task.h"
#include "string.h"

#define MAX_TASK_NUM				        10      // 最大任务数

typedef struct
{                
    unsigned char  readyFlag;                       // 任务就绪标志
	unsigned short taskTime;		                // 任务运行时间
	unsigned short runTime;                         // 任务已经运行时间
	void (*taskHandler)(void);		                // 任务处理指针
}TASK_T;


static TASK_T taskHandle[MAX_TASK_NUM];

/****************************************************************
*函数名称：TaskInit
*函数功能：任务初始化
*输入参数：无
*返回值：无
*备注：无
*****************************************************************/
void TaskInit(void)
{
	memset(taskHandle, 0x00, sizeof(TASK_T) * MAX_TASK_NUM);
}

/****************************************************************
*函数名称：TaskRegister
*函数功能：任务注册函数
*输入参数：task，需要注册的任务，taskTime，任务时间
*返回值：无
*备注：任务时间以1ms为基准
*****************************************************************/
void TaskRegister(void (*task)(void), unsigned short taskTime)
{	
	unsigned char i;
	
	for (i = 0; i < MAX_TASK_NUM; i++)
	{
		if ((taskHandle[i].taskHandler == NULL) && (task != NULL))
		{
			taskHandle[i].taskHandler = task;
			taskHandle[i].taskTime = taskTime;
			break;
		}
	}	
}

/****************************************************************
*函数名称：TaskStart
*函数功能：任务开始
*输入参数：无
*返回值：无
*备注：无
*****************************************************************/
void TaskStart(void)
{
    for (unsigned char i = 0; i < MAX_TASK_NUM; i++)
    {
        if (taskHandle[i].taskHandler != NULL)
        {
            taskHandle[i].readyFlag = 1;
        }
    }
}

/****************************************************************
*函数名称：TaskSchedule
*函数功能：任务调度
*输入参数：无
*返回值：无
*备注：无
*****************************************************************/
void TaskSchedule(void)
{
    for (unsigned char i = 0; i < MAX_TASK_NUM; i++)
    {
        if (taskHandle[i].readyFlag)        // 任务就绪
        {
            taskHandle[i].runTime += 1;     // 运行时间增加
            
            if (taskHandle[i].runTime == taskHandle[i].taskTime)    // 任务时间到
            {	
                if (taskHandle[i].taskHandler != NULL)
                {
                    taskHandle[i].taskHandler();
                    taskHandle[i].runTime = 0;
                }
            }
        }
    }
}


/**
  * @brief Tick中断，1ms中断一次
  */
void SysTick_Handler(void)
{
	TaskSchedule();
}
