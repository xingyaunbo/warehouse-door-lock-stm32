/***********************************Copy Left*****************************************
文件名：    dal_key.c
文件描述：  KEY的GPIO初始化
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/
#include "dal_key.h"


void sKey_Delay(uint8_t k)
{
	for(uint8_t i = 0;i<k;i++)
	{
		__NOP();__NOP();__NOP();__NOP();
	}
}



/****************************************************************
***函数名称：DAL_KeyInit
***函数功能：KEY的GPIO初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void DAL_KeyInit(void)
{ 
   sHAL_Matrix_Key_Init(); //矩阵键盘打开
}



uint8_t Matrix_Key_Scan(void)
{
	uint16_t temp = 0;
	uint8_t key_val = 0XAA;

	//第一行
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_6 );
	LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_7 |LL_GPIO_PIN_8|LL_GPIO_PIN_9      \
						| LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13);
	sKey_Delay(10);
	if ( (LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
	{
			sKey_Delay(20);//消抖

			if ( (LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
			{
					temp= ( LL_GPIO_ReadInputPort(GPIOB) & 0x3F80 );		//对列进行扫描
					switch(temp)                                 //第一行
					{
							case 0x3B80:  key_val = 7;   break;
							case 0x3780:  key_val = 8;   break;
							case 0x2F80:  key_val = 9;   break;
							case 0x1F80:  key_val = 12;   break;
							default:    key_val = 0XAA;   break;
					}
			}
	}
	//第二行
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_7 );
	LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_6 |LL_GPIO_PIN_8|LL_GPIO_PIN_9      \
						| LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 );
	sKey_Delay(10);
	if((LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
	{
			sKey_Delay(20);//消抖

			if((LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
			{
					temp= ( LL_GPIO_ReadInputPort(GPIOB) & 0X3F40);		//对列进行扫描
					switch(temp)								//第二行
					{
							case 0x3B40:  key_val = 4;   break;
							case 0x3740:  key_val = 5;   break;
							case 0x2F40:  key_val = 6;   break;
							case 0x1F40:  key_val = 13;   break;
							default:    key_val = 0XAA;   break;
					}
			}
	}
	//第三行
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_8 );
	LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_6 |LL_GPIO_PIN_7|LL_GPIO_PIN_9      \
						| LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 );
	sKey_Delay(10);
	if((LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
	{
			sKey_Delay(20);//消抖

			if((LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
			{
					temp= ( LL_GPIO_ReadInputPort(GPIOB) & 0X3EC0);		//对列进行扫描
					switch(temp)          //第三行
					{
							case 0x3AC0:  key_val = 1;   break;
							case 0x36C0:  key_val = 2;   break;
							case 0x2EC0:  key_val = 3;   break;
							case 0x1EC0:  key_val = 14;   break;
							default:    key_val = 0XAA;   break;
					}
			}
	}
	//第四行
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_9 );
	LL_GPIO_SetOutputPin(GPIOB,LL_GPIO_PIN_6 |LL_GPIO_PIN_7|LL_GPIO_PIN_8     \
						| LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 );
	sKey_Delay(10);
	if((LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
	{
			sKey_Delay(20);//消抖

			if((LL_GPIO_ReadInputPort(GPIOB) & 0x3C00) != 0x3C00 )
			{
					temp= ( LL_GPIO_ReadInputPort(GPIOB) & 0X3DC0);		//对列进行扫描
					switch(temp)          //第四行
					{
							case 0x39C0:  key_val = 10;   break;
							case 0x35C0:  key_val = 0;   break;
							case 0x2DC0:  key_val = 11;   break;
							case 0x1DC0:  key_val = 15;   break;
							default:    key_val = 0XAA;   break;
					}
			}
	}
	
	return key_val;
 
}



