#include "def.h"


//�������ȼ�  
#define UPDATE_TASK_PRIO	( tskIDLE_PRIORITY + 3)

//�����ջ��С	

#define LED_STK_SIZE 		256
#define UPDATE_STK_SIZE 	1024*2

//������
static TaskHandle_t xHandleTaskUpdate = NULL;

//������
static void vTaskUpdate(void *pvParameters);
const uint8_t SoftBulidDate[] = __DATE__ " " __TIME__;
const uint8_t Softversion[] = "1.0.1";
const uint8_t DevModel[] = "SmartChannelIAP";

static void DisplayDevInfo (void);

static void DisplayDevInfo(void)
{
	printf("Softversion :%s\r\n",Softversion);
	printf("Model :%s\r\n", DevModel);
	printf("ProductBatch :%s\r\n", SoftBulidDate);	
}

int main(void)
{ 
	bsp_Init();
	
    DisplayDevInfo();

	//������ʼ����
    xTaskCreate((TaskFunction_t )vTaskUpdate,            //������
                (const char*    )"update",               //��������
                (uint16_t       )UPDATE_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )UPDATE_TASK_PRIO,        //�������ȼ�
                (TaskHandle_t*  )&xHandleTaskUpdate);   //������        
                
    vTaskStartScheduler();          //�����������
}
 


static void vTaskUpdate(void *pvParameters)
{
    uint32_t file_total_size = 0;
    char spi_flash_value[4] = {0};
    char mcu_flash_value[4] = {0};    
    char file_size_value[16] = {0};

    char spi_flash_flag = 0;
    char mcu_flash_flag = 0;
    
    char spi_flash_len = 0;
    char mcu_flash_len = 0;
    int fileLen = 0;

    
    while(1)
    {   

        //��ӡ�²�����DEBUGʱʹ�ã�RELEASE����ɾ��
        //ef_print_env();
        
//        ef_set_env("WSPIFLASH", W_SPI_FLASH_OK);
//        ef_set_env("WMCUFLASH", W_MCU_FLASH_OK);

//        ef_set_env("WMCUFLASH", W_MCU_FLASH_NEED);
//        while(1);

        //��ȡ������־λ
        spi_flash_len = ef_get_env_blob("WSPIFLASH", spi_flash_value, sizeof(spi_flash_value) , NULL);
        spi_flash_flag = memcmp(W_SPI_FLASH_NEED, spi_flash_value,spi_flash_len);

        
        mcu_flash_len = ef_get_env_blob("WMCUFLASH", mcu_flash_value, sizeof(mcu_flash_value) , NULL);
        mcu_flash_flag = memcmp(W_MCU_FLASH_NEED, mcu_flash_value,mcu_flash_len);       

        //DBG("spi value = %s,len = %d,spiflag = %d,mcu value = %s,len = %d,mcuflag = %d\r\n",\
        spi_flash_value,spi_flash_len,spi_flash_flag,mcu_flash_value,mcu_flash_len,mcu_flash_flag);

        
        //������ת
        if((spi_flash_flag != 0) && (mcu_flash_flag != 0))
        {
            DBG("jump app \r\n");
            //����Ҫ����������APP
			if(IAP_JumpToApplication())
			{
                DBG("iap jump error,please download app\r\n");

                //��תʧ�ܣ�����Ҫ�������б�־λ������дSPI FLASH��MCU FLASH
                ef_set_env_blob("WSPIFLASH", W_SPI_FLASH_NEED,4);
                ef_set_env_blob("WMCUFLASH", W_MCU_FLASH_NEED,4);  

                //add 2019.09.05���Է��֣���Ҫ�������������ɹ�
                NVIC_SystemReset();
            }            
        }
        else
        {
            //�ж�SPI FLASH�Ƿ���д�꣬MCU FLASHδд�꣬��FLASH��дMCU FLASH
            if(spi_flash_flag == 0) //�����Ѵ���flash�ڲ�������δд��mcu
            {
                DBG("IAP STATR! ---> Write SPI FLASH\r\n");
                            
                //��Ҫ����������IAP��������
                file_total_size = IAP_DownLoadToFlash();
                
                if(file_total_size > 0)
                {
                    DBG("write stm flash\r\n");
                    if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                    {
                        //д��MCU FLASH ��ɱ�־λ
                        if(ef_set_env_blob("WMCUFLASH",W_MCU_FLASH_OK,4) == EF_NO_ERR)
                        {
                            DBG("STM_FLASH_Write success\r\n");
                        }  
                    }
                    else
                    {
                        ef_set_env_blob("WMCUFLASH", W_MCU_FLASH_NEED,4);                        
                    }
                }
            }
            else //SPI FLASH�޳����ļ����������س����ļ���д�뵽MCU FLASH��
            {  

                DBG("IAP STATR! ---> Write MCU FLASH\r\n");
                //��ȡ�ļ���С
                //file_size = ef_get_env((const char * )"FileSize");
                
                ef_get_env_blob("FileSize", NULL, 0, &fileLen);
                fileLen = ef_get_env_blob("FileSize", file_size_value, sizeof(file_size_value) , NULL);  

                file_total_size = str2int((const char *)file_size_value);

                if(IAP_DownLoadToSTMFlash(file_total_size) == 1)
                {
                    //д��MCU FLASH ��ɱ�־λ
                    if(ef_set_env_blob("WMCUFLASH",W_MCU_FLASH_OK,4) == EF_NO_ERR)
                    {
                        DBG("STM_FLASH_Write success\r\n");
                    } 
                }                
            }
        }        

        vTaskDelay(100);
    }
}





