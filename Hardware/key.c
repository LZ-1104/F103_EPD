#include "stm32f10x.h"                  // Device header
#include "Delay.h"



/**
  * @brief 
  * @param  
  * @retval 
  */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure_C;
	GPIO_InitStructure_C.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure_C.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure_C.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure_C);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure_D;
	GPIO_InitStructure_D.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure_D.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure_D.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure_D);






}
/**
  * @brief 
  * @param  
  * @retval 
  */
uint8_t Key0_GetNum(void)
{
	uint8_t Key_Num=0;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==0);
		Delay_ms(20);
		Key_Num=1;
			
	}
	return Key_Num;
}
/**
  * @brief 
  * @param  
  * @retval 
  */
uint8_t Key1_GetNum(void)
{
	uint8_t Key_Num=0;
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)==0);
		Delay_ms(20);
		Key_Num=1;
			
	}
	return Key_Num;
}

uint8_t Key2_GetNum(void)
{
	uint8_t Key_Num=0;
	if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)==0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2)==0);
		Delay_ms(20);
		Key_Num=1;
			
	}
	return Key_Num;
}



