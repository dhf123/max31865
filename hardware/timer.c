#include "timer.h"
#include "gpio.h"
#include "wdog.h"

//����ɢ�ȷ��ȵ�Ƶ�� ������Ϊ25Khz
void pwm_pb1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef       TIM_OCInitStructure;
	
	//����PB1ΪTIM3_CH4�ĸ���PWM���
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_1);
	
	//����TIM3
	/*
	TIM3��APB1����ʹ�ܣ�APB1ʱ��Ϊ48M����SYSCLK��HCLK������Ϊ48M�����
	25K=48M=1920
	
	����
	TIM_Prescaler=0
	TIM_ClockDivision=0
	TIM_Period=1920
	
	����PWM1ģʽ���������ǰ����ߵ�ƽ
	*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ�ܶ�ʱ��3ʱ��
	TIM_TimeBaseStructure.TIM_Period        = 1920;// �����Զ���װ����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler     = 0;//����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);    //��ʼ����ʱ��3

	TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;// PWM1ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;//�������ǰ����ߣ��������
  TIM_OCInitStructure.TIM_Pulse = PWM_OFF;

	TIM_OC4Init(TIM3, &TIM_OCInitStructure);//ͨ��4

	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);//ʹ��Ԥװ�ؼĴ���
	TIM_Cmd(TIM3, ENABLE);
}

//�ı�pwmռ�ձ�
void chang_pb1_pwm_duty_cycle(uint16_t CCRx_Val)
{
	TIM3->CCR4=CCRx_Val;
	TIM_Cmd(TIM3, ENABLE);
}

void timer14_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE); 
	TIM_TimeBaseStructure.TIM_Period        = 59999;// �����Զ���װ����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler     = 799;//����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseStructure);    //��ʼ����ʱ��3
	
	TIM_ITConfig(  //ʹ�ܻ���ʧ��ָ����TIM�ж�
		TIM14, //TIM14
		TIM_IT_Update ,
		ENABLE  //ʹ��
		);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;  //��ռ���ȼ�1��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM14, ENABLE);  //ʹ��TIMx����
}



#if 0
uint16_t PrescalerValue = 0;
__IO uint16_t CCR1_Val = 10000;
__IO uint16_t CCR2_Val = 20000;
__IO uint16_t CCR3_Val = 30000;
__IO uint16_t CCR4_Val = 40000;
__IO uint16_t temp1;
__IO uint16_t temp2;
__IO uint16_t temp3;
__IO uint16_t temp4;
uint16_t capture = 0;
void timer_init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ�ܶ�ʱ��3ʱ��
	/* -----------------------------------------------------------------------
	TIM3 ����: ����Ƚ�ʱ��ģʽ:

	�������� TIM3����ʱ��(TIM3CLK) ����ΪAPB1 ʱ�� (PCLK1),  
		=> TIM3CLK = PCLK1 = SystemCoreClock = 48 MHz
				
	�� TIM3 ����ʱ�� ��Ϊ6 MHz, Ԥ��Ƶ�����԰������湫ʽ���㣺
		 Prescaler = (TIM3CLK / TIM3 counter clock) - 1
		 Prescaler = (PCLK1 /6 MHz) - 1
	
	��ʵ��TIM3 counter clock=48M=48000000
	CC1 ��ת�� = TIM3 counter clock / CCR1_Val = 4800 Hz
	CC2 ��ת��= TIM3 counter clock / CC2_Val = 2400 Hz
	CC3 ��ת�� = TIM3 counter clock / CCR3_Val = 1600 Hz
	CC4 ��ת��= TIM3 counter clock / CCR4_Val = 1200 Hz
	----------------------------------------------------------------------- */
  /* Time ��ʱ���������� */
  TIM_TimeBaseStructure.TIM_Period = 65535;
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* Ԥ��Ƶ������ */
  TIM_PrescalerConfig(TIM3, 0, TIM_PSCReloadMode_Immediate);

  /* ����Ƚ�ʱ��ģʽ�������� */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	/* ����Ƚ�ʱ��ģʽ����: Ƶ��1*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OC1Init(TIM3, &TIM_OCInitStructure);
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* ����Ƚ�ʱ��ģʽ����: Ƶ��2*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM3, &TIM_OCInitStructure);
  TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* ����Ƚ�ʱ��ģʽ����: Ƶ��3*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);

  /* ����Ƚ�ʱ��ģʽ����: Ƶ��4*/
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM3, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Disable);
   
  /* TIM �ж�ʹ�� */
  TIM_ITConfig(TIM3,TIM_IT_CC1|TIM_IT_CC2| TIM_IT_CC3 | TIM_IT_CC4, ENABLE);

  /* TIM3 ʹ�� */
  TIM_Cmd(TIM3, ENABLE);
}

#endif


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		}
	
#if 0	
	if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
     temp1++;
    /* LED3 toggling with frequency = 219.7 Hz */
		LED_TURN;
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare1(TIM3, capture + CCR1_Val);
  }
	if (TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
     temp2++;
    /* LED3 toggling with frequency = 219.7 Hz */
		
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare2(TIM3, capture + CCR2_Val);
  }
  if (TIM_GetITStatus(TIM3, TIM_IT_CC3) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
     temp3++;
    /* LED3 toggling with frequency = 219.7 Hz */
		
    capture = TIM_GetCapture3(TIM3);
    TIM_SetCompare3(TIM3, capture + CCR3_Val);
  }
  if(TIM_GetITStatus(TIM3, TIM_IT_CC4) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC4);
     temp4++;
    /* LED4 toggling with frequency = 439.4 Hz */

    capture = TIM_GetCapture4(TIM3);
    TIM_SetCompare4(TIM3, capture + CCR4_Val);
  }
#endif
}






