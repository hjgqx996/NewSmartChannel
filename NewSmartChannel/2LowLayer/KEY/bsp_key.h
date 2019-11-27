/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : key.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��25��
  ����޸�   :
  ��������   : ȫ����(B��)���Ű���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��25��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/
#ifndef __bsp_KEY_H
#define __bsp_KEY_H	 

#include "sys.h" 
#include "delay.h"

#define KEY_DOOR_B          PEin(9) 
#define KEY_FIREFIGHTING    PEin(2)

#define KEY_OPEN_DOOR_A     PDin(7)
#define KEY_OPEN_DOOR_B     PDin(4)


/* �����ڶ�Ӧ��RCCʱ�� */
#define RCC_ALL_KEY             (RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE)

#define GPIO_PORT_KEY           GPIOE
#define GPIO_PIN_KEY_DOOR_B     GPIO_Pin_9
#define GPIO_PIN_FIREFIGHTING    GPIO_Pin_2


#define GPIO_PORT_OPEN_DOOR    GPIOD
#define GPIO_PIN_OPEN_DOOR_A     GPIO_Pin_7
#define GPIO_PIN_OPEN_DOOR_B     GPIO_Pin_4
  	



#define KEY_NONE                0   //�ް���
#define KEY_DOOR_B_PRES 	    1	//B�ſ��ż�����
#define KEY_FIREFIGHTING_PRES 	2	//��������������
#define KEY_OPEN_DOOR_A_PRES 	3	//A�ſ��ż�����
#define KEY_OPEN_DOOR_B_PRES 	4	//B�ſ��ż�����


#define KEY_ON	1
#define KEY_OFF	0



void bsp_key_Init(void);	//IO��ʼ��
uint8_t bsp_key_Scan(uint8_t);  		//����ɨ�躯��		

uint8_t bsp_Gpio_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);

#endif

