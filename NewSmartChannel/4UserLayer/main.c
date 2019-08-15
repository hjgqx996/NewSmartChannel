/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : main.c
  �� �� ��   : ����
  ��    ��   : �Ŷ�
  ��������   : 2019��7��9��
  ����޸�   :
  ��������   : ������ģ��
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2019��7��9��
    ��    ��   : �Ŷ�
    �޸�����   : �����ļ�

******************************************************************************/

/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include "def.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
//�������ȼ�   


#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY)

#define READER_TASK_PRIO	( tskIDLE_PRIORITY + 1)
#define QR_TASK_PRIO	    ( tskIDLE_PRIORITY + 1)

#define INFRARED_TASK_PRIO	( tskIDLE_PRIORITY + 2)

#define CMD_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define RS485_TASK_PRIO	    ( tskIDLE_PRIORITY + 3)

#define START_TASK_PRIO		( tskIDLE_PRIORITY + 4)


//�����ջ��С    
#define LED_STK_SIZE 		128
#define MOTOR_STK_SIZE 		512 
#define CMD_STK_SIZE 		1024*2
#define INFRARED_STK_SIZE 	512
#define RS485_STK_SIZE 		1024*1
#define START_STK_SIZE 	    128
#define QR_STK_SIZE 		512
#define READER_STK_SIZE     512



//�¼���־
#define TASK_BIT_0	 (1 << 0)
#define TASK_BIT_1	 (1 << 1)
#define TASK_BIT_2	 (1 << 2)
#define TASK_BIT_3	 (1 << 3)
#define TASK_BIT_4	 (1 << 4)
#define TASK_BIT_5	 (1 << 5)
#define TASK_BIT_6	 (1 << 6)


//#define TASK_BIT_ALL (TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 | TASK_BIT_3|TASK_BIT_4 | TASK_BIT_5 | TASK_BIT_6 )
#define TASK_BIT_ALL ( TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 |TASK_BIT_3|TASK_BIT_4|TASK_BIT_5|TASK_BIT_6)

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
//������
static TaskHandle_t xHandleTaskLed = NULL;      //LED��
static TaskHandle_t xHandleTaskMotor = NULL;    //�������
static TaskHandle_t xHandleTaskCmd = NULL;      //androidͨѶ
static TaskHandle_t xHandleTaskInfrared = NULL; //�����ӳ
static TaskHandle_t xHandleTaskReader = NULL;   //Τ��������
static TaskHandle_t xHandleTaskQr = NULL;       //��ά���ͷ
static TaskHandle_t xHandleTaskRs485 = NULL;
static TaskHandle_t xHandleTaskStart = NULL;    //���Ź�

static EventGroupHandle_t xCreatedEventGroup = NULL;



/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

//������
static void vTaskLed(void *pvParameters);
static void vTaskMortorToHost(void *pvParameters);
//static void vTaskKey(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskInfrared(void *pvParameters);
static void vTaskRs485(void *pvParameters);
static void vTaskReader(void *pvParameters);
static void vTaskQR(void *pvParameters);
static void vTaskStart(void *pvParameters);






static void AppTaskCreate(void);
static void AppObjCreate (void);
static void App_Printf(char *format, ...);
//static void AppEventCreate (void);


int main(void)
{    
    //Ӳ����ʼ��
    bsp_Init();  
    
	/* �������� */
	AppTaskCreate();

	/* ��������ͨ�Ż��� */
	AppObjCreate();

    
    /* �������ȣ���ʼִ������ */
    vTaskStartScheduler();
    
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    //����LED����
    xTaskCreate((TaskFunction_t )vTaskLed,         
                (const char*    )"vTaskLed",       
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )LED_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskLed);   

    //���������Ϣ��������
    xTaskCreate((TaskFunction_t )vTaskMortorToHost,     
                (const char*    )"vTMTHost",   
                (uint16_t       )MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotor);     

    //��androidͨѶ�������ݽ���
    xTaskCreate((TaskFunction_t )vTaskMsgPro,     
                (const char*    )"cmd",   
                (uint16_t       )CMD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CMD_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskCmd);      

    //���⴫����״̬����
    xTaskCreate((TaskFunction_t )vTaskInfrared,     
                (const char*    )"vTaskInfrared",   
                (uint16_t       )INFRARED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )INFRARED_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskInfrared);    


    //485����
    xTaskCreate((TaskFunction_t )vTaskRs485,     
                (const char*    )"vTaskRs485",   
                (uint16_t       )RS485_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RS485_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskRs485);  

    //Τ��������
    xTaskCreate((TaskFunction_t )vTaskReader,     
                (const char*    )"vTaskReader",   
                (uint16_t       )READER_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )READER_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskReader);    

    //��ά��ɨ��ģ��
    xTaskCreate((TaskFunction_t )vTaskQR,     
                (const char*    )"vTaskQR",   
                (uint16_t       )QR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )QR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskQr);                  

    //���Ź�
	xTaskCreate((TaskFunction_t )vTaskStart,     		/* ������  */
                (const char*    )"vTaskStart",   		/* ������    */
                (uint16_t       )START_STK_SIZE,        /* ����ջ��С����λword��Ҳ����4�ֽ� */
                (void*          )NULL,           		/* �������  */
                (UBaseType_t    )START_TASK_PRIO,       /* �������ȼ�*/
                (TaskHandle_t*  )&xHandleTaskStart );   /* ������  */                

}


/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨ�Ż���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* �����¼���־�� */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
        App_Printf("�����¼���־��ʧ��\r\n");
    }

	/* ���������ź��� */
    gxMutex = xSemaphoreCreateMutex();
	
	if(gxMutex == NULL)
    {
        /* û�д����ɹ����û�������������봴��ʧ�ܵĴ������ */
        App_Printf("���������ź���ʧ��\r\n");
    }    

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
static void vTaskStart(void *pvParameters)
{
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* ����ӳ�1000ms */
	
	/* 
	  ��ʼִ����������������ǰʹ�ܶ������Ź���
	  ����LSI��32��Ƶ�����溯��������Χ0-0xFFF���ֱ������Сֵ1ms�����ֵ4095ms
	  �������õ���4s�����4s��û��ι����ϵͳ��λ��
	*/
	bsp_InitIwdg(4000);
	
	/* ��ӡϵͳ����״̬������鿴ϵͳ�Ƿ�λ */
	App_Printf("=====================================================\r\n");
	App_Printf("=ϵͳ����ִ��\r\n");
	App_Printf("=====================================================\r\n");
	
    while(1)
    {        
		/* �ȴ������������¼���־ */
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* �¼���־���� */
							         TASK_BIT_ALL,       /* �ȴ�TASK_BIT_ALL������ */
							         pdTRUE,             /* �˳�ǰTASK_BIT_ALL�������������TASK_BIT_ALL�������òű�ʾ���˳���*/
							         pdTRUE,             /* ����ΪpdTRUE��ʾ�ȴ�TASK_BIT_ALL��������*/
							         xTicksToWait); 	 /* �ȴ��ӳ�ʱ�� */
		
		if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
		{            
		    IWDG_Feed(); //ι��			
		}
	    else
		{
			/* ������ÿxTicksToWait����һ�� */
			/* ͨ������uxBits�򵥵Ŀ����ڴ˴�����Ǹ�������û�з������б�־ */
		}
    }
}



//LED������ 
void vTaskLed(void *pvParameters)
{
    while(1)
    {

        LED1=!LED1;  
        LED2=!LED2; 
        LED3=!LED3; 
        LED4=!LED4; 
        
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);        
        
        vTaskDelay(300);
        
    }

}   

//motor to host ������
void vTaskMortorToHost(void *pvParameters)
{
    uint8_t buf[16] = {0};
    uint8_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};
    while (1)
    {       
        if(bsp_Usart4_RecvOne(buf) == 1)
        {  
            
           if(bsp_Usart4_RecvOne(buf+1) == 1)
           {
                 
                dbh("vTaskMortorToHost", (char *)buf, 16);
                 
                if(buf[1] == 0x03)//��ȡ״̬
                {
                   readLen=bsp_Usart4_RecvAtTime(buf + 2, 5, 20);                   
                }
                else if(buf[1] == 0x06)//��ȡִ��״̬
                {
                    readLen=bsp_Usart4_RecvAtTime(buf + 2, 6, 20);
                }                

                if(readLen == 5 || readLen == 6) //���յ����ݲ�����
                {
                    iCRC = CRC16_Modbus(buf, readLen);  

                    crcBuf[0] = iCRC >> 8;
                    crcBuf[1] = iCRC & 0xff;  

                    if(crcBuf[1] == buf[readLen] && crcBuf[0] == buf[readLen+1])
                    {    
                        send_to_host(CONTROLMOTOR,buf,readLen+2);
                    }
                }
           }
        }
        
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
        
        vTaskDelay(10);
    }

}


#if 0
void vTaskKey(void *pvParameters)
{
    
	uint8_t ucKeyCode;
	uint8_t pcWriteBuffer[500];
    uint32_t id[3] = {0};

    uint16_t crc_value = 0;

    uint8_t cm4[] = { 0x02,0x7B,0x22,0x63,0x6D,0x64,0x22,0x3A,0x22,0x75,0x70,0x64,0x61,0x74,0x65,0x22,0x2C,0x22,0x76,0x61,0x6C,0x75,0x65,0x22,0x3A,0x7B,0x22,0x75,0x70,0x64,0x61,0x74,0x65,0x22,0x3A,0x22,0x41,0x37,0x22,0x7D,0x2C,0x22,0x64,0x61,0x74,0x61,0x22,0x3A,0x22,0x30,0x30,0x22,0x7D,0x03 };
    
    while(1)
    {
		ucKeyCode = bsp_key_Scan(0);
      
		
		if (ucKeyCode != KEY_NONE)
		{
            //dbg("ucKeyCode = %d\r\n",ucKeyCode);
              
			switch (ucKeyCode)
			{
				/* K1������ ��ӡ����ִ����� */
				case KEY_SET_PRES:	

					App_Printf("=================================================\r\n");
					App_Printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
					vTaskList((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);
                    
					App_Printf("\r\n������       ���м���         ʹ����\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);                    
					break;				
				/* K2�����£���ӡ���ڲ������� */
				case KEY_RR_PRES:
                    App_Printf("KEY_DOWN_K2\r\n");
					//FlashTest();
					//ReadIAP();  
                    ef_erase_bak_app( 0x10000 ); 
                    RestoreDefaultSetting();
                    SystemReset();
                    //IAP_DownLoadToFlash();					
					break;
				case KEY_LL_PRES:
                    App_Printf("KEY_DOWN_K3\r\n");
                    //ef_print_env();
					//SystemReset();
					//json_test();
                    Get_ChipID(id);
                    dbg("mcu id = %x %x %x\r\n",id[0],id[1],id[2]); 
					break;
				case KEY_OK_PRES:
                    App_Printf("KEY_DOWN_K4\r\n");
                    crc_value = CRC16_Modbus(cm4, 54);
                    App_Printf("hi = %02x, lo = %02x\r\n", crc_value>>8, crc_value & 0xff);
                    
//                    bsp_Usart3_SendString("1234");   
//                    spi_flash_demo();
//                    FlashTest();                    
//                  if(SPI_Flash_Test() == 0)
                    {
//                        BEEP = ~BEEP;
                    } 
					break;                
				
				/* �����ļ�ֵ������ */
				default:   
				App_Printf("KEY_default\r\n");
					break;
			}
		}

        /* �����¼���־����ʾ������������ */
//		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_2);
		
		vTaskDelay(20);
	}   

}
#endif




void vTaskMsgPro(void *pvParameters)
{    
//    int16_t code = 0;
    
    while(1)
    {          
        deal_Serial_Parse();    
	    deal_rx_data();

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_2);
        vTaskDelay(10);
    }
}


void vTaskInfrared(void *pvParameters)
{
    uint32_t code = 0;
    uint8_t dat[3] = {0};
    
    while(1)
    {
        code = bsp_infrared_scan();       

//        code = bsp_InfraredExitScan();

        if(code != ERR_INFRARED)
        {
            memset(dat,0x00,sizeof(dat));
            
            dat[0] = code>>16;    
            dat[1] = code>>8;
            dat[2] = code&0xff;
            send_to_host(GETSENSOR,dat,3);
        }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);    
        
        vTaskDelay(1);
    }

//    while(1)
//    {
//        code = bsp_GetStatus();        
//        
//        if(code != ERR_INFRARED)
//        {
//            printf("gSensorTop = %d, gSensorEnd = %d\r\n",gSensorTop,gSensorEnd);
//            memset(dat,0x00,sizeof(dat));            
//            dat[2] = code;            
//            send_to_host(GETSENSOR,dat,3);
//        }

//      /* �����¼���־����ʾ������������ */        
//      xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);    
//        
//      vTaskDelay(1);
//    }

}

void vTaskRs485(void *pvParameters)
{
    uint8_t recv_buf[32] = {0};
    uint16_t len = 0;
    uint16_t iCRC = 0;
    uint8_t readLen = 0;
    uint8_t crcBuf[2] = {0};
    
    while (1)
    {     
        memset(recv_buf,0x00,sizeof(recv_buf));
        
        bsp_RS485_Receive_Data(recv_buf,&len);       
        
        if(len == 7 || len == 8)
        {
            if(recv_buf[1] == 0x06)
            {
                readLen = 6;
            }else if(recv_buf[1] == 0x03)
            {
                readLen = 5;
            }
            
            iCRC = CRC16_Modbus(recv_buf, readLen);  
            
            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  
            
            if(crcBuf[1] == recv_buf[readLen] && crcBuf[0] == recv_buf[readLen+1])
            {    
                send_to_host(DOOR_B,recv_buf,len);
            }            
        }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
        
        vTaskDelay(20);
    }



}


void vTaskReader(void *pvParameters)
{ 
    uint32_t CardID = 0;
    uint8_t dat[4] = {0};
    while(1)
    {
        CardID = bsp_WeiGenScanf();

        if(CardID != 0)
        {
            memset(dat,0x00,sizeof(dat));            
            
			dat[0] = CardID>>24;
			dat[1] = CardID>>16;
			dat[2] = CardID>>8;
			dat[3] = CardID&0XFF;    
            
            send_to_host(WGREADER,dat,4);
        }  


		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_4);        
        
        vTaskDelay(20);
        
    }

}   


void vTaskQR(void *pvParameters)
{ 
    uint8_t recv_buf[256] = {0};
    uint8_t dat[256] = {0};
    uint16_t len = 0;

    

//    while(1)
//    {
//       memset(recv_buf,0x00,sizeof(recv_buf));  
//       memset(dat,0x00,sizeof(dat));  
//       
//       len = bsp_Usart3_Read(src,recv_buf,1000); 
//       
//       if(len > 0)
//       {
//            asc2bcd(dat, recv_buf, len, 0);         
//            send_to_host(QRREADER,dat,ceil(len/2));
//       }

//		/* �����¼���־����ʾ������������ */        
//		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
//        vTaskDelay(10);        
//    }

    while(1)
    {
       memset(recv_buf,0x00,sizeof(recv_buf));  
       memset(dat,0x00,sizeof(dat));  
       len = bsp_Usart3_RecvAtTime(recv_buf,100,2000);

       if(len > 0  && recv_buf[len-1] == 0x0A && recv_buf[len-2] == 0x0D)
       {
            asc2bcd(dat, recv_buf, len, 0);
            
            send_to_host(QRREADER,dat,ceil(len/2));
       }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
        vTaskDelay(10);        
    }

}   



/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void  App_Printf(char *format, ...)
{
    char  buf_str[200 + 1];
    va_list   v_args;


    va_start(v_args, format);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) format,
                                  v_args);
    va_end(v_args);

	/* �����ź��� */
	xSemaphoreTake(gxMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(gxMutex);
}




