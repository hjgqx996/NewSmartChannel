/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : key.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��25��
  ����޸�   :
  ��������   : ��������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��25��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#include "bsp_key.h"


//������ʼ������
void bsp_key_Init ( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_ALL_KEY, ENABLE ); //ʹ��GPIOA,GPIOCʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_RR|GPIO_PIN_KEY_LL|GPIO_PIN_KEY_SET; //KEY0 KEY1 KEY2��Ӧ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//
	GPIO_Init ( GPIO_PORT_KEY, &GPIO_InitStructure ); //��ʼ��GPIOE2,3,4


	GPIO_InitStructure.GPIO_Pin = GPIO_PIN_KEY_OK;//WK_UP��Ӧ����PA0
	GPIO_Init ( GPIO_PORT_KEY_OK, &GPIO_InitStructure ); //��ʼ��GPIOA0
}

//������������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2����
//4��KEY_UP���� ��WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY_UP!!
u8 bsp_key_Scan ( u8 mode )
{
	static u8 key_up=1;//�������ɿ���־
	if ( mode )
	{
		key_up=1;    //֧������
	}
	if ( key_up&& ( KEY_SET==0||KEY_RR==0||KEY_LL==0||KEY_OK==0 ) )
	{
		delay_ms ( 10 ); //ȥ����
		
		key_up=0;
        
		if ( KEY_SET==0 )
		{
			return KEY_SET_PRES;
		}
		else if ( KEY_RR==0 )
		{
			return KEY_RR_PRES;
		}
		else if ( KEY_LL==0 )
		{
			return KEY_LL_PRES;
		}
		else if ( KEY_OK==0 )
		{
			return KEY_OK_PRES;
		}
	}
	else if ( KEY_SET==1&&KEY_RR==1&&KEY_LL==1&&KEY_OK==1 )
	{
		key_up=1;
	}
    
	 return KEY_NONE;// �ް�������
}

