#include "stm32f10x.h"                  // Device header
#include "EPD.h"
#include "Delay.h"

uint8_t In[]={0x00};
int main(void)
{
	EPD_Init();
	
	EPD_Clear();
	uint16_t Time=0;
	EPD_ShowString(0,0,"LZ1104",EPD_8X16);
	EPD_Update();

	while(1)
	{
		EPD_ShowNum(0,32,Time,5,EPD_8X16);
		Time++;
		Delay_ms(2000);
		EPD_Update();
	}
}
