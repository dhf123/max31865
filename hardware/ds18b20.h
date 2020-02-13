#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f0xx.h"

#define num_of_sensor 2	//�¶ȴ���������



typedef enum {FAILED = 0,PASSED = !FAILED} TestStatus;

// ROM COMMANDS
#define ROM_Search_Cmd 0xF0
#define ROM_Read_Cmd   0x33
#define ROM_Match_Cmd  0x55
#define ROM_Skip_Cmd   0xCC
#define ROM_AlarmSearch_Cmd 0xEC

// DS18B20 FUNCTION COMMANDS
#define Convert_T         0x44
#define Write_Scratchpad  0x4E
#define Read_Scratchpad   0xBE
#define Copy_Scratchpad   0x48
#define Recall_EEPROM     0xB8
#define Read_PowerSupply  0xB4

//IO��������
#define DS18B20_IO_OUT() {GPIOA->MODER&=~0x11;GPIOA->MODER|=0x01;GPIOA->OTYPER&=~0x01;GPIOA->OTYPER|=0x00;}//����PA0��� ����
#define DS18B20_IO_IN()  {GPIOA->MODER&=~0x11;GPIOA->MODER|=0x00;}//����PA0����

#define DS18B20_DQ_OUT_H GPIO_SetBits(GPIOA,GPIO_Pin_0)	//����PA0=1
#define DS18B20_DQ_OUT_L GPIO_ResetBits(GPIOA,GPIO_Pin_0)	//����PA0=0

#define DS18B20_DQ_IN GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)	//��ȡPA0������
   	
u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
short DS18B20S_Get_Temp(u8 num);//���18b20��ȡ�¶ȳ���
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20   
void DS18B20_ReadID(unsigned char *p);//��ȡID
unsigned char Read_ROM(void);//��ȡ18b20�ڲ�ROM
#endif















