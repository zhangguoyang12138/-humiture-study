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
 * @brief	�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf������ӡ���ݵ��������
 * @param	ch��ָ�����͵ĵ����ֽ�����
 *			fp��FILE ָ��(�ļ�ָ�룩��ָ���׼�����������ָ��
 * @retval	�������������
 */
int fputc(int ch, FILE *fp)
{
	/* ���͵����ֽ����ݵ����� */
	USART_SendData(USART1, (uint8_t)ch);

	/* �ȴ�������� */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		;

	/* ������������� */
	return ch;
}

/**
 * @brief	���͵����ֽ����ݺ���
 * @param	pUSARTx��ͨ�ŵĴ��ڶ˿ڣ�x������1��2��3��4��5��6��7��8��
 *			ch��ָ�����͵ĵ����ֽ�����
 * @retval	��
 */
void Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
	/* ���͵����ֽ����ݵ����� */
	USART_SendData(pUSARTx, ch);

	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET)
		;
}

/**
 * @brief	�����ַ�������
 * @param	pUSARTx��ͨ�ŵĴ��ڶ˿ڣ�x������1��2��3��4��5��6��7��8��
 *			str��ָ�����͵��ַ�������
 * @retval	��
 */
void Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
	unsigned int k = 0;
	do
	{
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	} while (*(str + k) != '\0');

	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
		;
}


/**
 * @brief	���ڽӿڳ�ʼ������
 * @param	BaudRate�������ʣ���ͨ�����ʡ�
 * @retval	��
 */
void UART1_Config(uint32_t BaudRate)
{
	/* ����������͵Ľṹ�� */
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* �����˿�����ʱ�� */ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* �������ڶ˿�ʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	/* �������ŵĸ��ù��� */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	/* ����ͨ�����Ź�����ʽ */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;		  // UART1_TXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;	  // ���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	  // ����ģʽ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	  // ����ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; // ����ٶ�Ϊ��
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // UART1_RXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* ���ô��ڽӿڵĹ�����ʽ */
	USART_InitStruct.USART_BaudRate = BaudRate;									 // ������
	USART_InitStruct.USART_Parity = USART_Parity_No;							 // �޼���
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;					 // 8λ�ֳ�
	USART_InitStruct.USART_StopBits = USART_StopBits_1;							 // 1λֹͣλ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				 // �շ�ģʽ
	USART_Init(USART1, &USART_InitStruct);

	/* �������ڽ����ж� */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* ���ô��ڵ��ж����ȼ� */
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

	/* �������ڹ��� */
	USART_Cmd(USART1, ENABLE);
}

//����2��ͨ������Ϊ�� PA2-TXD   ,  PA3-RXD
void UART2_Config(uint32_t BaudRate)
{
	/* ����������͵Ľṹ�� */
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	/* �����˿�����ʱ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	/* �������ڶ˿�ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* �������ŵĸ��ù��� */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/* ����ͨ�����Ź�����ʽ */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;		  // UART1_TXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;	  // ���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;	  // ����ģʽ
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	  // ����ģʽ
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed; // ����ٶ�Ϊ��
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; // UART1_RXD
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* ���ô��ڽӿڵĹ�����ʽ */
	USART_InitStruct.USART_BaudRate = BaudRate;									 // ������
	USART_InitStruct.USART_Parity = USART_Parity_No;							 // �޼���
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;					 // 8λ�ֳ�
	USART_InitStruct.USART_StopBits = USART_StopBits_1;							 // 1λֹͣλ
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				 // �շ�ģʽ
	USART_Init(USART2, &USART_InitStruct);

	/* �������ڽ����ж� */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	/* ���ô��ڵ��ж����ȼ� */
	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);

	/* �������ڹ��� */
	USART_Cmd(USART2, ENABLE);
}

/* ���崮����ر������������ڽ��պͷ������� */
char rxd_data;	   // �洢���յ��ĵ����ֽ�����
char rxd_num;	   // �洢���յ��ĵ����ֽ����ݵ�����
char rxd_str[512]; // �洢�ַ�������

int password_flag = 1;

/**
 * @brief  �����жϷ�����
 * @note   �������ɹٷ��̶����ø��ģ��������õ��ã����ô��Σ����÷�������
 * @param  ��
 * @retval ��
 */
void USART1_IRQHandler(void)
{
	/* ��⴮���Ƿ���յ����� */
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		/* ��������жϱ�־λ */
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);

		/* ͨ������ �������� */
		rxd_data = USART_ReceiveData(USART1);	//һ�ν���8λ���ݣ��ٴ洢��rxd_data����
		
		rxd_str[rxd_num++] = rxd_data;
	}
	if(rxd_str[rxd_num-1] == '4')	//�ж������Ƿ���ս���	"LED1_ON."
	{
			//����strcmp������������������� �� ָ���ַ��� �����жϣ�����Ƚϵ�����������ȣ���������0.
			//if( rxd_str == "LED1_ON."  )
			if( 0 == strcmp(rxd_str, "1234") )	//������ͬ
			{
					access = !access;
					password_flag = 0;
			}	
			//���������ȷ�ˣ���������飬�Ա��´ν����ж�
			rxd_num = 0;
			memset(rxd_str, 0, sizeof(rxd_str));
	}
	
	//if(rxd_str[rxd_num] == '.')	//�ж������Ƿ���ս���
	if(rxd_str[rxd_num-1] == '.')	//�ж������Ƿ���ս���	"LED1_ON."
	{
		//����strcmp������������������� �� ָ���ַ��� �����жϣ�����Ƚϵ�����������ȣ���������0.
		//if( rxd_str == "LED1_ON."  )
		if( 0 == strcmp(rxd_str, "LED1_ON.") )	//������ͬ
		{
			/* LED1��ת */
			GPIO_ToggleBits(GPIOG, GPIO_Pin_14);	
		}	
			
		//���������ȷ�ˣ���������飬�Ա��´ν����ж�
		rxd_num = 0;
		memset(rxd_str, 0, sizeof(rxd_str));
	}
	
	/* �жϵ����ֽ����� */
	//if (rxd_data == 0x01 )
	if (rxd_data == 'A')
	{
		/* LED1��ת */
		GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
	}
	else if (rxd_data == 'B')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
			}
	}
	else if (rxd_data == 'C')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_6);
			}
	}
	else if (rxd_data == 'D')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_11);
			}
	}

	
	/* ���ԣ��ѽ��յ��ĵ����ֽ����ݷ��͵� ���ͷ�*/
	USART_SendData(USART1, rxd_data);
}

/* ���崮����ر������������ڽ��պͷ������� */
char rxd2_data;	   // �洢���յ��ĵ����ֽ�����
char rxd2_num=0;	   // �洢���յ��ĵ����ֽ����ݵ�����
char rxd2_str[512]; // �洢�ַ�������


/**
 * @brief  ����2�жϷ����������ڽ����¼���������
 * @note   �������ɹٷ��̶����ø��ģ��������õ��ã����ô��Σ����÷�������
 * @param  ��
 * @retval ��
 */
void USART2_IRQHandler(void)
{
	/* ��⴮���Ƿ���յ����� */
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
		/* ��������жϱ�־λ */
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);

		/* ͨ������ �������� */
		rxd2_data = USART_ReceiveData(USART2);	//һ�ν���8λ���ݣ��ٴ洢��rxd_data����
		
		rxd2_str[rxd2_num++] = rxd2_data;
	}
	
	if(rxd2_str[rxd_num-1] == '4')	//�ж������Ƿ���ս���	"LED1_ON."
	{
			//����strcmp������������������� �� ָ���ַ��� �����жϣ�����Ƚϵ�����������ȣ���������0.
			//if( rxd_str == "LED1_ON."  )
			if( 0 == strcmp(rxd2_str, "1234") )	//������ͬ
			{
					access = !access;
					password_flag = 0;
			}	
			//���������ȷ�ˣ���������飬�Ա��´ν����ж�
			rxd_num = 0;
			memset(rxd2_str, 0, sizeof(rxd2_str));
	}
	
	/* �жϵ����ֽ����� */
	
	if (rxd2_data == 'A')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_14);
			}
	}
	if (rxd2_data == 'B')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_13);
			}
	}
	if (rxd2_data == 'C')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_6);
			}
	}
	if (rxd2_data == 'D')	
	{
			if(password_flag == 0)
			{
					/* LED1��ת */
		      GPIO_ToggleBits(GPIOG, GPIO_Pin_11);
			}
	}

	/* ���ԣ��ѽ��յ��ĵ����ֽ����ݷ��͵� ���ͷ�*/
	USART_SendData(USART1, rxd2_data);	//������ͨ�ţ���ֻ��Ϊ�м�ת���ߣ��ѽ��յ������ݴ���������ʾ
}

int main(void)
{
	
		LED_Init();
	
		Key_Init();
	
		EXTILine0_Config();
	
	  LED_ALLDOWN();
	
		UART1_Config(115200);
	
		/* ����1��ʼ�����������趨Ϊ115200 */
		UART2_Config(115200);	//����ģ��ĳ���Ĭ�ϲ����ʣ�ͨ���ٶȣ�Ϊ 9600
	
		DHT11_GPIO_Config();
		
		/* ����ָ����Ϣ��PC�� */
		printf("�������óɹ���\r\n");	//����ͨ������1 ���͵�����
		
		Usart_SendString( USART2, "�������óɹ���\r\n");	//����ͨ������2 ���͵�����ģ�飬����ģ����ת�����ֻ�

		while(1){
				while(access == 1)
				{
						
						
						Get_DHT11_Data();
					  Delay_ms(500);
						//Delay_s(1);
						//Get_DHT11_Data();
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_14) == 0)
						{
								printf("1������\r\n");
						}
						else
						{
								printf("1������\r\n");
						}
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_13) == 0)
						{
								printf("2������\r\n");
						}
						else
						{
								printf("2������\r\n");
						}
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_6) == 0)
						{
								printf("3������\r\n");
						}
						else
						{
								printf("3������\r\n");
						}
						if(GPIO_ReadOutputDataBit(GPIOG ,GPIO_Pin_11) == 0)
						{
								printf("4������\r\n");
						}
						else
						{
								printf("4������\r\n");
						}
						
				}
		}

}
