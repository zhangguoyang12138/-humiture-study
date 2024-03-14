//#include <stdio.h>
#include "stm32f4xx.h" 	//包含ST官方头文件

#define LED1_PIN   GPIO_Pin_14
#define LED2_PIN   GPIO_Pin_13
#define LED3_PIN   GPIO_Pin_6
#define LED4_PIN   GPIO_Pin_11


//GPIO: 通用输入输出引脚
//定义变量
//结构体类型      变量名
//int             a;
GPIO_InitTypeDef  GPIO_InitStructure;

//（1）确定引脚是哪个端口号，如：PG14
//（2）修改代码，一般需要改动：         时钟配置、引脚配置、引脚控制。


/*
PG14--->LED0
PG13--->LED1
PG6--->LED2
PG11--->LED3
*/

//延时函数 (让CPU只进行计数处理，不作其它任何工作，即占用CPU只做变量的值的累加)
void delay(unsigned int time)
{
	int i, j;
	for(i=0; i<time; i++)
	{
		for(j=0; j<0x1FFF; j++);
	}
}

//C语言是模块化语言，每个模块之间通过数据（变量、指针）作关联。

//实现流水灯（流水所到之处灯亮，再全部灭掉，再如此循环）

int main(void)
{
	
/*  配置GPIO器件的代码，配置好需要使用的引脚，使得后续可以直接使用 */
	/* GPIOG Peripheral clock enable 开启端口时钟，用以控制引脚能够工作（运行输出高低电平） */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	/* Configure PG6 and PG8 in output pushpull mode 配置引脚的工作模式，输出还是输入模式 */
	GPIO_InitStructure.GPIO_Pin = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;	//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
/* 配置引脚的默认状态为：高电平（灭灯）*/
	//GPIOG->BSRRL = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
				//端口   引脚号
	GPIO_SetBits(GPIOG, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);	//功能同上，设置引脚为高电平
	
//void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	
	//嵌入式产品是不会退出程序，且如果程序退出了，会被重新执行（验证）。
	while(1)
	{
		/* Set LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN 设置引脚输出高电平 */
		//GPIOG->BSRRL = LED1_PIN | LED2_PIN;
		
		//灯亮（低电平）
		/* Reset LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN设置引脚输出低电平*/
		//GPIOG->BSRRH = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
		GPIO_ResetBits(GPIOG, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
		
		
		//延时函数（在单片机开发板中，是无法直接使用Linux系统提供的sleep()函数，该函数不是ST提供的文件里面有的函数） ）
		delay(1000);	//大概延时1s
		
		//灯灭（高电平）
		//GPIOG->BSRRL = LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN;
		GPIO_SetBits(GPIOG, LED1_PIN | LED2_PIN | LED3_PIN | LED4_PIN);
		
		//延时
		delay(1000);	//大概延时1s
	}
}
	//KEIL软件通过文件的最后一行为 新的一行 来识别文件已编写完毕，即已结束。
