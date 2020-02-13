#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"

//#define LED_OFF()  											 GPIOA->BSR = GPIO_Pin_0	//高电平
//#define LED_ON()                         GPIOA->BRR = GPIO_Pin_0	//低电平
//#define LED_TURN() 										   GPIOA->ODR ^= GPIO_Pin_0

//#define HEAT_OFF()  											 GPIOA->BSR = GPIO_Pin_3	//高电平
//#define HEAT_ON()                         GPIOA->BRR = GPIO_Pin_3	//低电平
//#define HEAT_TURN() 										   GPIOA->ODR ^= GPIO_Pin_3

//#define POWER_OFF()  											 GPIOA->BSR = GPIO_Pin_4	//高电平
//#define POWER_ON()                         GPIOA->BRR = GPIO_Pin_4	//低电平
//#define POWER_TURN() 										   GPIOA->ODR ^= GPIO_Pin_4

#define LED_OFF()  											 GPIO_SetBits(GPIOA,GPIO_Pin_0)	//高电平
#define LED_ON()                         GPIO_ResetBits(GPIOA,GPIO_Pin_0)	//低电平


#define HEAT_ON()  											 GPIO_SetBits(GPIOA,GPIO_Pin_1)	//高电平
#define HEAT_OFF()                       GPIO_ResetBits(GPIOA,GPIO_Pin_1)	//低电平

#define POWER_OFF()  										 GPIO_SetBits(GPIOA,GPIO_Pin_3)	//高电平
#define POWER_ON()                       GPIO_ResetBits(GPIOA,GPIO_Pin_3)	//低电平

void led_init(void);
void control_init(void);

#endif
