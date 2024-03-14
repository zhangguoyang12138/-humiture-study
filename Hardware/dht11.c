#include "dht11.h"

//��ʱnus��΢��
void delay_us(int nus)
{
	int tmp;
	
	SysTick->LOAD = nus * 21;//��ʱnms����Ҫ�ĵļ���
	
	SysTick->VAL = 0;//����������
	
	SysTick->CTRL |= 1;//��ʼ����---�����ʱ��VAL�Ż��LOAD�ϻ�ȡ��ֵ�����ҽ���һ������ͼ�1
	
	//�ڼ������ݼ���0֮ǰ�����붼�ڴ˴�ͣ��
	do{
		tmp = SysTick->CTRL;
	}while(  !(tmp&(1<<16))  &&  (tmp&0x01) );
	// &������ֻҪ��һ��Ϊ0���������0
	// ��tmp��16λΪ0��ʱ��Ҳ���Ǽ�����û�е����Ҫ����ѭ����������ֵ0ȡ������������ѭ��
	// ��tmp��16λΪ1��ʱ��Ҳ���Ǽ��������ˣ�����Ҫ��ѭ��������ʱ�����0��ͬ��Ҫȡ��������ѭ��
	// tmp&0x01 ��֤CTRL�ĵ�0λʼ��Ϊ1����Ϊ�жϻ����һλǿ����Ϊ0����ֹ�ж����Ӱ��
	
	SysTick->CTRL &= ~1;//ֹͣ����
	
	SysTick->VAL = 0;//����������
}

/*
 * ��������DHT11_GPIO_Config
 * ����  ������DHT11�õ���I/O��
 * ����  ����
 * ���  ����
 */
void DHT11_GPIO_Config(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����DHT11_PORT������ʱ��*/
	RCC_AHB1PeriphClockCmd(DHT11_CLK, ENABLE); 

	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/															   
  	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  
  /*�������ŵ��������Ϊ�������*/
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
  /*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
	/*������������Ϊ50MHz */   
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��DHT11_PORT*/
  GPIO_Init(DHT11_PORT, &GPIO_InitStructure); 
}

/*
 * ��������DHT11_Mode_IPU
 * ����  ��ʹDHT11-DATA���ű�Ϊ��������ģʽ
 * ����  ����
 * ���  ����
 */
static void DHT11_Mode_IPU(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/	
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;

	/*��������ģʽΪ��������ģʽ*/ 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��DHT11_PORT*/
	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);	 
}

/*
 * ��������DHT11_Mode_Out_PP
 * ����  ��ʹDHT11-DATA���ű�Ϊ�������ģʽ
 * ����  ����
 * ���  ����
 */
static void DHT11_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	/*ѡ��Ҫ���Ƶ�DHT11_PORT����*/															   
	GPIO_InitStructure.GPIO_Pin = DHT11_PIN;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
  
	/*�������ŵ��������Ϊ�������*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  
	/*��������Ϊ����ģʽ*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  
	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��DHT11_PORT*/
  	GPIO_Init(DHT11_PORT, &GPIO_InitStructure);	 	 
}

/* 
 * ��DHT11��ȡһ���ֽڣ�MSB����
 */
static uint8_t Read_Byte(void)
{
	uint8_t i, temp=0;

	for(i=0;i<8;i++)    
	{	 
		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
		while(DHT11_DATA_IN()==Bit_RESET);

		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		delay_us(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   
		//Delay_us(4);

		if(DHT11_DATA_IN()==Bit_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
		{
			/* �ȴ�����1�ĸߵ�ƽ���� */
			while(DHT11_DATA_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
		}
	}
	return temp;
}
/*
 * һ�����������ݴ���Ϊ40bit����λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 */
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
	uint16_t count;
	
	/*���ģʽ*/
	DHT11_Mode_Out_PP();
	
	/*��������*/
	DHT11_DATA_OUT(DHT11_LOW);
	
	/*��ʱ18ms*/
	delay_us(20000);

	/*�������� ������ʱ30us*/
	DHT11_DATA_OUT(DHT11_HIGH); 

	delay_us(30);   //��ʱ30us
	//Delay_us(3);

	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
	DHT11_Mode_IPU();

	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
	if(DHT11_DATA_IN()==Bit_RESET)     
	{
		count=0;
		/*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
		while(DHT11_DATA_IN()==Bit_RESET)
		{
		  count++;
		  if(count>1000)  return 0;
		  delay_us(10); 
				//Delay_us(1);
		}    
		
		/*��ʼ��������*/   
		DHT11_Data->humi_int= Read_Byte();

		DHT11_Data->humi_deci= Read_Byte();

		DHT11_Data->temp_int= Read_Byte();

		DHT11_Data->temp_deci= Read_Byte();

		DHT11_Data->check_sum= Read_Byte();

		/*��ȡ���������Ÿ�Ϊ���ģʽ*/
		DHT11_Mode_Out_PP();
		/*��������*/
		DHT11_DATA_OUT(DHT11_HIGH);

		/*����ȡ�������Ƿ���ȷ*/
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

//�������
DHT11_Data_TypeDef DHT11_Data;


void Get_DHT11_Data(void)
{
	/* �ж��Ƿ�ɹ���ȡ����ʪ������ */
	if (Read_DHT11(&DHT11_Data) == SUCCESS)
	{
		printf("��ȡDHT11�ɹ���ʪ��Ϊ%d.%d ��RH���¶�Ϊ %d.%d�档\r\n\r\n",
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
		//���������������
		
	}
	
	/* ע�⣺ DHT11һ��ͨѶʱ�����3ms��������������������鲻С��100ms�� */
	delay_us(1000);
}
