#include "SysDoorStatus.h"
#include "onenet_app.h"


uint8_t DoorStatus= 0;


uint8_t sGetDoorStatus(void)
{
	return DoorStatus;
}

void sSetDoorStatus(uint8_t status)
{
	DoorStatus = status ;
}



void sSysDoorOpenTimeOut(void)  //100MSһ��
{
	static uint8_t status = 0;
	static uint8_t CountTime = 0;
	
	if(CountTime>0)
	{
		CountTime--;
	}
	if(DoorStatus == 1)
	{
		DoorStatus = 2;
		CountTime = 50;
		if(status == 0)
		{
			status =1;
			sOpenDoor(); OneNet_ReportLock(0);
			OLED_ShowCHinese(80,4,9);  //��
			OLED_ShowCHinese(96,4,10); //��
		}
	}
	if(((CountTime == 0)&&(status == 1)) || (DoorStatus == 0 ))
	{
		if(status == 1)
		{
			OneNet_ReportLock(1);
		}
		CountTime = 0;
		status = 0;
		OLED_ShowCHinese(80,4,11); //��
		OLED_ShowCHinese(96,4,12); //��
		sCloseDoor();
	}
	
	
	
	
	
	
}

