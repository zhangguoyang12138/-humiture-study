#include "stm32f4xx.h"                  // Device header
#include "Delay.h"

void Key_Init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;

		GPIO_Init(GPIOG, &GPIO_InitStructure);
			
}
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	
	if (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_2) == 0)
	{
			Delay_ms(20);
		  while (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_2) == 0)
			Delay_ms(20);
			KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_3) == 0)
	{
			Delay_ms(20);
		  while (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_3) == 0)
			Delay_ms(20);
			KeyNum = 2;
	}
	if (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_4) == 0)
	{
			Delay_ms(20);
		  while (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_4) == 0)
			Delay_ms(20);
			KeyNum = 3;
	}
	if (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_5) == 0)
	{
			Delay_ms(20);
		  while (GPIO_ReadInputDataBit(GPIOG ,GPIO_Pin_5) == 0)
			Delay_ms(20);
			KeyNum = 4;
	}

	return KeyNum;
}
