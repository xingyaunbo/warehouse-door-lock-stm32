/***********************************Copy Left*****************************************
文件名：    key_control.h
文件描述：  按键控制测试
创建时间：  2023/03/01
修改时间： 
修改内容；

***************************************************************************************/

#ifndef _KEY_CONTROL_H_
#define _KEY_CONTROL_H_	 

#ifdef __cplusplus
extern "C" {
#endif


#include "dal_key.h"
#include "dal_led.h"
#include "dal_relay.h"
#include "SysDoorStatus.h"
#include "oled.h"


extern uint8_t bsetRtcTime;
extern uint8_t bsetPass;
extern uint8_t bsetRtcTime;

void sSetPasswordValue(uint32_t num);
uint32_t sGetPasswordValue(void);
/*---------------- 系统模式定义 ----------------*/
#define SYS_MODE_MENU      0   /* 主菜单(选择验证方式) */
#define SYS_MODE_PASSWORD  1   /* 密码输入模式 */
#define SYS_MODE_NFC       2   /* NFC刷卡模式 */
#define SYS_MODE_FACE      3   /* 人脸功能模式 */

uint8_t sGetSysMode(void);
void KeyControl(void);
void sSetfLenrnICId(uint32_t num);
uint32_t sGetfLenrnICId(void);

#ifdef __cplusplus
}
#endif

#endif








