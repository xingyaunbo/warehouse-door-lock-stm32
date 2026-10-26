# 基于 STM32 的仓库多认证智能门禁系统

毕业设计：以 STM32F103C8T6 为核心的仓库门禁门锁，支持**人脸识别 / NFC 刷卡 / 密码 / 远程**四种认证方式，通过 ESP8266-01S 接入中国移动 OneNET 云平台，配套微信小程序实现远程监控与控制。

## 功能特性

- **多认证开门**：人脸识别（USART1）、NFC 刷卡（RC522/SPI）、6 位数字密码（EEPROM 掉电保存）
- **云端物模型**：认证事件、门锁状态、人脸数量实时上报；远程开门 / 清除人脸库 / 修改密码在线下发
- **微信小程序**：门锁状态实时显示、门禁动态日志、远程开门、人脸库管理、密码修改
- **本地交互**：OLED 中文分层菜单（主菜单 → 人脸子菜单）、按键操作、继电器开门 5 秒自动关闭、蜂鸣提示
- **启动自检**：上电显示 WiFi 与云平台连接状态，连接成功后进入系统界面

## 硬件组成

| 模块 | 接口/引脚 |
|------|-----------|
| STM32F103C8T6 | 主控 |
| 人脸识别模块 | USART1 (PA9/PA10) 115200 |
| ESP8266-01S | USART2 (PA2/PA3) 115200，独立 AMS1117 3.3V 供电 |
| RC522 NFC | SPI |
| OLED 0.96寸 | 软件 I2C (PB1/PB2) |
| 继电器/蜂鸣器/LED | GPIO |
| AT24C02 EEPROM | I2C（密码存储） |

## 软件架构

采用 Hal / Dal / App 三层架构，基于 LL 库开发，SysTick 1ms 调度器统一 100ms 轮询任务：

```
Source
├── Hal    硬件抽象层（usart/spi/gpio/relay/eeprom...）
├── Dal    器件驱动层（人脸模块协议帧、ESP8266 AT 状态机、RC522、OLED...）
├── App    应用层（菜单/按键/门锁状态机/人脸任务/OneNET 云任务/调度器）
└── Src    main
```

ESP8266 驱动为**非阻塞 AT 状态机**：连 WiFi → MQTTCLEAN → MQTTUSERCFG → MQTTCONN → 订阅 → 在线，支持断线自动分级重连；下发指令采用轻量字符串解析（不依赖 cJSON），自动回复 set_reply。

## OneNET 物模型

| 属性 | 类型 | 方向 | 说明 |
|------|------|------|------|
| LockState | enum | 上报 | 0-开启 1-关闭 |
| AuthType | enum | 上报 | 1-人脸 2-NFC 3-密码 4-远程 |
| AuthResult | bool | 上报 | 认证结果 |
| FaceCount | int | 上报 | 人脸库数量 |
| RemoteUnlock | bool | 下发 | 远程开门 |
| Face_Clear | bool | 下发 | 清除人脸库 |
| Password_Set | string | 下发 | 设置 6 位密码（写入 EEPROM） |

## 目录说明

```
├── STM32源程序    Keil MDK 工程（DoorSys.uvprojx）
├── 微信小程序      OneNET HTTP API 版监控小程序
└── 硬件原理图      原理图(PDF/PNG)、PCB、云平台配置信息
```

## 编译与烧录

1. Keil MDK 5 打开 `STM32源程序/MDK-ARM/DoorSys.uvprojx`，直接编译（0 错误 0 警告）
2. 烧录 `DoorSys.hex`（或 AXF）至 STM32F103C8T6

## 使用说明

1. 在 `STM32源程序/Source/Dal/Src/dal_esp8266.c` 顶部配置 WiFi 名称/密码（当前为 12345678）
2. 上电后 OLED 显示 WiFi / Cloud 连接状态，均 OK 后进入主菜单
3. 主菜单：1-密码 2-NFC 3-人脸；人脸子菜单：1-识别 2-录入 3-清除
4. 微信小程序：导入 `微信小程序` 目录，填入 OneNET 产品级 token（见硬件原理图/云平台配置信息.txt），即可远程查看与控制

## 硬件注意事项

- ESP8266 必须使用**独立 AMS1117 3.3V** 供电（WiFi 发射峰值电流大），并与 STM32 共地
- PA2 → ESP8266 RX，PA3 → ESP8266 TX（交叉连接），EN 脚悬空
- 人脸模块：识别/录入/清除遵循 0xEF 0xAA 帧头 + 异或校验协议
