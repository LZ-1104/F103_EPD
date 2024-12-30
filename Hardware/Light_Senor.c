#include "stm32f10x.h"                  // Device header

void LightSenor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode=GPIO_Mode_IN_FLOATING ;
	GPIO_Initstructure.GPIO_Pin=GPIO_Pin_0 ;
	GPIO_Initstructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
}

uint8_t LightSenor_Get()
{
	return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
}
