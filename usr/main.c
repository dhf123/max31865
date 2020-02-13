#include "stm32f0xx.h"
#include "gpio.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "delay.h"
#include "ds18b20.h"
#include "wdog.h"
#include "exti.h"
#include <stdio.h>
#include "math.h"

#include "spi_max31865.h"

#define disable_irq() __disable_irq() 
#define enable_irq() __enable_irq() 





u8 Fault_Error;


int main(void)
{
	/*系统时钟初始化为48M
	各个时钟如下:
	LSI_RC=40K	HSI_RC=8M HSE=8M HSI14_RC=14M
	PLL=SYSCLK=HCLK=AHB=FCLK=APB1=48M
	*/
	SystemInit();
	/*将systick作为系统延时精度定时器*/
	delay_init();
	led_init();
	control_init();
	usart_init();
	
	LED_ON();
	//printf("start init");
	SPI_MAX31865_Init();
	
	
	
	while(1)          
	{	
			
			
			Fault_Error = SPI_MAX31865_Read(0x07);//读取错误状态
			//printf("%x\r\n",Fault_Error);
			if (Fault_Error == 0) 
			{
					MAX31865_Get_Temp();
					delay_ms(1000); // For better readability	
			} 
			else 
			{
					//MAX31865_Fault_Detection(Fault_Error);
					SPI_MAX31865_Write(0x80, 0x82);//清除错误标志
					delay_us(700);
					SPI_MAX31865_Write(0x80, 0xD0);//重新启动自动3线转换
					delay_ms(700);
			}
			
	
	}
	
	

}





