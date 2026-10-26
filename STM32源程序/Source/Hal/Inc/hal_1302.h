/***********************************Copy Left*****************************************
文件名：    hal_1302.h
文件描述：  LED的GPIO初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_1302_H_
#define _HAL_1302_H_	 

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define  HAL_1302_PRO	GPIOC
#define  HAL_1302_SDA  LL_GPIO_PIN_14
#define  HAL_1302_SCL  LL_GPIO_PIN_15
#define  HAL_1302_CS   LL_GPIO_PIN_13

//#define HAL_RTC_CS_HIGH()              LL_GPIO_SetOutputPin(HAL_1302_PRO,HAL_1302_CS)//GPIOC->ODR |= HAL_1302_CS                              // RTC_CS高
//#define HAL_RTC_CS_LOW()               LL_GPIO_ResetOutputPin(HAL_1302_PRO,HAL_1302_CS)//GPIOC->ODR &= ~HAL_1302_CS      

//#define HAL_RTC_SCLK_HIGH()            LL_GPIO_SetOutputPin(HAL_1302_PRO,HAL_1302_SCL)//GPIOC->ODR |= HAL_1302_SCL       
//#define HAL_RTC_SCLK_LOW()             LL_GPIO_ResetOutputPin(HAL_1302_PRO,HAL_1302_SCL)// GPIOC->ODR &= ~HAL_1302_SCL 

//#define HAL_RTC_SDO_HIGH()             LL_GPIO_SetOutputPin(HAL_1302_PRO,HAL_1302_SDA)//GPIOC->ODR |= HAL_1302_SDA      
//#define HAL_RTC_SDO_LOW()              LL_GPIO_ResetOutputPin(HAL_1302_PRO,HAL_1302_SDA)//GPIOC->ODR &= ~HAL_1302_SDA


#define HAL_RTC_CS_HIGH()              GPIOC->ODR |= GPIO_ODR_ODR13                               
#define HAL_RTC_CS_LOW()               GPIOC->ODR &= ~GPIO_ODR_ODR13      

#define HAL_RTC_SCLK_HIGH()            GPIOC->ODR |= GPIO_ODR_ODR15       
#define HAL_RTC_SCLK_LOW()             GPIOC->ODR &= ~GPIO_ODR_ODR15 

#define HAL_RTC_SDO_HIGH()             GPIOC->ODR |= GPIO_ODR_ODR14      
#define HAL_RTC_SDO_LOW()              GPIOC->ODR &= ~GPIO_ODR_ODR14

#define HAL_RTC_SDO_IN()               {GPIOC->CRH &= ~GPIO_CRH_CNF14;           \
                                        GPIOC->CRH &= ~GPIO_CRH_MODE14;          \
                                        GPIOC->CRH |= GPIO_CRH_CNF14_0;          }
 
#define HAL_RTC_SDO_OUT()              {GPIOC->CRH &= ~GPIO_CRH_CNF14;           \
                                        GPIOC->CRH &= ~GPIO_CRH_MODE14;          \
                                        GPIOC->CRH |= GPIO_CRH_MODE14;           }         

//#define HAL_RTC_SDO_IN()  {GPIOC->CRH&=0XF0FFFFFF;GPIOC->CRH|=0x08000000;}	//PB15输入模式
//#define HAL_RTC_SDO_OUT() {GPIOC->CRH&=0XF0FFFFFF;GPIOC->CRH|=0x03000000;} 	//PB15输出模式
										
#define HAL_RTC_SDO_READ()             (GPIOC->IDR & GPIO_IDR_IDR14)

/****************************************************************
***函数名称：HAL_1302Init
***函数功能：1302 GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_1302Init(void);

#ifdef __cplusplus
}
#endif

#endif
