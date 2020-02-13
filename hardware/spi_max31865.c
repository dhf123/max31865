#include "spi_max31865.h"
#include "delay.h"
#include <stdio.h>
#include "math.h"
#include "gpio.h"


#define LED_TURN() 										   GPIOA->ODR ^= GPIO_Pin_0

static float a = 0.00390830;
static float b = -0.0000005775;



void SPI_MAX31865_Init(void)
{	
		u8 read_value = 0;
  
    SPI_InitTypeDef  SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA| RCC_AHBPeriph_GPIOB,  ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
    
		//CS
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(SPI_MAX31865_CS_GPIO_PORT, &GPIO_InitStructure); 
    // Deselect the MAX31865: Chip Select high
    SPI_MAX31865_CS_HIGH();
    
    //SCK MISO MOSI配置  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    
    //SCK
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_SPI_SCK_PIN;
    GPIO_Init(SPI_MAX31865_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
    
    //MISO
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_SPI_MISO_PIN;
    GPIO_Init(SPI_MAX31865_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
    
    //MOSI
    GPIO_InitStructure.GPIO_Pin = SPI_MAX31865_SPI_MOSI_PIN;
    GPIO_Init(SPI_MAX31865_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
         
    
		//DRDY
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   
    GPIO_InitStructure.GPIO_Pin = DRDY_PIN;
    GPIO_Init(DRDY_PORT, &GPIO_InitStructure);
    
    //复用配置
    GPIO_PinAFConfig(SPI_MAX31865_SPI_SCK_GPIO_PORT, SPI_MAX31865_SPI_SCK_SOURCE, GPIO_AF_0);
    GPIO_PinAFConfig(SPI_MAX31865_SPI_MISO_GPIO_PORT, SPI_MAX31865_SPI_MISO_SOURCE, GPIO_AF_0);
    GPIO_PinAFConfig(SPI_MAX31865_SPI_MOSI_GPIO_PORT, SPI_MAX31865_SPI_MOSI_SOURCE, GPIO_AF_0);    
    
    
    //SPI配置
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;       
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                                                
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                                                                   
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                                                                         
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                                                                                  
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                                                                     
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;         
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                                                            
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI_MAX31865_SPI, &SPI_InitStructure);
    SPI_RxFIFOThresholdConfig(SPI_MAX31865_SPI, SPI_RxFIFOThreshold_QF);
    
    //使能SPI
    SPI_Cmd(SPI_MAX31865_SPI, ENABLE);
    
    //等待配置完成
    delay_ms(200);


		//配置
		SPI_MAX31865_Write( 0x80,0xD0 );
		read_value = SPI_MAX31865_Read(0x00);
		if(read_value == 0xD0)
		{
			
			SPI_MAX31865_Write( 0x83,0xFF );
			SPI_MAX31865_Write( 0x84,0xFF );
			SPI_MAX31865_Write( 0x85,0x00 );
			SPI_MAX31865_Write( 0x86,0x00 );
		}
		else
		{
			
			while(1);//在此处保持循环
		}
}



//u8 get_temp_times_over_zero;
//u8 get_temp_times_over_fifteen;

u8 temp_detc_count;	//温度测试计数

//温度转换
void MAX31865_Get_Temp(void) 
{
		u8 msb_rtd,lsb_rtd;//电阻寄存器
		u8 fault_test;//故障检测，转换中检测
		float RTD;
		float R;
		float Temp;
	
    lsb_rtd = SPI_MAX31865_Read(0x02);		//读取RTD_LSB
    fault_test = lsb_rtd & 0x01;       		//RTD_LSB的bit0是一个故障位,表明RTD是否错误
    while (fault_test == 0) 							//RTD_LSB的bit0=0表示正确
    {
       LED_TURN();
        msb_rtd = SPI_MAX31865_Read(0x01);//读取RTD_MSB
        //转换操作
				RTD = (((msb_rtd<<8)|lsb_rtd)&0xFFFE)>>1; //Combining RTD_MSB and RTD_LSB to protray decimal value.
        R = (RTD * 400) / 32768; //Conversion of ADC RTD code to resistance  ADC转换RTD代码为电阻值
			
				//计算1
        Temp = -100 * a + sqrt(100 * 100 * a * a- 4 * 100 * b* (100 - R)); //RTD电阻值转化温度
        Temp = Temp / (2 * 100 * b);
			
				
			temp_detc_count++;
			if(temp_detc_count > 10)
			{
				temp_detc_count = 11;
			}
			
			//上电检测，第一次运行，温度转换以第十次为准
			if(temp_detc_count == 10)
			{
				//温度低于0度，开启加热，关闭供电
				//温度介于0-5度之间，开启加热，开启供电
				//温度高于5度，关闭加热，开启供电
				if(Temp < 0) 
				{
					HEAT_ON();
					POWER_OFF();
				}
				else if(Temp >= 0 && Temp < 5)
				{
					HEAT_ON();
					POWER_ON();
				}
				else if(Temp >= 5)
				{
					HEAT_OFF();
					POWER_ON();
				}
			}
			//后面运行
			else if(temp_detc_count == 11)
			{
				//温度低于-5度，开启加热，关闭供电
				//温度介于-5到0度之间，开启加热，保持供电
				//温度介于0到5度之间，开启加热，保持供电
				//温度高于5度，关闭加热，保持供电
				if(Temp < -5)
				{
					HEAT_ON();
					POWER_OFF();
				}
				//温度
				else if(Temp >= -5 && Temp < 0)
				{
					HEAT_ON();
					POWER_ON();
				}
				else if(Temp >= 0 && Temp < 5)
				{
					HEAT_ON();
					POWER_ON();
				}
				else if(Temp >= 5)
				{
					HEAT_OFF();
					POWER_ON();
				}
			}
				
			
			
//				
//				if(Temp>=15)
//				{
//					get_temp_times_over_fifteen++;
//					if(get_temp_times_over_fifteen>=10)
//					{
//						HEAT_OFF();
//						get_temp_times_over_fifteen = 0;
//					}
//				}
//				else
//				{
//					get_temp_times_over_fifteen = 0;
//				}
//				
//				
//				if(Temp>=0)
//				{
//					get_temp_times_over_zero++;
//					if(get_temp_times_over_zero>=10)
//					{
//						POWER_ON();
//						get_temp_times_over_zero = 0;
//					}
//				}
//				else if(Temp<0)
//				{
//					POWER_OFF();
//					HEAT_ON();
//					get_temp_times_over_zero = 0;
//				}
				
				
				
				
				//printf("%.2f\r\n",Temp);
				
				delay_ms(100);
				//循环读取
        lsb_rtd = SPI_MAX31865_Read(0x02);
        fault_test = lsb_rtd & 0x01;
    }
		//printf("Conversion error!\r\n");
    //转换出错
}

//故障检测
void MAX31865_Fault_Detection(u8 fault) 
{
    u8 temp = 0;
    temp = fault & 0x80;//检测bit7位
    if (temp > 0) 
		{
        //printf("Bit D7 is Set. It's Possible your RTD device is disconnected from RTD+ or RTD-. Please verify your connection and High Fault Threshold Value!\r\n");
    }
    temp = fault & 0x40;//检测bit6位
    if (temp > 0) 
		{
        //printf("Bit D6 is Set. It's Possible your RTD+ and RTD- is shorted. Please verify your connection and your Low Fault Threshold Value!\r\n");
    }
    temp = fault & 0x20;//检测bit5位
    if (temp > 0) 
		{
        //printf("Bit D5 is Set. Vref- is greater than 0.85 * Vbias!\r\n");
    }
    temp = fault & 0x10;//检测bit4位
    if (temp > 0) 
		{
        //printf("Bit D4 is Set. Please refer to data sheet for more information!\r\n");
    }
    temp = fault & 0x08;//检测bit3位
    if (temp > 0) 
		{
       // printf("Bit D3 is Set. Please refer to data sheet for more information!\r\n");
    }
    temp = fault & 0x04;//检测bit2位
    if (temp > 0) 
		{
       // printf("Bit D2 is Set. Please refer to data sheet for more information!\r\n");
    }
}














//SPI读写
uint8_t SPI_ReadWriteByte(uint8_t value)
{
  while (SPI_I2S_GetFlagStatus(SPI_MAX31865_SPI, SPI_I2S_FLAG_TXE) == RESET);
		SPI_SendData8(SPI_MAX31865_SPI, value);

  while (SPI_I2S_GetFlagStatus(SPI_MAX31865_SPI, SPI_I2S_FLAG_RXNE) == RESET);
		return SPI_ReceiveData8(SPI_MAX31865_SPI);
}


//从MAX31865中读取一个字节的数据
//先发送一个字节的寄存器地址数据，然后发送0x00表示读取
uint8_t SPI_MAX31865_Read(u8 address)         
{
    u8 value = 0;
    
    SPI_MAX31865_CS_LOW();  
    value =SPI_ReadWriteByte(address);  //发送读取地址，0x01,0x02....                 
    value = SPI_ReadWriteByte(0x00);   
    SPI_MAX31865_CS_HIGH();
    
    return value;
}


void SPI_MAX31865_Write(u8 address, u8 value)         
{
    SPI_MAX31865_CS_LOW();   
    SPI_ReadWriteByte(address);      //发送写地址，0x80,0x82             
    SPI_ReadWriteByte(value); 		//发送数据
    SPI_MAX31865_CS_HIGH();
}


