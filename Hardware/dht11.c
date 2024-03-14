#include "dht11.h"

//延时nus个微秒
void delay_us(int nus)
{
	int tmp;
	
	SysTick->LOAD = nus * 21;//延时nms所需要的的计数
	
	SysTick->VAL = 0;//计数器清零
	
	SysTick->CTRL |= 1;//开始计数---从这个时候VAL才会从LOAD上获取数值，并且接收一次脉冲就减1
	
	//在计数器递减到0之前，代码都在此处停留
	do{
		tmp = SysTick->CTRL;
	}while(  !(tmp&(1<<16))  &&  (tmp&0x01) );
	// &运算是只要有一个为0，结果就是0
	// 当tmp的16位为0的时候，也就是计数还没有到达，需要继续循环，所以数值0取反，继续运行循环
	// 当tmp的16位为1的时候，也就是计数到达了，不需要再循环，但此时结果非0，同样要取反，结束循环
	// tmp&0x01 保证CTRL的第0位始终为1，因为中断会把这一位强制置为0，防止中断造成影响
	
	SysTick->CTRL &= ~1;//停止计数
	
	SysTick->VAL = 0;//计数器清零
}

/*
 * 函数名：DHT11_GPIO_Config
 * 描述  ：配置DHT11用到的I/O口
 * 输入  ：无
 * 输出  ：无
 */
void DHT11_GPIO_Config(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启DHT11_PORT的外设时钟*/
	RCC_AHB1PeriphClockCmd(DHT11_CLK, ENABLE); 

	/*选择要控制的DHT11_PORT引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  
  /*设置引脚的输出类型为推挽输出*/
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
  /*设置引脚为上拉模式*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
	/*设置引脚速率为50MHz */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化DHT11_PORT*/
  GPIO_Init(DHT11_PORT, &GPIO_InitStructure); 
}

/*
 * 函数名：DHT11_Mode_IPU
 * 描述  ：使DHT11-DATA引脚变为上拉输入模式
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*选择要控制的DHT11_PORT引脚*/	
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;

	/*设置引脚模式为浮空输入模式*/ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化DHT11_PORT*/
	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);	 
}

/*
 * 函数名：DHT11_Mode_Out_PP
 * 描述  ：使DHT11-DATA引脚变为推挽输出模式
 * 输入  ：无
 * 输出  ：无
 */
static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	/*选择要控制的DHT11_PORT引脚*/															   
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  
	/*设置引脚的输出类型为推挽输出*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
	/*设置引脚为上拉模式*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化DHT11_PORT*/
  	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);	 	 
}

/* 
 * 从DHT11读取一个字节，MSB先行
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(DHT11_DATA_IN()==Bit_RESET);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		delay_us(40); //延时x us 这个延时需要大于数据0持续的时间即可	   
		//Delay_us(4);

		if(DHT11_DATA_IN()==Bit_SET)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(DHT11_DATA_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	return temp;
}
/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	uint16_t count;
	
	/*输出模式*/
	DHT11_Mode_Out_PP();
	
	/*主机拉低*/
	DHT11_DATA_OUT(DHT11_LOW);
	
	/*延时18ms*/
	delay_us(20000);

	/*总线拉高 主机延时30us*/
	DHT11_DATA_OUT(DHT11_HIGH); 

	delay_us(30);   //延时30us
	//Delay_us(3);

	/*主机设为输入 判断从机响应信号*/ 
	DHT11_Mode_IPU();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(DHT11_DATA_IN()==Bit_RESET)     
	{
		count=0;
		/*轮询直到从机发出 的80us 低电平 响应信号结束*/  
		while(DHT11_DATA_IN()==Bit_RESET)
		{
		  count++;
		  if(count>1000)  return 0;
		  delay_us(10); 
				//Delay_us(1);
		}    
		
		/*开始接收数据*/   
		DHT11_Data->humi_int= Read_Byte();

		DHT11_Data->humi_deci= Read_Byte();

		DHT11_Data->temp_int= Read_Byte();

		DHT11_Data->temp_deci= Read_Byte();

		DHT11_Data->check_sum= Read_Byte();

		/*读取结束，引脚改为输出模式*/
		DHT11_Mode_Out_PP();
		/*主机拉高*/
		DHT11_DATA_OUT(DHT11_HIGH);

		/*检查读取的数据是否正确*/
		if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
			return 1;
		else 
			return 0;
	}
	else
	{		
		return 0;
	}   
}

//定义变量
DHT11_Data_TypeDef DHT11_Data;


void Get_DHT11_Data(void)
{
	/* 判断是否成功读取到温湿度数据 */
	if (Read_DHT11(&DHT11_Data) == SUCCESS)
	{
		printf("读取DHT11成功，湿度为%d.%d ％RH，温度为 %d.%d℃。\r\n\r\n",
			   DHT11_Data.humi_int,
			   DHT11_Data.humi_deci,
			   DHT11_Data.temp_int,
			   DHT11_Data.temp_deci);
	}
	else
	{
		printf("Read DHT11 ERROR!\r\n");
	}

	if( DHT11_Data.temp_int > 25 )
	{
		//亮起灯作警报处理
		
	}
	
	/* 注意： DHT11一次通讯时间最大3ms，主机连续采样间隔建议不小于100ms。 */
	delay_us(1000);
}
