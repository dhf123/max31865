#include "ds18b20.h"
#include "delay.h"	
u8 ds18b20_id[2][8] = {0};
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
 	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//PA0��ΪDQ�ڽ���ds18b20������ ��ʼ��Ϊ�������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 	//���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	//����
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

 	GPIO_SetBits(GPIOA,GPIO_Pin_0);    //����ߵ�ƽ

	DS18B20_Rst();//��λds18b20

	return DS18B20_Check();//�ȴ���Ӧ
} 
//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); //SET PA0 OUTPUT
	DS18B20_DQ_OUT_L; //����DQ
	delay_us(750);    //����750us
	DS18B20_DQ_OUT_H; //DQ=1 
	delay_us(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PA0 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUT_L; 
	delay_us(2);
    DS18B20_DQ_OUT_H; 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT_L;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT_H;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT_L;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT_H;
            delay_us(2);                          
        }
    }
}
 

unsigned char Read_ROM(void)
{
  unsigned char data;
  while(DS18B20_Init());
  DS18B20_Write_Byte(ROM_Read_Cmd);
  
  data = DS18B20_Read_Byte();

  return data;
}

void DS18B20_ReadID(unsigned char *p)
{
unsigned char n;
while(DS18B20_Init());
DS18B20_Write_Byte(ROM_Read_Cmd); 
for(n=0;n<8;n++) *p++=DS18B20_Read_Byte(); //��ȡ64λROM ID
}

//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Rst();	   
	  DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 
 

#if num_of_sensor==1
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	  short tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(float)tem*0.625;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 
#endif
#if num_of_sensor==2
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20S_Get_Temp(u8 num)
{
    u8 temp,n;
    u8 TL,TH;
	  short tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    //DS18B20_Write_Byte(0xcc);// skip rom
		DS18B20_Write_Byte(ROM_Match_Cmd);//ƥ��ID
		for(n=0;n<8;n++) 
					DS18B20_Write_Byte(ds18b20_id[num-1][n]); //����64 bit ID
   DS18B20_Write_Byte(0xbe);// convert	
	    
    TL=DS18B20_Read_Byte(); // LSB   
    TH=DS18B20_Read_Byte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(float)tem*0.625;//ת��     
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 
#endif


