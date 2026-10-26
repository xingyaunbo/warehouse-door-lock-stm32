/***********************************Copy Left*****************************************
文件名：    hal_iic.c
文件描述：  EEPROM的GPIO初始化
创建时间：  2023/03/02
修改时间： 
修改内容；

***************************************************************************************/
#ifndef _HAL_IIC_H_
#define _HAL_IIC_H_	 

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
   	   		   
////IO方向设置
//#define HAL_SDA_IN()      {GPIOB->CRH &= ~(0xF << 28); \
//                           GPIOB->CRH |= 8 << 28;}
//                          //{GPIOB->CRH &= ~GPIO_CRH_MODE15;   GPIOB->CRH |= GPIO_CRH_CNF15_1;}

//#define HAL_SDA_OUT()     {GPIOB->CRH &= (~(0xF << 28)); \
//                           GPIOB->CRH |= 2 << 28;}
//                          //GPIOB->CRH &= GPIO_CRH_CNF15;     GPIOB->CRH |= GPIO_CRH_MODE15
#define  HAL_IIC_PRO	GPIOB
#define  HAL_IIC_SDA  LL_GPIO_PIN_15
#define  HAL_IIC_SCL  LL_GPIO_PIN_14


						   
#define HAL_SDA_IN()  {GPIOB->CRH&=0X0ffFFFFF;GPIOB->CRH|=0x80000000;}	//PB15输入模式
#define HAL_SDA_OUT() {GPIOB->CRH&=0X0ffFFFFF;GPIOB->CRH|=0x20000000;} 	//PB15输出模式
//IO操作函数	 
#define HAL_SCL_HIGH()    GPIOB->ODR |= (0x01 << 14);           // GPIOB->ODR |= GPIO_ODR_ODR14
#define HAL_SCL_LOW()     GPIOB->ODR &= ~(0x01 << 14);          ///GPIOB->ODR &= ~GPIO_ODR_ODR14

#define HAL_SDA_HIGH()    GPIOB->ODR |= (0x01 << 15);           //GPIOB->ODR |= GPIO_ODR_ODR15
#define HAL_SDA_LOW()     GPIOB->ODR &= ~(0x01 << 15);          //GPIOB->ODR &= ~GPIO_ODR_ODR15

#define HAL_READ_SDA()    (GPIOB->IDR & (1 << 15))//(GPIOB->IDR & GPIO_IDR_IDR15)
 
/****************************************************************
***函数名称：HAL_IIC_Init
***函数功能：IIC的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void HAL_IIC_Init(void);
void DAL_IIC_Init(void);
void DAL_IIC_Start(void);
void DAL_IIC_Stop(void);
uint8_t DAL_IIC_WaitAck(void);
void DAL_IIC_Ack(void);
void DAL_IIC_Nack(void);
void DAL_IIC_SendByte(uint8_t byte);
uint8_t DAL_IIC_ReadByte(uint8_t ack);

#ifdef __cplusplus
}
#endif

#endif

















