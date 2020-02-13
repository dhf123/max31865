#include "gpio.h"





void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
 
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);


  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	GPIO_SetBits(GPIOA,GPIO_Pin_0);//LED

}


void control_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//加热
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//附加
	GPIO_SetBits(GPIOA,GPIO_Pin_3);//电源,初始关
}
