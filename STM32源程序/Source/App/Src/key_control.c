/***********************************Copy Left*****************************************
文件名：    key_control.c
文件描述：  按键控制测试
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/

#include "key_control.h"
#include "history_record.h"
#include "rtc_time.h"
#include "led_blink.h"
#include "face_app.h"
#include "onenet_app.h"

#define KEY_UP_TIME         1       // 按键上拉时间
#define KEY_DOWN_TIME       10      // 按键下拉时间

unsigned char keyFlag = 0;      // 按键1标志 

unsigned char keyUpTime = 0;
unsigned char keyDownTime = 0;   // 按键下拉时间
uint8_t Lcd_Key_Display_Addr = 40;
uint8_t key_num = 0;
uint32_t PasswordValue = 0;
uint32_t ScanfPassWare = 0;
uint8_t fLenrnICId = 0;

uint8_t bsetRtcTime = 0;
uint8_t bsetPass = 0;
	
char DispBuffNum[11] = {'0','1','2','3','4','5','6','7','8','9',' '};



void sSetPasswordValue(uint32_t num)
{
	PasswordValue = num;
}
uint32_t sGetPasswordValue(void)
{
	return PasswordValue;
}

void sSetfLenrnICId(uint32_t num)
{
	fLenrnICId = num;
}
uint32_t sGetfLenrnICId(void)
{
	return fLenrnICId;
}

#if 0
/****************************************************************
***函数名称：KeyCheck
***函数功能：按键检测
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
static void KeyCheck(void)
{
    if (!DAL_KEY1_STASE())                  // 按键1下拉
    {
        keyDownTime++;
        keyUpTime = 0;
        
        if (keyDownTime >= KEY_DOWN_TIME)   // 通过计数进行延时防抖，不使用Delay等延时     
        {
            keyDownTime = KEY_DOWN_TIME;
            keyFlag = 1;                   // 按键1下拉
        }
    }
    else 
    {   
        if (keyFlag)                       // 按键1抬起
        {
            keyUpTime++;

            if (keyUpTime >= KEY_UP_TIME)   // 通过计数进行延时防抖，不使用Delay等延时     
            {
                keyUpTime = 0;
                keyFlag = 0;
            }       
        }
    }
}

/****************************************************************
***函数名称：ControlLed
***函数功能：控制LED
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
static void ControlLed(void)
{
    if (keyFlag)              // 按键1按下
    {
        //DAL_LED0_ON();      // 开灯
    }
    else 
    {
       // DAL_LED0_OFF();     // 关灯 
    }
}
#endif
/*---------------- 系统模式控制 ----------------*/
static uint8_t sSysMode = SYS_MODE_MENU;    /* 当前系统模式 */
static uint16_t sModeTimeOutCnt = 0;       /* NFC模式超时计数(250ms) */

uint8_t sGetSysMode(void)
{
    return sSysMode;
}

/* 主菜单显示: 1卡密 2NFC 3人脸 (页2-3) */
static void SysMenuDisplay(void)
{
    OLED_ShowString(0, 2, (uint8_t*)"1", 16);
    OLED_ShowCHinese(8, 2, 13);      /* 卡 */
    OLED_ShowCHinese(24, 2, 14);     /* 密 */
    OLED_ShowString(40, 2, (uint8_t*)"2", 16);
    OLED_ShowString(48, 2, (uint8_t*)"NFC", 16);
    OLED_ShowString(72, 2, (uint8_t*)"3", 16);
    OLED_ShowCHinese(80, 2, 15);     /* 人 */
    OLED_ShowCHinese(96, 2, 16);     /* 脸 */
    OLED_ShowString(112, 2, (uint8_t*)" ", 16);
    OLED_ShowString(120, 2, (uint8_t*)" ", 16);
}

/* 人脸子菜单显示: 1识别 2录入 3清除 (页2-3) */
static void FaceMenuDisplay(void)
{
    OLED_ShowString(0, 2, (uint8_t*)"1", 16);
    OLED_ShowCHinese(8, 2, 17);      /* 识 */
    OLED_ShowCHinese(24, 2, 18);     /* 别 */
    OLED_ShowString(40, 2, (uint8_t*)"2", 16);
    OLED_ShowCHinese(48, 2, 24);     /* 录 */
    OLED_ShowCHinese(64, 2, 25);     /* 入 */
    OLED_ShowString(80, 2, (uint8_t*)"3", 16);
    OLED_ShowCHinese(88, 2, 29);     /* 清 */
    OLED_ShowCHinese(104, 2, 30);    /* 除 */
    OLED_ShowString(120, 2, (uint8_t*)" ", 16);
}

static uint8_t sMenuSel = 0;    /* 主菜单当前选中项: 0卡密 1NFC 2人脸 */
static uint8_t sFaceSel = 0;   /* 人脸菜单当前选中项: 0识别 1录入 2清除 */

/* 消息区显示主菜单当前选中项 */
static void MenuSelShow(void)
{
    OLED_ShowString(40, 6, (uint8_t*)"          ", 16);
    OLED_ShowString(40, 6, (uint8_t*)">>", 16);
    if(sMenuSel == 0)
    {
        OLED_ShowCHinese(56, 6, 13);      /* 卡 */
        OLED_ShowCHinese(72, 6, 14);      /* 密 */
    }
    else if(sMenuSel == 1)
    {
        OLED_ShowString(56, 6, (uint8_t*)"NFC", 16);
    }
    else
    {
        OLED_ShowCHinese(56, 6, 15);     /* 人 */
        OLED_ShowCHinese(72, 6, 16);     /* 脸 */
    }
}

/* 消息区显示人脸菜单当前选中项 */
static void FaceSelShow(void)
{
    OLED_ShowString(40, 6, (uint8_t*)"          ", 16);
    OLED_ShowString(40, 6, (uint8_t*)">>", 16);
    if(sFaceSel == 0)
    {
        OLED_ShowCHinese(56, 6, 17);      /* 识 */
        OLED_ShowCHinese(72, 6, 18);      /* 别 */
    }
    else if(sFaceSel == 1)
    {
        OLED_ShowCHinese(56, 6, 24);      /* 录 */
        OLED_ShowCHinese(72, 6, 25);      /* 入 */
    }
    else
    {
        OLED_ShowCHinese(56, 6, 29);      /* 清 */
        OLED_ShowCHinese(72, 6, 30);      /* 除 */
    }
}

/* 模式切换: 分级刷新标题(页0-1)/标签(页6)/菜单区 */
static void sSetSysMode(uint8_t mode)
{
    sSysMode = mode;
    sModeTimeOutCnt = 0;
    OLED_ShowString(40, 6, (uint8_t*)"          ", 16);      /* 清消息区 */
    OLED_ShowString(0, 0, (uint8_t*)"                ", 16);  /* 清标题区 */
    OLED_ShowString(0, 6, (uint8_t*)"     ", 16);             /* 清标签区 */
    if(mode == SYS_MODE_FACE)
    {
        OLED_ShowCHinese(32, 0, 15);      /* 人 */
        OLED_ShowCHinese(48, 0, 16);      /* 脸 */
        OLED_ShowCHinese(64, 0, 34);      /* 管 */
        OLED_ShowCHinese(80, 0, 35);      /* 理 */
        OLED_ShowCHinese(0, 6, 15);       /* 人 */
        OLED_ShowCHinese(16, 6, 16);      /* 脸 */
        OLED_ShowString(32, 6, (uint8_t*)":", 16);
        sFaceSel = 0;
        FaceMenuDisplay();          /* 人脸子菜单 */
        FaceSelShow();               /* >>识别 */
    }
    else
    {
        SysMenuDisplay();           /* 主菜单 */
        if(mode == SYS_MODE_MENU)
        {
            OLED_ShowCHinese(16, 0, 0);   /* 智能门禁系统 */
            OLED_ShowCHinese(32, 0, 1);
            OLED_ShowCHinese(48, 0, 2);
            OLED_ShowCHinese(64, 0, 3);
            OLED_ShowCHinese(80, 0, 4);
            OLED_ShowCHinese(96, 0, 5);
            sMenuSel = 0;
            MenuSelShow();          /* >>卡密 */
        }
        else if(mode == SYS_MODE_PASSWORD)
        {
            OLED_ShowCHinese(32, 0, 14);   /* 密 */
            OLED_ShowCHinese(48, 0, 38);   /* 码 */
            OLED_ShowCHinese(64, 0, 36);   /* 验 */
            OLED_ShowCHinese(80, 0, 37);   /* 证 */
            OLED_ShowCHinese(0, 6, 14);    /* 密 */
            OLED_ShowCHinese(16, 6, 38);   /* 码 */
            OLED_ShowString(32, 6, (uint8_t*)":", 16);
        }
        else                        /* NFC */
        {
            OLED_ShowString(36, 0, (uint8_t*)"NFC", 16);
            OLED_ShowCHinese(60, 0, 36);   /* 验 */
            OLED_ShowCHinese(76, 0, 37);   /* 证 */
            OLED_ShowString(0, 6, (uint8_t*)"NFC:", 16);
        }
    }
}

void sKey_LCD_Analysis(uint8_t key_num)
{
	static uint8_t count= 0;
	char displaybuff[1];
	
	static uint8_t Display_ok_on_Flag = 0;
	static uint8_t TimeOut = 0;
	static uint8_t sPwdResultFlag = 0;    /* 密码结果显示完成后回主菜单 */
	uint8_t bSetPassWordBuff[3];
	char strBuff[50] = {0};
	
	/*---------------- 模式按键拦截(菜单/NFC/人脸模式) ----------------*/
	if(!bsetPass && !bsetRtcTime)
	{
		switch(sSysMode)
		{
			case SYS_MODE_MENU:
				if(key_num == 10)         /* 切换选择项 */
				{
					sMenuSel = (sMenuSel + 1) % 3;
					MenuSelShow();
					return;
				}
				if(key_num == 11)         /* 确认选择 */
				{
					if(sMenuSel == 0)
					{
						sSetSysMode(SYS_MODE_PASSWORD);
					}
					else if(sMenuSel == 1)
					{
						sSetSysMode(SYS_MODE_NFC);
						OLED_ShowString(40, 6, (uint8_t*)"   NFC   ", 16);
					}
					else
					{
						sSetSysMode(SYS_MODE_FACE);
					}
					return;
				}
				if(key_num == 1)          /* 1-密码 */
				{
					sSetSysMode(SYS_MODE_PASSWORD);
					return;
				}
				if(key_num == 2)          /* 2-NFC */
				{
					sSetSysMode(SYS_MODE_NFC);
					OLED_ShowString(40, 6, (uint8_t*)"   NFC   ", 16);
					return;
				}
				if(key_num == 3)          /* 3-人脸 */
				{
					sSetSysMode(SYS_MODE_FACE);
					return;
				}
				if(key_num != 12 && key_num != 13 && key_num != 15)
				{
					return;                /* 其余按键忽略 */
				}
				break;
			case SYS_MODE_NFC:
				if(key_num == 10)         /* 返回主菜单 */
				{
					sSetSysMode(SYS_MODE_MENU);
					return;
				}
				if(key_num != 12 && key_num != 13 && key_num != 15)
				{
					return;
				}
				break;
			case SYS_MODE_FACE:
				if(key_num == 1)          /* 人脸识别 */
				{
					FaceStartVerify();
					return;
				}
				if(key_num == 2)          /* 人脸录入 */
				{
					FaceStartEnroll();
					return;
				}
				if(key_num == 3)          /* 删除所有人脸 */
				{
					FaceDeleteAll();
					return;
				}
				if(key_num == 10)         /* 切换选择项 */
				{
					sFaceSel = (sFaceSel + 1) % 3;
					FaceSelShow();
					return;
				}
				if(key_num == 11)         /* 确认执行 */
				{
					if(sFaceSel == 0)
					{
						FaceStartVerify();
					}
					else if(sFaceSel == 1)
					{
						FaceStartEnroll();
					}
					else
					{
						FaceDeleteAll();
					}
					return;
				}
				if(key_num == 14)         /* 返回主菜单 */
				{
					sSetSysMode(SYS_MODE_MENU);
					return;
				}
				if(key_num != 12 && key_num != 13 && key_num != 15)
				{
					return;
				}
				break;
			default:                     /* 密码模式: 走原有输入逻辑 */
				break;
		}
	}
	/* NFC模式无操作20s超时返回主菜单 */
	if(sSysMode == SYS_MODE_NFC)
	{
		if(++sModeTimeOutCnt >= 80)
		{
			sSetSysMode(SYS_MODE_MENU);
		}
	}
	else
	{
		sModeTimeOutCnt = 0;
	}
	switch(key_num)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			if(bsetPass)
			{
				ScanfPassWare = (key_num + ScanfPassWare*16);
				if(ScanfPassWare>0x999999)  //密码只能输入6位数
				{
					Display_ok_on_Flag = 1;//显示标志位
					count = 0;
					ScanfPassWare = 0;
					Lcd_Key_Display_Addr = 40;
					TimeOut = 3;//减少显示的时间
					OLED_ShowString(40, 6, (uint8_t*)"   ERR  ", 16);
				}
				else
				{
					if(count == 0)
					{
						OLED_ShowString(40, 6, (uint8_t*)"         ", 16);
					}
					Lcd_Key_Display_Addr = count * 8 + 40;   //从地址40开始显示
					displaybuff[0] = DispBuffNum[key_num];
					count++;
					OLED_ShowString(Lcd_Key_Display_Addr, 6, (uint8_t*)displaybuff, 16);
				}
			}
			else if(bsetRtcTime == 1)
			{
				Lcd_Key_Display_Addr = count * 6 + 18;   //从地址40开始显示
				displaybuff[0] = DispBuffNum[key_num];
				count++;
				if(count<=2)
				{
					ScanfPassWare = (key_num + ScanfPassWare*16);
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)displaybuff, 12);
				}
				else
				{
					Lcd_Key_Display_Addr = 18;   //从地址40开始显示
					ScanfPassWare = 0;
					count = 0;
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)"  ", 12);
				}
				
			}
			else if(bsetRtcTime == 2)
			{
				//ScanfPassWare = (key_num + ScanfPassWare*10);
				Lcd_Key_Display_Addr = count * 6 + 36;   //从地址40开始显示
				displaybuff[0] = DispBuffNum[key_num];
				count++;
				if(count<=2)
				{
					ScanfPassWare = (key_num + ScanfPassWare*16);
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)displaybuff, 12);
				}
				else
				{
					Lcd_Key_Display_Addr = 36;   //从地址40开始显示
					ScanfPassWare = 0;
					count = 0;
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)"  ", 12);
				}
			}
			else if(bsetRtcTime == 3)
			{
				//ScanfPassWare = (key_num + ScanfPassWare*10);
				Lcd_Key_Display_Addr = count * 6 + 54;   //从地址40开始显示
				displaybuff[0] = DispBuffNum[key_num];
				count++;
				if(count<=2)
				{
					ScanfPassWare = (key_num + ScanfPassWare*16);
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)displaybuff, 12);
				}
				else
				{
					Lcd_Key_Display_Addr = 54;   //从地址40开始显示
					ScanfPassWare = 0;
					count = 0;
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)"  ", 12);
				}
			}
			else if(bsetRtcTime == 4)
			{
				//ScanfPassWare = (key_num + ScanfPassWare*10);
				Lcd_Key_Display_Addr = count * 6 + 78;   //从地址40开始显示
				displaybuff[0] = DispBuffNum[key_num];
				count++;
				if(count<=2)
				{
					ScanfPassWare = (key_num + ScanfPassWare*16);
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)displaybuff, 12);
				}
				else
				{
					Lcd_Key_Display_Addr = 78;   //从地址40开始显示
					ScanfPassWare = 0;
					count = 0;
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)"  ", 12);
				}
			}
			else if(bsetRtcTime == 5)
			{
				//ScanfPassWare = (key_num + ScanfPassWare*10);
				Lcd_Key_Display_Addr = count * 6 + 96;   //从地址40开始显示
				displaybuff[0] = DispBuffNum[key_num];
				count++;
				if(count<=2)
				{
					ScanfPassWare = (key_num + ScanfPassWare*16);
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)displaybuff, 12);
				}
				else
				{
					Lcd_Key_Display_Addr = 96;   //从地址40开始显示
					ScanfPassWare = 0;
					count = 0;
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)"  ", 12);
				}
			}
			else if(bsetRtcTime == 6)
			{
				//ScanfPassWare = (key_num + ScanfPassWare*10);
				Lcd_Key_Display_Addr = count * 6 + 114;   //从地址40开始显示
				displaybuff[0] = DispBuffNum[key_num];
				count++;
				if(count<=2)
				{
					ScanfPassWare = (key_num + ScanfPassWare*16);
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)displaybuff, 12);
				}
				else
				{
					Lcd_Key_Display_Addr = 114;   //从地址40开始显示
					ScanfPassWare = 0;
					count = 0;
					OLED_ShowString(Lcd_Key_Display_Addr, 2, (uint8_t*)"  ", 12);
				}
			}
			
			else
			{
				ScanfPassWare = (key_num + ScanfPassWare*16);
				if(ScanfPassWare>0x999999)  //密码只能输入6位数
				{
					Display_ok_on_Flag = 1;//显示标志位
					count = 0;
					ScanfPassWare = 0;
					Lcd_Key_Display_Addr = 40;
					TimeOut = 3;//减少显示的时间
					OLED_ShowString(40, 6, (uint8_t*)"   ERR  ", 16);
				}
				else
				{
					Lcd_Key_Display_Addr = count * 8 + 40;   //从地址40开始显示
					displaybuff[0] = DispBuffNum[key_num];
					count++;
					OLED_ShowString(Lcd_Key_Display_Addr, 6, (uint8_t*)displaybuff, 16);
				}
			}
			break;
		case 10://清除之前输入的数据
			
			if( bsetRtcTime == 1)//年
			{
				OLED_ShowString(18, 2, (uint8_t*)"  ", 12);
			}
			else if( bsetRtcTime == 2)//月
			{
				OLED_ShowString(36, 2, (uint8_t*)"  ", 12);
			}
			else if( bsetRtcTime == 3)//日
			{
				OLED_ShowString(54, 2, (uint8_t*)"  ", 12);
			}
			else if( bsetRtcTime == 4)//时
			{
				OLED_ShowString(78, 2, (uint8_t*)"  ", 12);
			}
			else if( bsetRtcTime == 5)//分
			{
				OLED_ShowString(96, 2, (uint8_t*)"  ", 12);
			}
			else if( bsetRtcTime == 6)//秒
			{
				OLED_ShowString(114, 2, (uint8_t*)"  ", 12);
			}
			else
			{
//				count = 0;
//				ScanfPassWare = 0;
				Lcd_Key_Display_Addr = 40;
				OLED_ShowString(40, 6, (uint8_t*)"        ", 16);
			}
			count = 0;
			ScanfPassWare = 0;
			
			//显示6个空格
			break;
		case 11://确认
			if( bsetPass == 1 )
			{
				Display_ok_on_Flag = 1;
				bsetPass = 0;
				if( (ScanfPassWare <= 0x999999) && (count == 6) )
				{
					bSetPassWordBuff[0] = (ScanfPassWare & 0xff0000) >> 16;
					bSetPassWordBuff[1] = (ScanfPassWare & 0x00ff00) >> 8;
					bSetPassWordBuff[2] = (ScanfPassWare & 0xff);
					OLED_ShowString(40, 6, (uint8_t*)"   OK!   ", 16);
					sSetPasswordValue(ScanfPassWare);
					EEPROM_WriteContinuous(0x00,3,bSetPassWordBuff);
					DEBUG_uart2("密码设置成功");
				}
				else
				{
					OLED_ShowString(40, 6, (uint8_t*)"   ERR  ", 16);
				}
//				count = 0;
//				ScanfPassWare = 0;
				Lcd_Key_Display_Addr = 40;
				TimeOut = 3;//减少显示的时间
			}
			else if( bsetRtcTime == 1)//年
			{
				if(ScanfPassWare == 0)
				{
					SetTime.year = ReadTime.year;
				}
				else
				{
					SetTime.year = ScanfPassWare;
				}

				bsetRtcTime = 2;
				sprintf(strBuff, " 20%02x/  /%02x  %02x:%02x:%02x"
			        , SetTime.year
			        //, ReadTime.month
			        , ReadTime.day
			        , ReadTime.hour
			        , ReadTime.minute
					, ReadTime.second); //将时间数据转换出来
				OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
			}
			else if( bsetRtcTime == 2)//月
			{
				if(ScanfPassWare == 0)
				{
					SetTime.month = ReadTime.month;
				}
				else
				{
					SetTime.month = ScanfPassWare;
				}

				bsetRtcTime = 3;
				sprintf(strBuff, " 20%02x/%02x/    %02x:%02x:%02x"
			        , SetTime.year
			        , SetTime.month
			        //, ReadTime.day
			        , ReadTime.hour
			        , ReadTime.minute
					, ReadTime.second); //将时间数据转换出来
				OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
			}
			else if( bsetRtcTime == 3)//日
			{
				if(ScanfPassWare == 0)
				{
					SetTime.day = ReadTime.day;
				}
				else
				{
					SetTime.day = ScanfPassWare;
				}

				bsetRtcTime = 4;
				sprintf(strBuff, " 20%02x/%02x/%02x    :%02x:%02x"
			        , SetTime.year
			        , SetTime.month
			        , SetTime.day
			        //, ReadTime.hour
			        , ReadTime.minute
					, ReadTime.second); //将时间数据转换出来
				OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
			}
			else if( bsetRtcTime == 4)//时
			{
				if(ScanfPassWare == 0)
				{
					SetTime.hour = ReadTime.hour;
				}
				else
				{
					SetTime.hour = ScanfPassWare;
				}

				bsetRtcTime = 5;
				sprintf(strBuff, " 20%02x/%02x/%02x  %02x:  :%02x"
			        , SetTime.year
			        , SetTime.month
			        , SetTime.day
			        , SetTime.hour
			        //, ReadTime.minute
					, ReadTime.second); //将时间数据转换出来
				OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
			}
			else if( bsetRtcTime == 5)//分
			{
				if(ScanfPassWare == 0)
				{
					SetTime.minute = ReadTime.minute;
				}
				else
				{
					SetTime.minute = ScanfPassWare;
				}

				bsetRtcTime = 6;
				sprintf(strBuff, " 20%02x/%02x/%02x  %02x:%02x:  "
			        , SetTime.year
			        , SetTime.month
			        , SetTime.day
			        , SetTime.hour
			        , SetTime.minute
					//, ReadTime.second
					); //将时间数据转换出来
				OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
			}
			else if( bsetRtcTime == 6)//秒
			{
				Display_ok_on_Flag = 1;
				if(ScanfPassWare == 0)
				{
					SetTime.second = ReadTime.second;
				}
				else
				{
					SetTime.second = ScanfPassWare;
				}
				bsetRtcTime = 0;
				DAL_1302SetTime(&SetTime);
				sPwdResultFlag = 1;
				OLED_ShowString(40, 6, (uint8_t*)"   OK!   ", 16);//设置时间成功
			}
			else
			{
				Display_ok_on_Flag = 1;
				sPwdResultFlag = 1;
				if( ScanfPassWare == sGetPasswordValue())
				{
					//开门
					sSetDoorStatus(1);//开门
					sSetBuzzerStatus(1); OneNet_ReportAuth(3, 1);
					OLED_ShowString(40, 6, (uint8_t*)"   OK!   ", 16);
				}
				else
				{
					//提示失败
					sSetBuzzerStatus(2); OneNet_ReportAuth(3, 0);
					OLED_ShowString(40, 6, (uint8_t*)"   NO!   ", 16);
				}
//				count = 0;
//				ScanfPassWare = 0;
				Lcd_Key_Display_Addr = 40;
			}
			count = 0;
			ScanfPassWare = 0;
			
		
			
			//显示6个空格
			break;
		
		case 12:   //查询历史记录    第1排第4个
			
			sSetReadHistoryRecordFlag(1); //打开查询历史记录的标志位
			break;
		case 13:   //设置时间 //清除所有历史记录      第2排第4个
			sSetClaerHistoryRecordFlag(1);
			DEBUG_uart2("清除历史记录");
			
//			if( ScanfPassWare == sGetPasswordValue())
//			{
//				OLED_ShowString(40, 6, (uint8_t*)" SetCode", 16);//请刷卡
//				bsetPass  = 1; //设置密码
//				count = 0;
//				ScanfPassWare = 0;
//				
//			}
			break;
		case 14:   //历史下一条      第3排第4个
//			bsetRtcTime = 1;//设置时间
//			DAL_1302ReadTime(&ReadTime);
//			sprintf(strBuff, " 20  /%02x/%02x  %02x:%02x:%02x"
//			        //, ReadTime.year
//			        , ReadTime.month
//			        , ReadTime.day
//			        , ReadTime.hour
//			        , ReadTime.minute
//					, ReadTime.second); //将时间数据转换出来
//			OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
//			OLED_ShowString(40, 6, (uint8_t*)" SetTime", 16);//设置时间
		    //OLED_ShowString(0, 2, (uint8_t*)" 20  /  /      :  :  ", 12);
			//DAL_1302SetTime(&SetTime);
		
			break;
		case 15:   //录入新的卡ID            第4排第4个
			Display_ok_on_Flag = 1;
			if( ScanfPassWare == sGetPasswordValue())
			{
				
				sSetfLenrnICId(1);//支持开始录入
				OLED_ShowString(40, 6, (uint8_t*)" Learning", 16);//请刷卡
			}
			else if( ScanfPassWare == 0x111111)
			{
				sSetfLenrnICId(2);//开始删除卡片
				OLED_ShowString(40, 6, (uint8_t*)" Clearning!", 16);//请刷卡
			}
			else if( ScanfPassWare == 0x444444)//密码恢复出厂设置
			{
				//sSetfLenrnICId(2);//开始删除卡片
				OLED_ShowString(40, 6, (uint8_t*)" 123456", 16);//请刷卡
				bSetPassWordBuff[0] = 0x12;
				bSetPassWordBuff[1] = 0x34;
				bSetPassWordBuff[2] = 0x56;
				//OLED_ShowString(40, 6, (uint8_t*)"   OK!   ", 16);
				sSetPasswordValue(0x123456);
				EEPROM_WriteContinuous(0x00,3,bSetPassWordBuff);
			}
			else if( ScanfPassWare == 0x222222)
			{
				OLED_ShowString(40, 6, (uint8_t*)" SetCode", 16);//请刷卡
				bsetPass  = 1; //设置密码
				count = 0;
				ScanfPassWare = 0;
			}
			else if( ScanfPassWare == 0x333333)
			{
				bsetRtcTime = 1;//设置时间
				DAL_1302ReadTime(&ReadTime);
				sprintf(strBuff, " 20  /%02x/%02x  %02x:%02x:%02x"
			        //, ReadTime.year
			        , ReadTime.month
			        , ReadTime.day
			        , ReadTime.hour
			        , ReadTime.minute
					, ReadTime.second); //将时间数据转换出来
				OLED_ShowString(0, 2, (uint8_t*)strBuff, 12);
				OLED_ShowString(40, 6, (uint8_t*)" SetTime", 16);//设置时间
			}
			
			else
			{
				//提示失败
				OLED_ShowString(40, 6, (uint8_t*)"  Fault!  ", 16);//失败
			}
			count = 0;
			TimeOut = 0;
			ScanfPassWare = 0;
			Lcd_Key_Display_Addr = 40;
			
			
			break;
		
		default:
			break;
			
	}
	if( Display_ok_on_Flag)
	{
		TimeOut ++ ;
		if(TimeOut >= 5)   //清除OK或者ON
		{
			TimeOut = 0;
			Display_ok_on_Flag = 0;
			//sSetfLenrnICId(0);
			OLED_ShowString(40, 6, (uint8_t*)"         ", 16);
			if(sPwdResultFlag) /* tail return-to-menu */
			{
				sPwdResultFlag = 0;
				if(sSysMode == SYS_MODE_PASSWORD)
				{
				sSetSysMode(SYS_MODE_MENU);
				}
			}
		}
	}

}


/****************************************************************
***函数名称：KeyControl
***函数功能：按键控制测试
***输入参数：无
***返回值：无
***备注：无
*****************************************************************/
void KeyControl(void)
{
	static uint8_t timecount = 0;
	static uint8_t uiInited = 0;
	if(!uiInited)            /* 上电绘制主菜单 */
	{
		uiInited = 1;
		sSetSysMode(SYS_MODE_MENU);          /* 初始选中: 卡密 */
	}
    //KeyCheck();
    //ControlLed();
	key_num = Matrix_Key_Scan();
	timecount++;
	if(timecount >= 1)
	{
		timecount = 0;
		sKey_LCD_Analysis(key_num);
	}
}

