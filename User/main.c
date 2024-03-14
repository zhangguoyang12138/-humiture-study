#include "stm32f4xx.h"                  // Device header
#include <string.h>
#include <stdio.h>
#include "Delay.h"
#include "dht11.h"
#include "LED.h"
#include "KEY.h"

uint8_t KeyNum=0;


static void EXTILine0_Config(void)
{
			EXTI_InitTypeDef   EXTI_InitStructure;
			GPIO_InitTypeDef   GPIO_InitStructure;
			NVIC_InitTypeDef   NVIC_InitStructure;

			/* Enable GPIOG clock */
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
			/* Enable SYSCFG clock */
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

			/* Configure PG2 pin as input floating */
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 |GPIO_Pin_3 |GPIO_Pin_4 |GPIO_Pin_5;
			GPIO_Init(GPIOG, &GPIO_InitStructure);
			/* Connect EXTI Line2 to PG2 pin */
			SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource2);
			SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource3);
			SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource4);
			SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource5);
			/* Configure EXTI Line2 */
			EXTI_InitStructure.EXTI_Line = EXTI_Line2;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
			
			EXTI_InitStructure.EXTI_Line = EXTI_Line3;
			EXTI_Init(&EXTI_InitStructure);
			
			EXTI_InitStructure.EXTI_Line = EXTI_Line4;
			EXTI_Init(&EXTI_InitStructure);			
			
			EXTI_InitStructure.EXTI_Line = EXTI_Line5;
			EXTI_Init(&EXTI_InitStructure);			
			/* Enable and set EXTI Line2 Interrupt to the lowest priority */
			
			NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
			
			NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
			NVIC_Init(&NVIC_InitStructure);
			
			NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
			NVIC_Init(&NVIC_InitStructure);
			
			NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	
			NVIC_Init(&NVIC_InitStructure);
}

int flag,mod=0;
int access=0;

/**
 * @brief	重定向c库函数printf到串口，重定向后可使用printf函数打印数据到串口输出
 * @param	ch：指定发送的单个字节数据
 *			fp：FILE 指针(文件指针），指向标准输出缓冲区的指针
 * @retval	反馈输出的数据
 */
int fputc(int ch, FILE *fp)
{
	/* 发送单个字节数据到串口 */
	USART_SendData(USART1, (uint8_t)ch);

	/* 等待发送完毕 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;

	/* 反馈输出的数据 */
	return ch;
}

/**
 * @brief	发送单个字节数据函数
 * @param	pUSARTx：通信的串口端口，x可以是1、2、3、4、5、6、7或8。
 *			ch：指定发送的单个字节数据
 * @retval	无
 */
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
	/* 发送单个字节数据到串口 */
	USART_SendData(pUSARTx, ch);

	/* 等待发送完成 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
		;
}

/**
 * @brief	发送字符串函数
 * @param	pUSARTx：通信的串口端口，x可以是1、2、3、4、5、6、7或8。
 *			str：指定发送的字符串数据
 * @retval	无
 */
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
	unsigned int k = 0;
	do
	{
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	} while (*(str + k) != '\0');

	/* 等待发送完成 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
		;
}


/**
 * @brief	串口接口初始化函数
 * @param	BaudRate：波特率，即通信速率。
 * @retval	无
 */
void UART1_Config(uint32_t BaudRate)
{
	/* 定义相关类型的结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* 开启端口引脚时钟 */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* 开启串口端口时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* 配置引脚的复用功能 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* 配置通信引脚工作方式 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;		  // UART1_TXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;	  // 复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	  // 推完模式
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉模式
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; // 输出速度为高
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // UART1_RXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* 配置串口接口的工作方式 */
	USART_InitStruct.USART_BaudRate = BaudRate;									 // 波特率
	USART_InitStruct.USART_Parity = USART_Parity_No;							 // 无检验
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;					 // 8位字长
	USART_InitStruct.USART_StopBits = USART_StopBits_1;							 // 1位停止位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				 // 收发模式
	USART_Init(USART1, &USART_InitStruct);

	/* 开启串口接收中断 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* 配置串口的中断优先级 */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

	/* 开启串口工作 */
	USART_Cmd(USART1, ENABLE);
}

//串口2的通信引脚为： PA2-TXD   ,  PA3-RXD
void UART2_Config(uint32_t BaudRate)
{
	/* 定义相关类型的结构体 */
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* 开启端口引脚时钟 */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* 开启串口端口时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* 配置引脚的复用功能 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/* 配置通信引脚工作方式 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;		  // UART1_TXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;	  // 复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	  // 推完模式
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	  // 上拉模式
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; // 输出速度为高
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; // UART1_RXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* 配置串口接口的工作方式 */
	USART_InitStruct.USART_BaudRate = BaudRate;									 // 波特率
	USART_InitStruct.USART_Parity = USART_Parity_No;							 // 无检验
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;					 // 8位字长
	USART_InitStruct.USART_StopBits = USART_StopBits_1;							 // 1位停止位
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				 // 收发模式
	USART_Init(USART2, &USART_InitStruct);

	/* 开启串口接收中断 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* 配置串口的中断优先级 */
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

	/* 开启串口工作 */
	USART_Cmd(USART2, ENABLE);
}

/* 定义串口相关变量和数组用于接收和发送数据 */
char rxd_data;	   // 存储接收到的单个字节数据
char rxd_num;	   // 存储接收到的单个字节数据的数量
char rxd_str[512]; // 存储字符串数据

int password_flag = 1;

/**
 * @brief  串口中断服务函数
 * @note   函数名由官方固定不得更改，函数不得调用，不得传参，不得返回数据
 * @param  无
 * @retval 无
 */
void USART1_IRQHandler(void)
{
	/* 检测串口是否接收到数据 */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		/* 清除接收中断标志位 */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		/* 通过串口 接收数据 */
		rxd_data = USART_ReceiveData(USART1);	//一次接收8位数据，再存储在rxd_data变量
		
		rxd_str[rxd_num++] = rxd_data;
	}
	if(rxd_str[rxd_num-1] == '4')	//判断数据是否接收结束	"LED1_ON."
	{
			//利用strcmp函数进行数组里的数据 与 指定字符串 进行判断，如果比较的两个数据相等，则函数返回0.
			//if( rxd_str == "LED1_ON."  )
			if( 0 == strcmp(rxd_str, "1234") )	//数据相同
			{
					access = !access;
					password_flag = 0;
			}	
			//如果接收正确了，则清空数组，以便下次进行判断
			rxd_num = 0;
			memset(rxd_str, 0, sizeof(rxd_str));
	}
	
	//if(rxd_str[rxd_num] == '.')	//判断数据是否接收结束
	if(rxd_str[rxd_num-1] == '.')	//判断数据是否接收结束	"LED1_ON."
	{
		//利用strcmp函数进行数组里的数据 与 指定字符串 进行判断，如果比较的两个数据相等，则函数返回0.
		//if( rxd_str == "LED1_ON."  )
		if( 0 == strcmp(rxd_str, "LED1_ON.") )	//数据相同
		{
			/* LED1反转 */
			GPIO_ToggleBits(GPIOG, GPIO_Pin_14);	
		}	
			
		//如果接收正确了，则清空数组，以便下次进行判断
		rxd_num = 0;
		memset(rxd_str, 0, sizeof(rxd_str));
	}
	
	/* 判断单个字节数据 */
	//if (rxd_data == 0x01 )
	if (rxd_data == 'A')
	{
		/* LED1反转 */
		GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
	}
	else if (rxd_data == 'B')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
			}
	}
	else if (rxd_data == 'C')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_6);
			}
	}
	else if (rxd_data == 'D')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_11);
			}
	}

	
	/* 回显，把接收到的单个字节数据发送到 发送方*/
	USART_SendData(USART1, rxd_data);
}

/* 定义串口相关变量和数组用于接收和发送数据 */
char rxd2_data;	   // 存储接收到的单个字节数据
char rxd2_num=0;	   // 存储接收到的单个字节数据的数量
char rxd2_str[512]; // 存储字符串数据


/**
 * @brief  串口2中断服务函数（串口紧急事件处理函数）
 * @note   函数名由官方固定不得更改，函数不得调用，不得传参，不得返回数据
 * @param  无
 * @retval 无
 */
void USART2_IRQHandler(void)
{
	/* 检测串口是否接收到数据 */
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		/* 清除接收中断标志位 */
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);

		/* 通过串口 接收数据 */
		rxd2_data = USART_ReceiveData(USART2);	//一次接收8位数据，再存储在rxd_data变量
		
		rxd2_str[rxd2_num++] = rxd2_data;
	}
	
	if(rxd2_str[rxd_num-1] == '4')	//判断数据是否接收结束	"LED1_ON."
	{
			//利用strcmp函数进行数组里的数据 与 指定字符串 进行判断，如果比较的两个数据相等，则函数返回0.
			//if( rxd_str == "LED1_ON."  )
			if( 0 == strcmp(rxd2_str, "1234") )	//数据相同
			{
					access = !access;
					password_flag = 0;
			}	
			//如果接收正确了，则清空数组，以便下次进行判断
			rxd_num = 0;
			memset(rxd2_str, 0, sizeof(rxd2_str));
	}
	
	/* 判断单个字节数据 */
	
	if (rxd2_data == 'A')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
			}
	}
	if (rxd2_data == 'B')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
			}
	}
	if (rxd2_data == 'C')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_6);
			}
	}
	if (rxd2_data == 'D')	
	{
			if(password_flag == 0)
			{
					/* LED1反转 */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_11);
			}
	}

	/* 回显，把接收到的单个字节数据发送到 发送方*/
	USART_SendData(USART1, rxd2_data);	//与蓝牙通信，它只作为中间转发者，把接收到的数据传给电脑显示
}

int main(void)
{
	
		LED_Init();
	
		Key_Init();
	
		EXTILine0_Config();
	
	  LED_ALLDOWN();
	
		UART1_Config(115200);
	
		/* 串口1初始化，波特率设定为115200 */
		UART2_Config(115200);	//蓝牙模块的出厂默认波特率（通信速度）为 9600
	
		DHT11_GPIO_Config();
		
		/* 发送指定信息到PC机 */
		printf("串口配置成功。\r\n");	//数据通过串口1 发送到电脑
		
		Usart_SendString( USART2, "蓝牙配置成功。\r\n");	//数据通过串口2 发送到蓝牙模块，蓝牙模块再转发到手机

		while(1){
				while(access == 1)
				{
						
						
						Get_DHT11_Data();
					  Delay_ms(500);
						//Delay_s(1);
						//Get_DHT11_Data();
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_14) == 0)
						{
								printf("1灯已亮\r\n");
						}
						else
						{
								printf("1灯已灭\r\n");
						}
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_13) == 0)
						{
								printf("2灯已亮\r\n");
						}
						else
						{
								printf("2灯已灭\r\n");
						}
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_6) == 0)
						{
								printf("3灯已亮\r\n");
						}
						else
						{
								printf("3灯已灭\r\n");
						}
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_11) == 0)
						{
								printf("4灯已亮\r\n");
						}
						else
						{
								printf("4灯已灭\r\n");
						}
						
				}
		}

}
