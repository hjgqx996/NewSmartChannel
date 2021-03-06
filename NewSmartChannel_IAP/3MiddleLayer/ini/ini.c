/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : ini.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年5月28日
  最近修改   :
  功能描述   : 参数设置
  函数列表   :
  修改历史   :
  1.日    期   : 2019年5月28日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
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
 函 数 名  : RestoreDefaultSetting
 功能描述  : 恢复默认设置,实际是写入系统启动所需要的参数
           若使用ef_env_set_default 则需要在ef_port.c中对默认值进行修改
 输入参数  : 
             
 输出参数  : 无
 返 回 值  : void
 
 修改历史      :
  1.日    期   : 2019年5月28日
    作    者   : 张舵

    修改内容   : 新生成函数

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
    //写升级标志位
    if(ef_set_env_blob("WSPIFLASH","5050",4) == EF_NO_ERR)
    {
        ef_set_env_blob("WMCUFLASH","6060",4);
        //jump iap
        printf("jump iap\r\n");
        NVIC_SystemReset();
    }
}

