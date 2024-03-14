#ifndef __DHT11_H_
#define __DHT11_H_

#include  "stm32f4xx.h"
#include <stdio.h>

#define DHT11_HIGH  1
#define DHT11_LOW   0


/*---------------------------------------*/
#define DHT11_CLK     RCC_AHB1Periph_GPIOG
#define DHT11_PIN     GPIO_Pin_9             
#define DHT11_PORT    GPIOG


#define DHT11_DATA_OUT(a)	if (a)	\
                                   GPIO_SetBits(DHT11_PORT,DHT11_PIN);\
                                   else		\
                                   GPIO_ResetBits(DHT11_PORT,DHT11_PIN)

#define  DHT11_DATA_IN()	  GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN)

typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
}DHT11_Data_TypeDef;

void DHT11_GPIO_Config(void);
extern DHT11_Data_TypeDef DHT11_Data; // 温湿度结构体
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data);
void Get_DHT11_Data(void);

#endif //__DHT11_H_
