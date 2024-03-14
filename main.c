//#include <stdio.h>
#include "stm32f4xx.h" 	//����ST�ٷ�ͷ�ļ�

#define LED1_PIN   GPIO_Pin_14
#define LED2_PIN   GPIO_Pin_13
#define LED3_PIN   GPIO_Pin_6
#define LED4_PIN   GPIO_Pin_11


//GPIO: ͨ�������������
//�������
//�ṹ������      ������
//int             a;
GPIO_InitTypeDef  GPIO_InitStructure;

//��1��ȷ���������ĸ��˿ںţ��磺PG14
//��2���޸Ĵ��룬һ����Ҫ�Ķ���         ʱ�����á��������á����ſ��ơ�


/*
PG14--->LED0
PG13--->LED1
PG6--->LED2
PG11--->LED3
*/

//��ʱ���� (��CPUֻ���м����������������κι�������ռ��CPUֻ��������ֵ���ۼ�)
void delay(unsigned int time)
{
	int i, j;
	for(i=0; i<time; i++)
	{
		for(j=0; j<0x1FFF; j++);
	}
}

//C������ģ�黯���ԣ�ÿ��ģ��֮��ͨ�����ݣ�������ָ�룩��������

//ʵ����ˮ�ƣ���ˮ����֮����������ȫ������������ѭ����

int main(void)
{
	
/*  ����GPIO�����Ĵ��룬���ú���Ҫʹ�õ����ţ�ʹ�ú�������ֱ��ʹ�� */
	/* GPIOG Peripheral clock enable �����˿�ʱ�ӣ����Կ��������ܹ���������������ߵ͵�ƽ�� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	/* Configure PG6 and PG8 in output pushpull mode �������ŵĹ���ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;	//��������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
/* �������ŵ�Ĭ��״̬Ϊ���ߵ�ƽ����ƣ�*/
	//GPIOG->BSRRL = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
				//�˿�   ���ź�
	GPIO_SetBits(GPIOG, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);	//����ͬ�ϣ���������Ϊ�ߵ�ƽ
	
//void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	
	//Ƕ��ʽ��Ʒ�ǲ����˳���������������˳��ˣ��ᱻ����ִ�У���֤����
	while(1)
	{
		/* Set LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN ������������ߵ�ƽ */
		//GPIOG->BSRRL = LED1_PIN | LED2_PIN;
		
		//�������͵�ƽ��
		/* Reset LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN������������͵�ƽ*/
		//GPIOG->BSRRH = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
		GPIO_ResetBits(GPIOG, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
		
		
		//��ʱ�������ڵ�Ƭ���������У����޷�ֱ��ʹ��Linuxϵͳ�ṩ��sleep()�������ú�������ST�ṩ���ļ������еĺ����� ��
		delay(1000);	//�����ʱ1s
		
		//���𣨸ߵ�ƽ��
		//GPIOG->BSRRL = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
		GPIO_SetBits(GPIOG, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
		
		//��ʱ
		delay(1000);	//�����ʱ1s
	}
}
	//KEIL���ͨ���ļ������һ��Ϊ �µ�һ�� ��ʶ���ļ��ѱ�д��ϣ����ѽ�����
