#include "wdog.h"

//�������Ź������ھ��Ȳ��ߵĳ���
//iwdg���ڲ�����RC�����ṩʱ��40K
void iwdg_init(void)
{
	//IWDG�Ĵ���дʹ��
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	//IWDGʱ�ӷ�Ƶ40K/256=156HZ(6.4ms)
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	//ι��ʱ�䣬ע�ⲻ�ܴ���0xfff
	IWDG_SetReload(160);//(160/156)s��ι��һ�Σ�����λ,��������tim14ÿ�����ι��
	//ι��
	IWDG_ReloadCounter();
	//ʹ�ܿ��Ź�
	IWDG_Enable();
}
//ι�������Ź�
void iwdg_feed()
{
	IWDG_ReloadCounter();		
}


#if 0
//���ڿ��Ź������ھ��ȸ߳���(ι�����ˣ�ϵͳ��λ��ι�����ˣ�ϵͳ��λ�����������ж�)
void wwdg_init(void)
{
//	//WWDGʱ��ʹ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
//	//WWDGʱ�ӷ�Ƶ
//	WWDG_SetPrescaler(WWDG_Prescaler_8);
//	//����ֵ����,�ﵽ0x42֮ǰι������ι����
//	WWDG_SetWindowValue(0x42);
//	//��ʼֵ0x7F
//	WWDG_Enable(0x7F);
//	//����жϱ�־
//	WWDG_ClearFlag();
//	//ʹ���ж�
//	WWDG_EnableIT();
}

#endif







