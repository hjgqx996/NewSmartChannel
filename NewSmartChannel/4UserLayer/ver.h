/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : ver.h
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��8��12��
  ����޸�   :
  ��������   : ���г���汾����
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��8��12��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

#ifndef __VER_H
#define __VER_H
#include "stm32f4xx_conf.h"


extern const uint8_t g_revision_date[12];
extern const uint8_t g_revision_time[16];

typedef struct
{
	const unsigned char *vString;
	const unsigned char *vDate;
}VERSION_T;

extern const VERSION_T DevVersion;



#endif
----------
