#include "stm32f4xx.h"                  // Device header


void LED_Init(void)
{
	  GPIO_InitTypeDef  GPIO_InitStructure;
	
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_6 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOG, &GPIO_InitStructure);
			
}
void LED1_ON(void)
{
		GPIO_ResetBits(GPIOG, GPIO_Pin_14);

}
void LED1_OFF(void)
{
		GPIO_SetBits(GPIOG, GPIO_Pin_14);

}
void LED1_Turn(void)
{
		if(GPIO_ReadOutputDataBit(GPIOG, GPIO_Pin_14) == 0)
		{
					GPIO_SetBits(GPIOG, GPIO_Pin_14);
		}
		else
		{
					GPIO_ResetBits(GPIOG, GPIO_Pin_14);
		}

}
void LED2_ON(void)
{
		GPIO_ResetBits(GPIOG, GPIO_Pin_13);

}
void LED2_OFF(void)
{
		GPIO_SetBits(GPIOG, GPIO_Pin_13);

}
void LED2_Turn(void)
{
		if(GPIO_ReadOutputDataBit(GPIOG, GPIO_Pin_13) == 0)
		{
					GPIO_SetBits(GPIOG, GPIO_Pin_13);
		}
		else
		{
					GPIO_ResetBits(GPIOG, GPIO_Pin_13);
		}

}
void LED3_ON(void)
{
		GPIO_ResetBits(GPIOG, GPIO_Pin_6);

}
void LED3_OFF(void)
{
		GPIO_SetBits(GPIOG, GPIO_Pin_6);

}
void LED3_Turn(void)
{
		if(GPIO_ReadOutputDataBit(GPIOG, GPIO_Pin_6) == 0)
		{
					GPIO_SetBits(GPIOG, GPIO_Pin_6);
		}
		else
		{
					GPIO_ResetBits(GPIOG, GPIO_Pin_6);
		}

}
void LED4_ON(void)
{
		GPIO_ResetBits(GPIOG, GPIO_Pin_11);

}
void LED4_OFF(void)
{
		GPIO_SetBits(GPIOG, GPIO_Pin_11);

}
void LED4_Turn(void)
{
		if(GPIO_ReadOutputDataBit(GPIOG, GPIO_Pin_11) == 0)
		{
					GPIO_SetBits(GPIOG, GPIO_Pin_11);
		}
		else
		{
					GPIO_ResetBits(GPIOG, GPIO_Pin_11);
		}

}
void LED_ALLDOWN()
{
		GPIO_SetBits(GPIOG, GPIO_Pin_14);
		GPIO_SetBits(GPIOG, GPIO_Pin_13);
		GPIO_SetBits(GPIOG, GPIO_Pin_6);
		GPIO_SetBits(GPIOG, GPIO_Pin_11);
}
