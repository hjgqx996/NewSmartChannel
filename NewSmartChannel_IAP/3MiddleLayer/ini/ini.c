/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : ini.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��5��28��
  ����޸�   :
  ��������   : ��������
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "ini.h"
#include "bsp_spi_flash.h"

#define SFUD_DEMO_TEST_BUFFER_SIZE 1024

static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];


void ReadIAP(void) 
{
    size_t i;
    uint32_t addr = 0x10000;
    size_t size = SFUD_DEMO_TEST_BUFFER_SIZE;
    size_t j = 0;

    for(j=0;j<42;j++,addr+=SFUD_DEMO_TEST_BUFFER_SIZE)
    {        
        
        bsp_sf_ReadBuffer(sfud_demo_test_buf, addr, size);  
        
        printf("Start from 0x%08X, size is %ld. The data is:\r\n", addr,size);
        printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) 
        {
            if (i % 16 == 0) 
            {
                printf("[%08X] ", addr + i);
            }
            printf("%02X ", sfud_demo_test_buf[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) 
            {
                printf("\r\n");
            }
        }
        printf("\r\n");

    }
    if (i == size) {
        printf("The flash test is success.\r\n");
    }
}


//#define param_start_addr 0x00

/*****************************************************************************
 �� �� ��  : RestoreDefaultSetting
 ��������  : �ָ�Ĭ������,ʵ����д��ϵͳ��������Ҫ�Ĳ���
           ��ʹ��ef_env_set_default ����Ҫ��ef_port.c�ж�Ĭ��ֵ�����޸�
 �������  : 
             
 �������  : ��
 �� �� ֵ  : void
 
 �޸���ʷ      :
  1.��    ��   : 2019��5��28��
    ��    ��   : �Ŷ�

    �޸�����   : �����ɺ���

*****************************************************************************/
void RestoreDefaultSetting(void)
{
    if(ef_env_set_default()== EF_NO_ERR)
    {
        printf("RestoreDefaultSetting success\r\n");
    }
    
}

void SystemReset(void)
{
    //д������־λ
    if(ef_set_env_blob("WSPIFLASH","5050",4) == EF_NO_ERR)
    {
        ef_set_env_blob("WMCUFLASH","6060",4);
        //jump iap
        printf("jump iap\r\n");
        NVIC_SystemReset();
    }
}

