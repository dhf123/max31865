#include "adc.h"
#include "usart.h"
#include "stdio.h"


#define ADC1_DR_Address     0x40012440	//ADC1��ַ
#define AD_Count   10			//AD�˲���������	

u16 ADCresults[2][AD_Count];//AD�ɼ�ֵ
u16 AD_Data[2];      //ƽ�����AD�ɼ�ֵ
__IO uint16_t ADC_ConvertedValue[AD_Count][2];//���ڴ洢adc_dma���ڴ�
u8 flag_ADC = 0;//ADC_DMA������ɱ�־

//������������
u8 first_get_T_flag;//���ڳ���ʼ�ɼ��¶�����ֵ���ɼ���ɺ����㣬����ϵͳ��λ���²ɼ�
u8 T_set;
u8 k=0;
u32 T_set_data_sum=0;
u16 Vol_ad_data;

//ת����ĵ�ѹֵ
float Vol_real;

/**
  * @brief  ADC�˲�
  * @param  ��
  * @retval ��
  */
void ADC_Filter(void)
{
	u8 i, j;
	u8 T_set_data=0;
	u32 temp[2]={0};
	/*��DMA������ȡ��AD����*/
	for(i=0; i<2; i++)
	{
		for(j=0; j<AD_Count; j++)
		{
			ADCresults[i][j] = ADC_ConvertedValue[j][i];	
		}	
	}

	/*ȡֵ���ȡƽ��*/
	for(i=0; i<2; i++)
	{
		AD_Data[i] = 0;
		for(j=0; j<AD_Count; j++)
		{
			temp[i] += ADCresults[i][j];
		}		
		AD_Data[i] = temp[i] / AD_Count;
	}
	Vol_ad_data = AD_Data[0];//��ѹ��ADֵ�����͵�ѹ����
	T_set_data = (u8)((float)AD_Data[1]/4095*110);//�¶�����ADֵת�¶�����ֵ
	if(first_get_T_flag==1)//���ƽ��ȡ�¶�����ֵ
	{
		k++;
		T_set_data_sum += T_set_data;
		if(k==10)
		{
			first_get_T_flag=0;
			T_set = T_set_data_sum/10;
			T_set_data_sum=0;
			k=0;
		}
	}
//	if(first_get_T_flag==2)
//	{
//		k++;
//		T_set_data_sum += T_set_data;
//		if(k==10)
//		{
//			first_get_T_flag=0;
//			T_set = T_set_data_sum/10;
//			T_set_data_sum=0;
//			k=0;
//		}
//	}
	//Vol_real = (float)Vol_ad_data/4095*3.3;//ʵ�ʵ�ѹ��ûʲô�ã���ͷȥ��
}

void adc_init(void)
{
	ADC_InitTypeDef     ADC_InitStruct;
  DMA_InitTypeDef     DMA_InitStruct;
	NVIC_InitTypeDef    NVIC_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	
	/* Enable  GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* ADC1 Periph clock enable */
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);//ADC���ʱ��Ƶ��14M��14>48/4
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

	 /* Configure PA.01 02  as analog input */ 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;//GPIO_Pin_1��Ϊ��ѹ�ɼ���GPIO_Pin_2��Ϊ�¶���ֵ����ɼ�
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PA,����ʱ������������
	
	ADC_DeInit(ADC1);//������ADC�ļĴ���ȫ����Ϊȱʡֵ
	ADC_StructInit(&ADC_InitStruct); //��ʼ��adc�ṹ�壬���������������·���ݽ���
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;//����adc�ֱ���
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;//��������ת��
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ�������
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;//adc�ɼ������Ҷ���
	ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Upward;//����ɨ��
	ADC_Init(ADC1, &ADC_InitStruct);//��ʼ��
	ADC_ChannelConfig(ADC1, ADC_Channel_1 | ADC_Channel_2, ADC_SampleTime_239_5Cycles); //ͨ��1��2�ɼ���������
	ADC_GetCalibrationFactor(ADC1);//У׼
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);//ѭ��ģʽ�µ�DMA����
	ADC_DMACmd(ADC1, ENABLE);//ʹ��ADCDMA
	ADC_Cmd(ADC1, ENABLE); //ʹ��ADC
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));//�ȴ�ADCEN��־
	ADC_StartOfConversion(ADC1);//�������
	
	//����ÿ��ͨ���ɼ�50�Σ�������ͨ�����ɼ�������ɺ����DMA�жϣ����ñ�־
	DMA_DeInit(DMA1_Channel1);//��λDMA1_Channel1
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;	//DMA����ADC����ַ
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;//DMA�ڴ����ַ
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//������Ϊ���ݴ���Դ
	DMA_InitStruct.DMA_BufferSize = 2 * AD_Count;//DMAͨ����DMA����Ĵ�С
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ�Ĵ�������
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ�Ĵ�������
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//����ȡֵ��С����Ϊ����
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//���ݴ�С����Ϊ����
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;//DMAѭ��ģʽ������ɺ����¿�ʼ����
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;//DMA���ȼ�����Ϊ��
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;//DMAͨ��x�����ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStruct);//DMA��ʼ��
	DMA_ClearITPendingBit(DMA1_IT_TC1);//���һ��DMA�жϱ�־                               
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);//ʹ��DMA��������ж�
	
	
	NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn; //ѡ��DMA1ͨ���ж� 
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;  //�ж�ʹ��
	NVIC_InitStruct.NVIC_IRQChannelPriority = 1;  //���ȼ���Ϊ0
	NVIC_Init(&NVIC_InitStruct);  //ʹ�� DMA �ж�
	
	DMA_Cmd(DMA1_Channel1, ENABLE);//DMA1 Channel1 ʹ��
}







/**
  * @brief  DMA1_Channel1�жϷ�����
  * @param  ��
  * @retval ��
  */
void DMA1_Channel1_IRQHandler()  
{  
	/*�ж�DMA��������ж�*/ 
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)                        
	{ 
		flag_ADC = 1;
	}	
	/*���DMA�жϱ�־λ*/	
	DMA_ClearITPendingBit(DMA1_IT_TC1);                      
}  








#if 0
__IO uint16_t RegularConvData_Tab;
void adc_dma_init(void)
{
	ADC_InitTypeDef     ADC_InitStruct;
  DMA_InitTypeDef     DMA_InitStruct;
	GPIO_InitTypeDef    GPIO_InitStruct;
	
	ADC_DeInit(ADC1);
	
	/* Enable  GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  /* ADC1 Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);

  /* Configure PA.01  as analog input */ 
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1 ;//GPIO_Pin_1��Ϊ��ѹ�ɼ���GPIO_Pin_2��Ϊ�¶���ֵ����ɼ�
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOA, &GPIO_InitStruct);				// PA1,����ʱ������������

 
  /* DMA1 Channel1 Config */
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_Address;
  DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&RegularConvData_Tab;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStruct.DMA_BufferSize =4;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStruct);
  
  /* DMA1 Channel1 enable */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC DMA request in circular mode */
  ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);//DMAѭ��ģʽ
  
  /* Enable ADC_DMA */
  ADC_DMACmd(ADC1, ENABLE);  
  
  /* Initialize ADC structure */
  ADC_StructInit(&ADC_InitStruct);
  
  /* Configure the ADC1 in continous mode withe a resolutuion equal to 12 bits  */
  ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; 
  ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Backward;
  ADC_Init(ADC1, &ADC_InitStruct); 
 
  
  /* Convert the ADC1 Vref  with 55.5 Cycles as sampling time */ 
  ADC_ChannelConfig(ADC1, ADC_Channel_1  , ADC_SampleTime_55_5Cycles); 
  
  /* ADC Calibration */
  ADC_GetCalibrationFactor(ADC1);
  ADC_DMACmd(ADC1, ENABLE);
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
  while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
  ADC_StartOfConversion(ADC1);
	

}

#endif
