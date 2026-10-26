/***********************************Copy Left*****************************************
文件名：    hal_system.c
文件描述：  系统时钟初始化
创建时间：  2023/02/28
修改时间： 
修改内容；

***************************************************************************************/

#include "hal_system.h"




#define SYSTEM_CLOCK           72000000UL
#define SYSTEM_TICK            1000UL
#define SYSTICK_LOAD           (SYSTEM_CLOCK / SYSTEM_TICK - 1)

/****************************************************************
***函数名称：SystemInit
***函数功能：系统时钟初始化
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
//void SystemInit(void)
//{
//	unsigned char temp = 0;
//    unsigned char pll = 8;          // PLL倍频
//	
//	RCC->CR |= RCC_CR_HSEON;          // 外部高速时钟使能HSEON
//    
//	while(!(RCC->CR & RCC_CR_HSERDY));          // 等待外部时钟就绪
//    
//	RCC->CFGR = 0X00000400;         // APB1=DIV2; APB2=DIV1;AHB=DIV1;
//	pll -= 2;                       // 抵消2个单位
//	RCC->CFGR |= pll << 18;         // 设置PLL值 2~16
//	RCC->CFGR |= RCC_CFGR_PLLSRC;	// PLLSRC ON 
//	FLASH->ACR |= 0x32;	            // FLASH 2个延时周期

//	RCC->CR |= RCC_CR_PLLON;        // PLLON
//    
//	while(!(RCC->CR >> 25));        // 等待PLL锁定
//    
//	RCC->CFGR |= RCC_CFGR_SW_PLL;   // PLL作为系统时钟	 
//    
//	while(temp != 0x02)             // 等待PLL作为系统时钟设置成功
//	{   
//		temp = RCC->CFGR >> 2;
//		temp &= 0x03;
//	}    
//    
//    // SysTick初始化
//    SysTick->LOAD = SYSTICK_LOAD;   
//    SysTick->VAL = 0UL;
//    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | 
//                    SysTick_CTRL_CLKSOURCE_Msk | 
//                    SysTick_CTRL_TICKINT_Msk;
//}	
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

   if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
    Error_Handler();  
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(72000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(72000000);
  // SysTick初始化
    SysTick->LOAD = SYSTICK_LOAD;   
    SysTick->VAL = 0UL;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | 
                    SysTick_CTRL_CLKSOURCE_Msk | 
                    SysTick_CTRL_TICKINT_Msk;
}
