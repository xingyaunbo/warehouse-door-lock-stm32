#include "DataAnalysis.h"
#include "string.h"
#include "history_record.h"
#include "stm32f1xx_ll_utils.h"
#include "rtc_time.h"
#include "key_control.h"
#include "led_blink.h"
#include "dal_ble.h"
#include "onenet_app.h"

void sUart2DataAnalysis(void);
void sRf522DataAnalysis(void);
int ArrayToStr(unsigned char *Buff, unsigned int BuffLen, char *OutputStr);


void sData_Anlaysis(void)
{
	sUart2DataAnalysis();
	sRf522DataAnalysis();

}
	

void sUart2DataAnalysis(void)
{
	uint8_t buff[50];
	_USART_RX_INFO* pUartRxInfo;
	uint8_t* bpPointer = 0;
	uint32_t Password = 0;
	uint8_t Passwordbuff[3];
	uint8_t i = 0;
	uint8_t StudentNumBuff[9];
	uint8_t IC_idNumBuff[4];
	uint8_t strogeBuff[32];
	uint8_t tx_buff[RECV_MAX_LEN];
	uint16_t tx_len = 0;
//	uint8_t ReadBuff[32];
	uint16_t CrcValue = 0;
	uint8_t f_Operation_ok = 0;
	uint16_t Error_code = 0;
	char strBuff[4] = {0};
	char OutBuff[20];
	
	if(sReadUart2RxFlag())//读取是否接收到新的有效数据
	{
		sSetRX_FLAG(0); //接受标志位清空
		
		sReadUart2RxBuff(buff);//获取接收到的数据
		pUartRxInfo = (_USART_RX_INFO *)buff;//数据格式转换
		
		bpPointer = (uint8_t*)&pUartRxInfo->ControlData;
		tx_buff[tx_len++] = SEND_DATA_HEAD;
		tx_buff[tx_len++] = RECV_DATA_HEAD;
		tx_buff[tx_len++] = pUartRxInfo->FunctionCode;
		tx_buff[tx_len++] = pUartRxInfo->ControlAdd;
		tx_buff[tx_len++] = 0;//数据有效长度
		
		if( (buff[1] == QUERY_CMD) || ( buff[1] == CONTROL_CMD) )
		{
			if(pUartRxInfo->FunctionCode == QUERY_CMD) //查询指令
			{
				switch(pUartRxInfo->ControlAdd)
				{
					case 1:
						if(pUartRxInfo->DataLen == 1)  //查询历史记录
						{
							if( *bpPointer )
							{
								if(!sGetReadHistoryRecordFlag())
								{
									sSetReadHistoryRecordFlag(1);
								}
							}
							else
							{
								DEBUG_uart2("查询数据错误");
							}
						}
						else
						{
							DEBUG_uart2("查询数据长度错误");
						}
					break;
					
					case 2://查询当前时间
						if(pUartRxInfo->DataLen == 1)  //查询历史记录
						{
							if( *bpPointer )
							{
								DAL_1302ReadTime(&ReadTime);
								tx_buff[tx_len++] = 0x20;
								tx_buff[tx_len++] = ReadTime.year;
								tx_buff[tx_len++] = ReadTime.month;
								tx_buff[tx_len++] = ReadTime.day;
								tx_buff[tx_len++] = ReadTime.hour;
								tx_buff[tx_len++] = ReadTime.minute;
								tx_buff[tx_len++] = ReadTime.second;
							}
						}
						
						
					break;
					
					case 3://当前密码
						if(pUartRxInfo->DataLen == 1)  //查询历史记录
						{
							if( *bpPointer )
							{
								tx_buff[tx_len++] = (sGetPasswordValue() >>16) & 0xFF;
								tx_buff[tx_len++] = (sGetPasswordValue() >>8) & 0xFF;
								tx_buff[tx_len++] = sGetPasswordValue() & 0xFF;
							}
						}
						
					break;
					
					case 4:
					break;
					
					case 5:
					break;
					
					default:
					break;
				}
				
			}
			else if(pUartRxInfo->FunctionCode == CONTROL_CMD)//控制指令
			{
				switch(pUartRxInfo->ControlAdd)
				{
					case 1:
						if( *bpPointer )
						{
							//开门标志位
							//DEBUG_uart2("开门");
							sSetDoorStatus(1);//开门
						}
						else
						{
							//关门标志位
							//DEBUG_uart2("关门");
							sSetDoorStatus(0);//开门   5秒自动关闭
						}
					break;
					
					case 2://录卡
						if(pUartRxInfo->DataLen == 1)  //查询历史记录
						{
							if( *bpPointer )
							{
								sSetfLenrnICId(1);
								OLED_ShowString(40, 6, (uint8_t*)" Learning", 16);//请刷卡
							}
						}
					break;
					
					case 3://删卡
						if(pUartRxInfo->DataLen == 1)  //查询历史记录
						{
							if( *bpPointer )
							{
								sSetfLenrnICId(2);
								OLED_ShowString(40, 6, (uint8_t*)" Clearning!", 16);//请刷卡
							}
						}
					break;
					
					case 4:
					break;
					
					case 5:
					break;
					
					default:
					break;
				}
			}
		}
		else if ( buff[1] == SET_CMD )
		{
			if(pUartRxInfo->FunctionCode == SET_CMD)//设置指令
			{
				switch(pUartRxInfo->ControlAdd)
				{
					case 1:
						if(pUartRxInfo->DataLen == 0x03 ) //密码长度为6个  占3个字节   A5 B1 01 03 11 11 11
						{
							Passwordbuff[0]=*bpPointer++;
							Passwordbuff[1]=*bpPointer++;
							Passwordbuff[2]=*bpPointer;							
							Password += (Passwordbuff[0] << 16)
										+(Passwordbuff[1] << 8)
										+(*bpPointer++);
							sSetPasswordValue(Password);
							EEPROM_WriteContinuous(0x00,3,Passwordbuff);
							//DEBUG_uart2("密码设置成功");
						}
						else
						{
							//DEBUG_uart2("设置密码长度错误");
						}
					break;
					
					case 2:   //设置识别学号及匹配ID
						if( (pUartRxInfo->DataLen >= 0x06) && (pUartRxInfo->DataLen<=14) ) //  A5 09 02 08 99 88 77 66 61 CF 05 4B
						{
							memset(StudentNumBuff,0,9);   //将数组清空
							for(i = 0;i<9;i++)//名字长度最大值为9个
							{
								if(*bpPointer != NULL)//逗号（0x2c）结束  0(NULL)
								{
									StudentNumBuff[i] = *bpPointer++;
								}
								else
								{
									bpPointer++; //跳过逗号符
									break;
								}
							}
							if(i>9)
							{
								DEBUG_uart2("输入数据错误！请重新输入");
								break;
							}
							memcpy(&strogeBuff[11],StudentNumBuff,9);
							
							IC_idNumBuff[0] = *bpPointer++;
							IC_idNumBuff[1] = *bpPointer++;
							IC_idNumBuff[2] = *bpPointer++;
							IC_idNumBuff[3] = *bpPointer;
							
							memcpy(&strogeBuff[6],IC_idNumBuff,4);
							strogeBuff[10] = 1;

							for(i=0;i<5;i++ )
							{
								if (stInformation[i].flag != 1 )
								{
									break;
								}
							}
							if(i<5)
							{
								EEPROM_WriteContinuous(i*0x20+0x20,16,&strogeBuff[0]);
								LL_mDelay(1);
								EEPROM_WriteContinuous(i*0x20+0x20+0x10,16,&strogeBuff[16]);
								memcpy(&stInformation[i].year_h,strogeBuff,20);
								DEBUG_uart2("IC卡录入成功");
							}
							else
							{
								DEBUG_uart2("位置已满");
							}
						}
						else
						{
							DEBUG_uart2("设置长度错误");
						}
					break;
					
					case 3:
						if( pUartRxInfo->DataLen == 0x04)    // A5 09 03 04 F3 C8 2E AC
						{
							IC_idNumBuff[0] = *bpPointer++;
							IC_idNumBuff[1] = *bpPointer++;
							IC_idNumBuff[2] = *bpPointer++;
							IC_idNumBuff[3] = *bpPointer;
							
							for(i=0; i<5;i++)
							{
								if( (stInformation[i].flag == 1)
									&&(stInformation[i].IC_id[0] == IC_idNumBuff[0]) 
									&& (stInformation[i].IC_id[1] == IC_idNumBuff[1])
									&& (stInformation[i].IC_id[2] == IC_idNumBuff[2])
									&& (stInformation[i].IC_id[3] == IC_idNumBuff[3]))
								{
									stInformation[i].flag = 0;
									//EEPROM_WriteContinuous(i*0x20+0x20+10,0,&stInformation[i].flag);
									EEPROM_WriteOneByte(i*0x20+0x20+10,0);
									DEBUG_uart2("清除ID成功");
									break;
								}
							}
							if(i>=5)
							{
								DEBUG_uart2("找不到对应ID，清除失败");
							}
						}
						
					break;
					
					case 4://清除历史记录
						if( pUartRxInfo->DataLen == 0x01)    // A5 B1 03 04 F3 C8 2E AC
						{
							if (*bpPointer == 1 )
							{
								if(!sGetClaerHistoryRecordFlag())
								{
									sSetClaerHistoryRecordFlag(1);
									//DEBUG_uart2("清除历史记录");
								}
							}
						}
						
					break;
					
					case 5://设置时间
						if( pUartRxInfo->DataLen == 0x07)    // A5 B1 03 04 F3 C8 2E 
						{
							SetTime.year = buff[5];
							SetTime.month = buff[6];
							SetTime.day = buff[7];
							SetTime.date = buff[8];
							SetTime.hour = buff[9];
							SetTime.minute = buff[10];
							SetTime.second = buff[11];
//							if( (SetTime.year <= 0x99) && (SetTime.month<=12)
//								&& (SetTime.day <= 0x31) && (SetTime.date<=7)
//								&& (SetTime.hour <= 0x24) && (SetTime.minute<=60)
//								&& (SetTime.second <= 0x60) )
							{
								sSetbSetRtcTimeFlag(1);
								//DEBUG_uart2("数据有效，设置中...");
							}
//							else
//							{
//								//DEBUG_uart2("设置时间数据错误");
//							}
							
						}
						
					break;
						case 6://设置蓝牙
							if( pUartRxInfo->DataLen <= 0x04)    // A5 B1 03 04 F3 C8 2E 
							{
								BLE_AT_ENABLE();//打开
//								sprintf(OutBuff,"AT+NAME=DoorBle_");//将时间数据转换出来
								for ( i = 0;i<pUartRxInfo->DataLen; i++ )
								{
									strBuff[i]=*bpPointer++;
								}
//								strncat(OutBuff,strBuff,pUartRxInfo->DataLen);
								DEBUG_uart2("AT+NAME=DoorBle_%s\r\n",strBuff);
								BLE_AT_DISENABLE();
							}
						
					break;
						case 7://恢复出厂设置
						if( pUartRxInfo->DataLen == 0x01)    // A5 B1 03 04 F3 C8 2E AC
						{
							if (*bpPointer == 1 )
							{
								sSetClaerHistoryRecordFlag(1);//清空历史记录
								sSetPasswordValue(0x123456);//恢复默认密码
								Passwordbuff[0]=0x12;
								Passwordbuff[1]=0x34;
								Passwordbuff[2]=0x56;		
								EEPROM_WriteContinuous(0x00,3,Passwordbuff);
								for(i=0; i<5;i++)
								{
									if( stInformation[i].flag == 1)
									{
										stInformation[i].flag = 0;
										EEPROM_WriteOneByte(i*0x20+0x20+10,0);
										//DEBUG_uart2("清除ID成功");
									}
								}
							}
						}
						
					break;
					
					default:
					break;
				}
			}
		}
		if ( (Error_code != 0) || (f_Operation_ok) )
		{
			f_Operation_ok = 0;
			tx_buff[tx_len++] = Error_code;
			tx_buff[4] = tx_len - 5;
			
		}
		else if( (pUartRxInfo->FunctionCode == SET_CMD) || (pUartRxInfo->FunctionCode == CONTROL_CMD) )
		{
			//tx_buff[tx_data_len++] = 1;
			tx_buff[tx_len++] = 0x01;
			tx_buff[4] = tx_len - 5;
		}
			
		CrcValue = Crc16(tx_buff,tx_len);
		//bAdd8_value = Add8(tx_buff, tx_data_len); 	
		tx_buff[tx_len++] = (CrcValue>>8)&0xff;
		tx_buff[tx_len++] = CrcValue & 0xff;
		
		sDal_Usart2SendData(tx_buff, tx_len);
		
	}
}

void sRf522DataAnalysis(void)
{
	uint8_t IDbuff[4];
	uint8_t i = 0;
	char IdNumbuff[9]={1,2,3,4,5,6,7,2,'\0'};
	static uint8_t ReadFlag = 0;
	static uint8_t countTime = 0;  //显示清空计时
	uint8_t StorageBuff[15];
	uint16_t StorageAddr;
	_INFORMATION p_Information;
	static uint8_t formerID_num = IC_MAX_NUM+1;  
	static uint8_t formerID_TimeOut = 0;  //显示清空计时
	uint8_t strogeBuff[32];

	if(formerID_TimeOut > 0)
	{
		formerID_TimeOut--;
	}
	if(GetRC522ID(IDbuff) && ((sGetSysMode() == SYS_MODE_NFC) || (sGetfLenrnICId() != 0)))
	{
		ReadFlag = 1;
		for(i=0;i<IC_MAX_NUM;i++)  //轮询对比 存储中是否有匹配的ID卡
		{
			if( (stInformation[i].flag == 1)   
				&& (stInformation[i].IC_id[0] == IDbuff[0]) && (stInformation[i].IC_id[1] == IDbuff[1]) 
				&& (stInformation[i].IC_id[2] == IDbuff[2])	&& (stInformation[i].IC_id[3] == IDbuff[3]))
			{
				break;
			}
		}
		if(sGetfLenrnICId() == 1 )
		{
			sSetfLenrnICId(0);
			OLED_ShowString(40, 6, (uint8_t*)"           ", 16);
			if( i < IC_MAX_NUM )
			{
				DEBUG_uart2("该卡已经存在，请选择其他卡！");
				OLED_ShowString(40, 6, (uint8_t*)" LEARN NO!", 16);
			}
			else 
			{
				for(i=0;i<5;i++ )
				{
					if (stInformation[i].flag != 1 )
					{
						break;
					}
				}
				if(i<5)
				{
					memcpy(&strogeBuff[6],IDbuff,4);
					strogeBuff[10] = 1;
					EEPROM_WriteContinuous(i*0x20+0x20,16,&strogeBuff[0]);
					LL_mDelay(1);
					EEPROM_WriteContinuous(i*0x20+0x20+0x10,16,&strogeBuff[16]);
					memcpy(&stInformation[i].year_h,strogeBuff,20);
					DEBUG_uart2("IC卡录入成功");
					OLED_ShowString(40, 6, (uint8_t*)" LEARN OK!", 16);
				}
				else
				{
					DEBUG_uart2("位置已满");
					OLED_ShowString(40, 6, (uint8_t*)" LEARN NO!", 16);
				}
			}
		}
		else if (sGetfLenrnICId() == 2)
		{
			sSetfLenrnICId(0);
			OLED_ShowString(40, 6, (uint8_t*)"           ", 16);
			if( i < IC_MAX_NUM )
			{
				DEBUG_uart2("清除ID成功");
				stInformation[i].flag = 0;
				EEPROM_WriteOneByte(i*0x20+0x20+10,0);
				OLED_ShowString(40, 6, (uint8_t*)" CLEAR OK!", 16);//清除成功
				
			}
			else
			{
				//卡不存在
				DEBUG_uart2("卡不存在");
				OLED_ShowString(40, 6, (uint8_t*)" CLEAR NO!", 16);
			}
		}
		else
		{
			if( (i < IC_MAX_NUM) && ((formerID_num != i) || (formerID_TimeOut==0) ))
			{
				formerID_num = i;  //保留当前卡的ID
				formerID_TimeOut = 4;   //保证相同卡间隔2秒后才能刷卡
				memcpy(&p_Information.year_h,&stInformation[i].year_h,STORGE_LEN );
				
				p_Information.year_h = 0x20;  //取高位
				p_Information.year_l = ReadTime.year;  //取高位
				p_Information.month = ReadTime.month;  //取高位
				p_Information.day = ReadTime.day;
				p_Information.hour = ReadTime.hour;
				p_Information.minute = ReadTime.minute;
				p_Information.flag = 0xAA;  //存储有效标志位
				//memcpy(&stInformation[i].IC_id,IDbuff,4);

				StorageAddr = sGetHistoryRecordAddr();   //读取当前的存储地址
				EEPROM_WriteContinuous(StorageAddr,16,&p_Information.year_h);//存储刷卡记录
				LL_mDelay(1);
				EEPROM_WriteContinuous(StorageAddr+0x10,4,&p_Information.NumID[5]);//存储刷卡记录
				StorageAddr+=0x20;
				if(StorageAddr>=0x200)
				{
					StorageAddr = 0x100;
				}
				StorageBuff[0] = (StorageAddr & 0xff00 )>> 8;
				StorageBuff[1] = StorageAddr & 0xff;
				sSetHistoryRecordAddr(StorageAddr);
				LL_mDelay(1);
				EEPROM_WriteContinuous(STROGE_HISTORY_EEPROM_ADDDR,STROGE_HISTORY_EEPROM_LEN,StorageBuff);//存储下一条需要存储的eeprom位置
				
				sSetDoorStatus(1);//开门
				sSetBuzzerStatus(1); OneNet_ReportAuth(2, 1);
				countTime = 0;  //关门计数器重新计数
				ArrayToStr(IDbuff,4,IdNumbuff);
				OLED_ShowString(48, 6, (uint8_t*)IdNumbuff, 16);
				DEBUG_uart2("ID:%s",IdNumbuff);
//				ReadFlag = 1;
			}
			else
			{
				if( i>= IC_MAX_NUM)
				{
					sSetBuzzerStatus(2); OneNet_ReportAuth(2, 0);
				}
				ArrayToStr(IDbuff,4,IdNumbuff);
				OLED_ShowString(48, 6, (uint8_t*)IdNumbuff, 16);
				DEBUG_uart2("同一张卡需要间隔2秒以上才能开启");
				//OLED_ShowString(40, 6, (uint8_t*)"   ON!   ", 16);
			}
		}
	}
	if(ReadFlag == 1)
	{
		countTime++;
		if(countTime>=7)
		{
			countTime =0;
			ReadFlag = 0;
			OLED_ShowString(40, 6, (uint8_t*)"           ", 16);
		}
	}
		
}

int ArrayToStr(unsigned char *Buff, unsigned int BuffLen, char *OutputStr)
{
	int i = 0;
	char TempBuff[128] = {0};
	char strBuff[256] = {0};
	
	for(i = 0; i<BuffLen;i++)
	{
		sprintf(TempBuff,"%02x",Buff[i]);//以十六进制格式输出到TempBuff，宽度为2
		strncat(strBuff,TempBuff,BuffLen*2);//将TempBuff追加到strBuff结尾
	}
	memcpy(OutputStr, strBuff, BuffLen*2); //将strBuff复制到OutputStr
	
	return BuffLen*2;
}
