#include "def.h"


//�������ȼ�    

#define LED_TASK_PRIO		( tskIDLE_PRIORITY + 1)
//#define KEY_TASK_PRIO		( tskIDLE_PRIORITY + 2)
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)
//#define START_TASK_PRIO		( tskIDLE_PRIORITY + 4)


//�����ջ��С	
//#define START_STK_SIZE 	    128  
#define LED_STK_SIZE 		256
//#define KEY_STK_SIZE 		1024
#define UPDATE_STK_SIZE 	1024





//������
//static TaskHandle_t xHandleTaskStart = NULL;
static TaskHandle_t xHandleTaskLed = NULL;
//static TaskHandle_t xHandleTaskKey = NULL;
static TaskHandle_t xHandleTaskUpdate = NULL;







//������
//static void vTaskStart(void *pvParameters);
static void vTaskLed(void *pvParameters);
//static void vTaskKey(void *pvParameters);
static void vTaskUpdate(void *pvParameters);





int main(void)
{ 

	bsp_Init();
	
//	ef_set_env("WSPIFLASH",W_SPI_FLASH_OK);
//	ef_set_env("WMCUFLASH",W_MCU_FLASH_OK);
    

	//������ʼ����
    xTaskCreate((TaskFunction_t )vTaskUpdate,            //������
                (const char*    )"update",               //��������
                (uint16_t       )UPDATE_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )UPDATE_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t*  )&xHandleTaskUpdate);   //������        

//    xTaskCreate((TaskFunction_t )vTaskLed,            //������
//                (const char*    )"vTaskLed",               //��������
//                (uint16_t       )LED_STK_SIZE,        //�����ջ��С
//                (void*          )NULL,                  //���ݸ��������Ĳ���
//                (UBaseType_t    )LED_TASK_PRIO,        //�������ȼ�
//                (TaskHandle_t*  )&xHandleTaskLed);   //������                     
                
    vTaskStartScheduler();          //�����������
}
 
/*
*********************************************************************************************************
*	�� �� ��: vTaskStart
*	����˵��: �������񣬵ȴ����������¼���־������
*	��    ��: pvParameters ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*   �� �� ��: 4  
*********************************************************************************************************
*/
//static void vTaskStart(void *pvParameters)
//{
//	EventBits_t uxBits;
//	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* ����ӳ�1000ms */
//	
//	/* 
//	  ��ʼִ����������������ǰʹ�ܶ������Ź���
//	  ����LSI��32��Ƶ�����溯��������Χ0-0xFFF���ֱ������Сֵ1ms�����ֵ4095ms
//	  �������õ���4s�����4s��û��ι����ϵͳ��λ��
//	*/
//	bsp_InitIwdg(4000);
//	
//	/* ��ӡϵͳ����״̬������鿴ϵͳ�Ƿ�λ */
//	App_Printf("=====================================================\r\n");
//	App_Printf("=ϵͳ����ִ��\r\n");
//	App_Printf("=====================================================\r\n");
//	
//    while(1)
//    {
//		/* �ȴ������������¼���־ */
//		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
//							         TASK_BIT_ALL,       /* �ȴ�TASK_BIT_ALL������ */
//							         pdTRUE,             /* �˳�ǰTASK_BIT_ALL�������������TASK_BIT_ALL�������òű�ʾ���˳���*/
//							         pdTRUE,             /* ����ΪpdTRUE��ʾ�ȴ�TASK_BIT_ALL��������*/
//							         xTicksToWait); 	 /* �ȴ��ӳ�ʱ�� */
//		
//		if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
//		{
//            //App_Printf("�߸��û�������������\r\n");
//			IWDG_Feed(); //ι��
//			
//		}
//	    else
//		{
//			/* ������ÿxTicksToWait����һ�� */
//			/* ͨ������uxBits�򵥵Ŀ����ڴ˴�����Ǹ�������û�з������б�־ */
//		}
//    }
//}


//LED1������
static void vTaskLed(void *pvParameters)
{
    while(1)
    {
        LED1=0;
        vTaskDelay(200);
        LED1=1;
        vTaskDelay(800);
    }
}

//static void vTaskKey(void *pvParameters)
//{
//    
//	uint8_t ucKeyCode;
//	uint8_t pcWriteBuffer[500];
//    
//    while(1)
//    {
//		ucKeyCode = bsp_key_Scan(0);
//		
//		if (ucKeyCode != 0)
//		{
//			switch (ucKeyCode)
//			{
//				/* K1������ ��ӡ����ִ����� */
//				case 1:			 
//					printf("=================================================\r\n");
//					printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
//					vTaskList((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);
//                    
//					printf("\r\n������       ���м���         ʹ����\r\n");
//					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//					printf("%s\r\n", pcWriteBuffer);                    
//					break;				
//				/* K2�����£���ӡ���ڲ������� */
//				case 2:
//					//FlashTest();
//					ReadIAP();
//					break;
//				case 3:
//					ef_print_env();
//					break;
//				case 4:
//                    printf("KEY_DOWN_K4\r\n");
//                    //RestoreDefaultSetting();
//                    IAP_DownLoadToFlash();
//					break;                
//				
//				/* �����ļ�ֵ������ */
//				default:   
//				printf("KEY_default\r\n");
//					break;
//			}
//		}
//		
//		vTaskDelay(20);
//	}   

//}


static void vTaskUpdate(void *pvParameters)
{
    uint32_t file_total_size = 0;
    char *spi_flash_value;
    char *mcu_flash_value;
    char *file_size;

    int8_t spi_flash_flag = 0;
    int8_t mcu_flash_flag = 0;
    
    while(1)
    {   

        //��ӡ�²�����DEBUGʱʹ�ã�RELEASE����ɾ��
        //ef_print_env();

        //��ȡ������־λ
        spi_flash_value = ef_get_env("WSPIFLASH");
        spi_flash_flag = strcmp(W_SPI_FLASH_OK, spi_flash_value);
        printf("spi_flash_value = %s,spi_flash_flag = %d\r\n",spi_flash_value,spi_flash_flag);

        mcu_flash_value = ef_get_env("WMCUFLASH"); 
        mcu_flash_flag = strcmp(W_MCU_FLASH_OK, mcu_flash_value);       
        printf("mcu_flash_value = %s,mcu_flash_flag = %d\r\n",mcu_flash_value,mcu_flash_flag);
        
        //������ת
        if((spi_flash_flag == 0) && (mcu_flash_flag == 0))
        {
            //����Ҫ����������APP
			printf("APP STATR!\r\n");
//            IAP_JumpToApplication();
			if(IAP_JumpToApplication())
			{
                printf("iap jump error,please download app\r\n");

                //��תʧ�ܣ�����Ҫ�������б�־λ������дSPI FLASH��MCU FLASH
                ef_set_env("WSPIFLASH", W_SPI_FLASH_NEED);
                ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                
            }            
        }
        else
        {
			printf("IAP STATR! \r\n");

            //�ж�SPI FLASH�Ƿ���д�꣬MCU FLASHδд�꣬��FLASH��дMCU FLASH
            if(spi_flash_flag == 0) //�����Ѵ���flash�ڲ�������δд��mcu
            {
                printf("IAP STATR! ---> Write MCU FLASH\r\n");
                //��ȡ�ļ���С
                file_size = ef_get_env((const char * )"FileSize");
                file_total_size = str2int((const char *)file_size);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //д��MCU FLASH ��ɱ�־λ
                    if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                    {
                        printf("STM_FLASH_Write success\r\n");
                    } 
                }
            }
            else //SPI FLASH�޳����ļ����������س����ļ���д�뵽MCU FLASH��
            {     
                printf("IAP STATR! ---> Write SPI FLASH\r\n");
                
                //��Ҫ����������IAP��������
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    printf("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //д��MCU FLASH ��ɱ�־λ
                        if(ef_set_env("WMCUFLASH",W_MCU_FLASH_OK) == EF_NO_ERR)
                        {
                            printf("STM_FLASH_Write success\r\n");
                        }  
                    }
                    else
                    {
                        ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);                        
                    }
                }
            }
        }        

        vTaskDelay(100);
    }
}





