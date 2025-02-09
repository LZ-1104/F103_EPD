#include "stm32f10x.h"                  // Device header


void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_Initstructure_B;
	GPIO_Initstructure_B.GPIO_Mode=GPIO_Mode_IPU ;
	GPIO_Initstructure_B.GPIO_Pin=GPIO_Pin_8 ;
	GPIO_Initstructure_B.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstructure_B);
}

void Beep_ON(void)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_8,Bit_RESET);
}

void Beep_OFF(void)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_8,Bit_SET);
}








