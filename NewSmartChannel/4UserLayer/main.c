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
#define HANDSHAKE_TASK_PRIO	( tskIDLE_PRIORITY)
#define QUERYMOTOR_TASK_PRIO ( tskIDLE_PRIORITY)
#define READER_TASK_PRIO	( tskIDLE_PRIORITY + 1)
#define QR_TASK_PRIO	    ( tskIDLE_PRIORITY + 1)
#define KEY_TASK_PRIO	    ( tskIDLE_PRIORITY + 2)
#define INFRARED_TASK_PRIO	( tskIDLE_PRIORITY + 2)
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define RS485_TASK_PRIO	    ( tskIDLE_PRIORITY + 3)
#define CMD_TASK_PRIO		( tskIDLE_PRIORITY + 4)
#define START_TASK_PRIO		( tskIDLE_PRIORITY + 5)


//�����ջ��С   
#define LED_STK_SIZE 		(256)
#define MOTOR_STK_SIZE 		(1024) 
#define CMD_STK_SIZE 		(1024*2)
#define INFRARED_STK_SIZE 	(1024)
#define RS485_STK_SIZE 		(1024*1)
#define START_STK_SIZE 	    (1024)
#define QR_STK_SIZE 		(1024)
#define READER_STK_SIZE     (1024)
#define HANDSHAKE_STK_SIZE  (1024)
#define KEY_STK_SIZE        (1024)
#define QUERYMOTOR_STK_SIZE      (1024)

//#define LED_STK_SIZE 		128
//#define MOTOR_STK_SIZE 		512 
//#define CMD_STK_SIZE 		1024*2
//#define INFRARED_STK_SIZE 	512
//#define RS485_STK_SIZE 		1024*1
//#define START_STK_SIZE 	    256
//#define QR_STK_SIZE 		512
//#define READER_STK_SIZE     256
//#define HANDSHAKE_STK_SIZE  256
//#define KEY_STK_SIZE        128
//#define QUERYMOTOR_STK_SIZE      256







//�¼���־
#define TASK_BIT_0	 (1 << 0)
#define TASK_BIT_1	 (1 << 1)
#define TASK_BIT_2	 (1 << 2)
#define TASK_BIT_3	 (1 << 3)
#define TASK_BIT_4	 (1 << 4)
#define TASK_BIT_5	 (1 << 5)
#define TASK_BIT_6	 (1 << 6)
#define TASK_BIT_7	 (1 << 7)
#define TASK_BIT_8	 (1 << 8)



//#define TASK_BIT_ALL (TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 | TASK_BIT_3|TASK_BIT_4 | TASK_BIT_5 | TASK_BIT_6 )
#define TASK_BIT_ALL ( TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 |TASK_BIT_3|TASK_BIT_4|TASK_BIT_5|TASK_BIT_6|TASK_BIT_7|TASK_BIT_8)

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
static TaskHandle_t xHandleTaskRs485 = NULL;    //B�ŵ��
static TaskHandle_t xHandleTaskStart = NULL;    //���Ź�
static TaskHandle_t xHandleTaskHandShake = NULL;    // ����
static TaskHandle_t xHandleTaskKey = NULL;      //B�Ű���
static TaskHandle_t xHandleTaskQueryMotor = NULL;      //���״̬��ѯ

static EventGroupHandle_t xCreatedEventGroup = NULL;



/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/

//������
static void vTaskLed(void *pvParameters);
static void vTaskMortorToHost(void *pvParameters);
static void vTaskKey(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskInfrared(void *pvParameters);
static void vTaskRs485(void *pvParameters);
static void vTaskReader(void *pvParameters);
static void vTaskQR(void *pvParameters);
static void vTaskStart(void *pvParameters);

//���Ϳ�������
static void vTaskHandShake(void *pvParameters);
static void vTaskQueryMotor(void *pvParameters);



static void AppTaskCreate(void);
static void AppObjCreate (void);
static void App_Printf(char *format, ...);

//static void AppEventCreate (void);

//static void DisplayDevInfo (void);

//static void DisplayDevInfo(void)
//{
//	printf("Softversion :%s\r\n",gDevinfo.SoftwareVersion);
//  printf("HardwareVersion :%s\r\n", gDevinfo.HardwareVersion);
//	printf("Model :%s\r\n", gDevinfo.Model);
//	printf("ProductBatch :%s\r\n", gDevinfo.ProductBatch);	    
//	printf("BulidDate :%s\r\n", gDevinfo.BulidDate);
//	printf("DevSn :%s\r\n", gDevinfo.GetSn());
//}


int main(void)
{   
    //Ӳ����ʼ��
    bsp_Init();  

    //DisplayDevInfo();
                    
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
    //��android����
    xTaskCreate((TaskFunction_t )vTaskHandShake,
                (const char*    )"vHandShake",       
                (uint16_t       )HANDSHAKE_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )HANDSHAKE_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskHandShake);  


    //��ѯ���״̬
    xTaskCreate((TaskFunction_t )vTaskQueryMotor,
                (const char*    )"vQueryMotor",       
                (uint16_t       )QUERYMOTOR_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )QUERYMOTOR_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskQueryMotor);                  

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
                (const char*    )"vInfrared",   
                (uint16_t       )INFRARED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )INFRARED_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskInfrared);    


    //ȫ���ŵ��״̬����
    xTaskCreate((TaskFunction_t )vTaskRs485,     
                (const char*    )"vRs485",   
                (uint16_t       )RS485_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RS485_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskRs485);  

    //Τ��������
    xTaskCreate((TaskFunction_t )vTaskReader,     
                (const char*    )"vReader",   
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

    //B�Ű���
    xTaskCreate((TaskFunction_t )vTaskKey,         
                (const char*    )"vTaskKey",       
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )KEY_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskKey);                   

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
	App_Printf("ϵͳ����ִ��\r\n");
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

//��ѯ���״̬
void vTaskQueryMotor(void *pvParameters)
{
    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
    
    while(1)
    {
        comSendBuf(COM4, ReadStatus,8);//��ѯA���״̬
        RS485_SendBuf(COM5,ReadStatus,8);//��ѯB���״̬
     
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_8);  
        vTaskDelay(300);     
    }

} 

//LED������ 
void vTaskLed(void *pvParameters)
{   
    uint8_t i = 0;
    BEEP = 0;
    vTaskDelay(300);
    BEEP = 1;
    
    while(1)
    {    
        if(Motro_A== 1)
        {
          LED3=!LED3;   
        }
        else
        {
            LED3 = 0;
        }
        
        if(Motro_B == 1)
        {
          LED2=!LED2;   
        }
        else
        {
            LED2 = 0;
        }    

        //ϵͳ״̬���еƣ�ÿ500ms һ��
        i++;
        if(i == 5)
        {
            i = 0;
            LED4=!LED4; 
        }
        
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);  
        vTaskDelay(100);     
    }
}   

//motor to host ������
void vTaskMortorToHost(void *pvParameters)
{
    uint8_t buf[8] = {0};
    uint16_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};
    while (1)
    {   
        readLen = comRecvBuff(COM4,buf,8);       

        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            {    
                send_to_host(CONTROLMOTOR,buf,readLen);
                Motro_A = 0;
            }            
        }           

        
        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
        
        vTaskDelay(100);
    }

}

void vTaskKey(void *pvParameters)
{
    
	uint8_t ucKeyCode;
    
    while(1)
    {
		ucKeyCode = bsp_key_Scan(0);      
		
		if (ucKeyCode != KEY_NONE)
		{                
			switch (ucKeyCode)
			{
				/* ���ż�����ִ������λ�����Ϳ������� */
				case KEY_DOOR_B_PRES:	 
                    SendAsciiCodeToHost(REQUEST_OPEN_DOOR_B,NO_ERR,"Request to open the door");
					break;			
			
				/* �����ļ�ֵ������ */
				default:   
				App_Printf("KEY_default\r\n");
					break;
			}
		}	

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_7);  

        
		vTaskDelay(50);
	}  

}


void vTaskMsgPro(void *pvParameters)
{
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

        if(code != ERR_INFRARED)
        {
            memset(dat,0x00,sizeof(dat));
            
            dat[0] = code>>16;    
            dat[1] = code>>8;
            dat[2] = code&0xff;
            code = 0;
            send_to_host(GETSENSOR,dat,3);
        }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);    
        
        vTaskDelay(10);
    }
}

void vTaskRs485(void *pvParameters)
{
    uint8_t buf[8] = {0};
    uint8_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};
    while (1)
    {
        readLen = RS485_Recv(COM5,buf,8);       

        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            {                   
                send_to_host(DOOR_B,buf,readLen);
                Motro_B = 0;
            }            
        }
        
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
        
        vTaskDelay(100);
    }

}


void vTaskReader(void *pvParameters)
{ 
    uint32_t CardID = 0;
    uint8_t dat[4] = {0};
    
    uint32_t FunState = 0;
    char *IcReaderState;

    IcReaderState = ef_get_env("ICSTATE");
    assert_param(IcReaderState);
    FunState = atol(IcReaderState);
    
    while(1)
    {

        if(FunState != 0x00)
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
        }


		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_4);        
        
        vTaskDelay(50);
        
    }

}   


void vTaskQR(void *pvParameters)
{ 
    uint8_t recv_buf[256] = {0};
    uint16_t len = 0;  
    
    uint32_t FunState = 0;
    char *QrCodeState;

    QrCodeState = ef_get_env("QRSTATE");
    assert_param(QrCodeState);
    FunState = atol(QrCodeState);
    
    while(1)
    {
       if(FunState != 0x00)
       {
           memset(recv_buf,0x00,sizeof(recv_buf));
           len = comRecvBuff(COM3,recv_buf,sizeof(recv_buf));

           if(len > 0  && recv_buf[len-1] == 0x0A && recv_buf[len-2] == 0x0D)
           {
                SendAsciiCodeToHost(QRREADER,NO_ERR,recv_buf);
           }
       }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
        vTaskDelay(50);        
    }
}   


void vTaskHandShake(void *pvParameters)
{
    uint32_t i_boot_times = NULL;
    char *c_old_boot_times, c_new_boot_times[12] = {0};
    uint8_t bcdbuf[6] = {0};

    /* get the boot count number from Env */
    c_old_boot_times = ef_get_env("boot_times");
    assert_param(c_old_boot_times);
    i_boot_times = atol(c_old_boot_times);
    
    /* boot count +1 */
    i_boot_times ++;

    /* interger to string */
    sprintf(c_new_boot_times,"%012ld", i_boot_times);
    
    /* set and store the boot count number to Env */
    ef_set_env("boot_times", c_new_boot_times);    

    asc2bcd(bcdbuf,(uint8_t *)c_new_boot_times , 12, 0);

    send_to_host(HANDSHAKE,bcdbuf,6);  
    
    vTaskDelete( NULL ); //ɾ���Լ�


//    uint32_t i_boot_times = NULL;
//    char *c_old_boot_times, c_new_boot_times[12] = {0};
//    

//    g1msCount = 1000*10;

//    c_old_boot_times = ef_get_env("boot_times");

//    DBG("1.c_old_boot_times = %s\r\n",c_old_boot_times);

//    while(1)
//    {
//        if(g1msCount == 0)
//        {
//            break;
//        }
//        /* get the boot count number from Env */
//        c_old_boot_times = ef_get_env("boot_times");
//        assert_param(c_old_boot_times);
//        i_boot_times = atol(c_old_boot_times);
//        
//        /* boot count +1 */
//        i_boot_times ++;

//        /* interger to string */
//        sprintf(c_new_boot_times,"%012ld", i_boot_times);

//        /* set and store the boot count number to Env */
//        ef_set_env("boot_times", c_new_boot_times);   


//    }

//    c_old_boot_times = ef_get_env("boot_times");

//    DBG("2.c_old_boot_times = %s\r\n",c_old_boot_times);    

//    vTaskDelete( NULL ); //ɾ���Լ�
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




