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

static void check_msg_queue(void);

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
//#define LED_STK_SIZE 		(1024)
//#define MOTOR_STK_SIZE 		(1024) 
//#define CMD_STK_SIZE 		(1024*2)
//#define INFRARED_STK_SIZE 	(1024)
//#define RS485_STK_SIZE 		(1024)
//#define START_STK_SIZE 	    (256)
//#define QR_STK_SIZE 		(1024)
//#define READER_STK_SIZE     (1024)
//#define HANDSHAKE_STK_SIZE  (1024)
//#define KEY_STK_SIZE        (512)
//#define QUERYMOTOR_STK_SIZE      (512)


#define LED_STK_SIZE 		(1024)
#define MOTOR_STK_SIZE 		(1024) 
#define CMD_STK_SIZE 		(1024*1)
#define INFRARED_STK_SIZE 	(1024)
#define RS485_STK_SIZE 		(1024)
#define START_STK_SIZE 	    (512)
#define QR_STK_SIZE 		(1280)
#define READER_STK_SIZE     (1024)
#define HANDSHAKE_STK_SIZE  (1024)
#define KEY_STK_SIZE        (1024)
#define QUERYMOTOR_STK_SIZE      (512)

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

//��ȡ���״̬������
#define READ_MOTOR_STATUS_TIMES 20


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

//�¼����
static EventGroupHandle_t xCreatedEventGroup = NULL;


//������Ϊ�ѻ�ģʽ����£����⣬����������ά��������
//�ѻ�ģʽ���ж��������ı��뷶Χ���Լ���ά��ļ������
#ifdef USEQUEUE
#define MONITOR_TASK_PRIO	( tskIDLE_PRIORITY + 4)
#define MONITOR_STK_SIZE   (1024)
static TaskHandle_t xHandleTaskMonitor = NULL;    //�������
static void vTaskMonitor(void *pvParameters);
static QueueHandle_t xTransQueue = NULL;
#endif


volatile int8_t mau_key = 0;

static QueueHandle_t xTransKeyQueue = NULL;
static QueueHandle_t xTransMotorBQueue = NULL;


#define  KEY_QUEUE_LEN    10   /* ���еĳ��ȣ����ɰ������ٸ���Ϣ */
#define  KEY_QUEUE_SIZE   4   /* ������ÿ����Ϣ��С���ֽڣ� */


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
static void vTaskQueryMotor(void *pvParameters);
//���Ϳ�������
static void vTaskHandShake(void *pvParameters);


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

	/* ��������ͨ�Ż��� */
	AppObjCreate();

	/* �������� */
	AppTaskCreate();
    
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

    //����LED����
    xTaskCreate((TaskFunction_t )vTaskLed,         
                (const char*    )"vTaskLed",       
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )LED_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskLed);                   

    //��ѯ���״̬
    xTaskCreate((TaskFunction_t )vTaskQueryMotor,
                (const char*    )"vQueryMotor",       
                (uint16_t       )QUERYMOTOR_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )QUERYMOTOR_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskQueryMotor);  
    
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

    #ifdef USEQUEUE
    //����߳�
    xTaskCreate((TaskFunction_t )vTaskMonitor,     
                (const char*    )"vTaskMonitor",   
                (uint16_t       )MONITOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MONITOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMonitor);
    #endif

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

    //������ֵ�ź���
//    gBinarySem_Handle = xSemaphoreCreateBinary();

//    if(gBinarySem_Handle == NULL)
//    {
//        App_Printf("������ֵ�ź���ʧ��\r\n");
//    }

    #ifdef USEQUEUE
    /* ������Ϣ���� */
    xTransQueue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
                              (UBaseType_t ) sizeof(QUEUE_TO_HOST_T *));/* ��Ϣ�Ĵ�С */
    if(xTransQueue == NULL)
    {
        App_Printf("����xTransQueue��Ϣ����ʧ��!\r\n");
    }	
    #endif


    /* ������Ϣ���� */
    xTransKeyQueue = xQueueCreate((UBaseType_t ) KEY_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t )KEY_QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
    if(xTransKeyQueue == NULL)
    {
        App_Printf("xTransKeyQueue error!\r\n");
    }	

    xTransMotorBQueue = xQueueCreate((UBaseType_t ) KEY_QUEUE_LEN,/* ��Ϣ���еĳ��� */
                            (UBaseType_t )KEY_QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
    if(xTransMotorBQueue == NULL)
    {
        App_Printf("xTransKeyQueue error!\r\n");
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

            //ʱ��ԭ��ֵ��̫׼ȷ����Ҫ����׼�ķ���

//            if((uxBits & TASK_BIT_0) != 0x01)
//            {
//                DBG("BIT_0 vTaskLed error = %02x,%02x   %02x \r\n",(uxBits & TASK_BIT_0),uxBits,TASK_BIT_0);
//            }

//            if((uxBits & TASK_BIT_1) != 0x02)
//            {
//                DBG("BIT_1 vTaskMotorToHost error = %02x, %02x   %02x \r\n",(uxBits & TASK_BIT_1),uxBits,TASK_BIT_1);
//            }

//            if((uxBits & TASK_BIT_2) != 0x04)
//            {
//                DBG("BIT_2 vTaskMsgPro error = %02x, %02x   %02x \r\n",(uxBits & TASK_BIT_2),uxBits,TASK_BIT_2);
//            }
//            
//            if((uxBits & TASK_BIT_3) != 0x08)
//            {
//                DBG("BIT_3 vTaskInfrared error = %02x, %02x   %02x \r\n",(uxBits & TASK_BIT_3),uxBits,TASK_BIT_3);
//            }

//            if((uxBits & TASK_BIT_4) != 0x10)
//            {
//                DBG("BIT_4 vTaskReader error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_4),uxBits,TASK_BIT_4);
//            }

//            if((uxBits & TASK_BIT_5) != 0x20)
//            {
//                DBG("BIT_5 vTaskQR error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_5),uxBits,TASK_BIT_5);
//            }       

//            if((uxBits & TASK_BIT_6) != 0x40)
//            {
//                DBG("BIT_6 vTaskRs485 error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_6),uxBits,TASK_BIT_6);
//            }  

//            if((uxBits & TASK_BIT_7) != 0x80)
//            {
//                DBG("BIT_7 vTaskKey error = %02x,%02x   ,%02x \r\n",(uxBits & TASK_BIT_7),uxBits,TASK_BIT_7);
//            } 

//            if((uxBits & (TASK_BIT_8>>8)) != 0x01)
//            {
//                DBG("BIT_8 vTaskQueryMotor error = %04x,%02x   ,%02x \r\n",(uxBits & (TASK_BIT_8>>8)),uxBits,TASK_BIT_8);
//            } 

            
		}
    }
}







//��ѯ���״̬
void vTaskQueryMotor(void *pvParameters)
{
    uint8_t ReadStatus[8] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
//    BaseType_t xReturn = pdPASS;
    
    while(1)
    {
//        xReturn = xSemaphoreTake(gBinarySem_Handle,portMAX_DELAY); 

//        if(xReturn == pdFALSE)
//        {
//            comSendBuf(COM4, ReadStatus,8);//��ѯA���״̬
            RS485_SendBuf(COM4,ReadStatus,8);
            RS485_SendBuf(COM5,ReadStatus,8);//��ѯB���״̬
           
//        }
     
		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_8);  
        vTaskDelay(500);     
    }

} 

//LED������ 
void vTaskLed(void *pvParameters)
{   
    //uint8_t pcWriteBuffer[512];
    
    uint8_t i = 0;
    BEEP = 0;
    vTaskDelay(300);
    BEEP = 1;
    
    while(1)
    {  
//        if(g500usCount == 0)
//        {
//            g500usCount = 1*60*1000;//30ms

//            App_Printf("\r\n=================================================\r\n");
//            App_Printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
//            vTaskList((char *)&pcWriteBuffer);
//            App_Printf("%s\r\n", pcWriteBuffer);

//            App_Printf("\r\n������       ���м���         ʹ����\r\n");
//            vTaskGetRunTimeStats((char *)&pcWriteBuffer);
//            App_Printf("%s\r\n", pcWriteBuffer);      
//        }
        
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
    
    BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
    uint32_t motora = 55;

    

    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptMotorToHost;  
    ptMotorToHost = &gQueueToHost;
    #endif
    
    while (1)
    {   
        readLen = RS485_Recv(COM4,buf,8); 

        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            { 
                #ifdef USEQUEUE
                ptMotorToHost->cmd = CONTROLMOTOR;
                memcpy(ptMotorToHost->data,buf,readLen);

    			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
    			if(xQueueSend(xTransQueue,              /* ��Ϣ���о�� */
    						 (void *) &ptMotorToHost,   /* ���ͽṹ��ָ�����ptQueueToHost�ĵ�ַ */
    						 (TickType_t)10) != pdPASS )
    			{
                    DBG("��xTransQueue��������ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ���\r\n");                
                } 
                else
                {
//                    DBG("��xTransQueue�������ݳɹ�\r\n");
                      dbh("CONTROLMOTOR",(char *)buf,readLen);
                }    
                #endif

                if(readLen == 8 && mau_key >= 55)
                {
                    xReturn = xQueueSend( xTransKeyQueue, /* ��Ϣ���еľ�� */
                                        &motora,/* ���͵���Ϣ���� */
                                        (TickType_t)30 );        /* �ȴ�ʱ�� 0 */    
                     if(xReturn != pdPASS)
                     {
                        DBG("queue is full,reset\r\n");
                        xQueueReset(xTransKeyQueue);
                     }
                     else
                     {
                        DBG("door A  return\r\n");
                        check_msg_queue();
                     }                     
                }               
                                      

//                dbh("RECV A",(char *)buf,readLen);
                send_to_host(CONTROLMOTOR,buf,readLen);
                vTaskResume(xHandleTaskQueryMotor);//����״̬��ѯ�߳�
                Motro_A = 0;
            }            
        }           

        
        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
        
        vTaskDelay(100);
    }

}

#if 1
void vTaskKey(void *pvParameters)
{
    uint8_t CloseDoor[8] = { 0x01,0x06,0x08,0x0C,0x00,0x01,0x8A,0x69 };
    uint8_t OpenDoor[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x02,0xCA,0x68 };
//    uint8_t OpenDoor_R[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x03,0x0B,0xA8 };
//    uint8_t QuestStatus[8] =  { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
//    uint8_t MotorReset[8] =  { 0x01,0x06,0x08,0x0C,0x00,0x07,0x0A,0x6B };    
	uint8_t ucKeyCode;

    BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
    uint32_t r_queue; /* ����һ��������Ϣ�ı��� */
    uint8_t i = 0;

    uint8_t t1 = 0;
    uint8_t t2 = 0;

    static uint8_t isOpen = 1;
    
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
                case KEY_FIREFIGHTING_PRES:
//                    vTaskSuspend(xHandleTaskQueryMotor); //���ǲ����������ص������ѯ 
                    mau_key = 100;
                    i = 0;
                    t1 = 0;
                    t2 =2;
                    do
                    {                           
                        xReturn = xQueueReceive( xTransKeyQueue,    /* ��Ϣ���еľ�� */
                                                 &r_queue,      /* ���͵���Ϣ���� */
                                                 (TickType_t)50); /* �ȴ�ʱ�� һֱ�� */
                        if(pdTRUE == xReturn&& i>0)
                        {
                            DBG("A��%d�ν��� = %d\r\n",i,r_queue);
                            if(r_queue == 55)
                            {
                                t1 = 1;
                            }
                        }  
                        else
                        {
                            if(isOpen)
                            {
                                RS485_SendBuf(COM4,OpenDoor,8);//��A��  
                            }
                            else
                            {
                                RS485_SendBuf(COM4,CloseDoor,8);//��A�� 
                            }
                        }
                        
                        xReturn = xQueueReceive( xTransMotorBQueue,    /* ��Ϣ���еľ�� */
                                                    &r_queue,      /* ���͵���Ϣ���� */
                                                    (TickType_t)50); /* �ȴ�ʱ�� һֱ�� */
                        if(pdTRUE == xReturn&& i>1)
                        {
                            DBG("B��%d�ν��� = %d\r\n",i,r_queue);
                            if(r_queue == 66)
                            {
                                t2 = 1;
                            }
                        }
                        else
                        {
                            if(isOpen)
                            {
                                RS485_SendBuf(COM5,OpenDoor,8);//��A��  
                            }
                            else
                            {
                                RS485_SendBuf(COM5,CloseDoor,8);//��A�� 
                            }
                        }

                        vTaskDelay(100);

                        if(t1==1 && t2==1)
                        {
                            mau_key = 0;
                            i=20;
                        }

                       check_msg_queue();
                    }
                    while (i++ < 8);
                    isOpen = !isOpen;
                    DBG("isOpen = %d\r\n",isOpen);                   
                    xQueueReset(xTransKeyQueue);
                    xQueueReset(xTransMotorBQueue);
                    //��android����������������Ϣ
                    SendAsciiCodeToHost(FIREFIGHTINGLINKAGE,NO_ERR,"Fire fighting linkage");                    
                    break;
                case KEY_OPEN_DOOR_A_PRES:
//                    vTaskSuspend(xHandleTaskQueryMotor); //���ǲ����������ص������ѯ
                    //Open door a manually
                    mau_key = 55;
                    i = 0;
                    do
                    {  
                        xReturn = xQueueReceive( xTransKeyQueue,    /* ��Ϣ���еľ�� */
                                                 &r_queue,      /* ���͵���Ϣ���� */
                                                 (TickType_t)50); /* �ȴ�ʱ�� һֱ�� */
                        if(pdTRUE == xReturn&& i!=0)
                        {
                            DBG("��%d�ν��� = %d\r\n",i,r_queue);
                            if(r_queue == 55)
                            {
                                
                                mau_key = 0;
                                i = 20;
                            }
                        }  
                        else
                        {
                            RS485_SendBuf(COM4,OpenDoor,8);//��A��                             
                            DBG("��A��\r\n");
                            vTaskDelay(100);   
                        }

                        check_msg_queue();
                    }
                    while (i++ < 8);                
                    xQueueReset(xTransKeyQueue);
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_A,NO_ERR,"Open door A manually"); 
                    break;
                case KEY_OPEN_DOOR_B_PRES:
//                    vTaskSuspend(xHandleTaskQueryMotor); //���ǲ����������ص������ѯ
                    //Open door b manually
                    mau_key = 66;
                    i= 0;
                    do
                    {                        
                        xReturn = xQueueReceive( xTransMotorBQueue,    /* ��Ϣ���еľ�� */
                                                 &r_queue,      /* ���͵���Ϣ���� */
                                                 (TickType_t)50); /* �ȴ�ʱ�� һֱ�� */
                        if(pdTRUE == xReturn && i!=0)
                        {
                            DBG("��%d�ν��� = %d\r\n",i,r_queue);
                            if(r_queue == 66)
                            {
                                
                                mau_key = 0;
                                i=20;
                            }
                        }
                        else
                        {
                            RS485_SendBuf(COM5,OpenDoor,8);//��B�� 
                            DBG("��B��\r\n");
                            vTaskDelay(100);

                        }

                        check_msg_queue();
                    }
                    while (i++ < 8); 
										xQueueReset(xTransMotorBQueue);
                    SendAsciiCodeToHost(MANUALLY_OPEN_DOOR_B,NO_ERR,"Open door B manually");
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
#else

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

#endif

void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {          
        deal_Serial_Parse();    
	    deal_rx_data();

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_2);
        vTaskDelay(100);
    }
}


void vTaskInfrared(void *pvParameters)
{
    uint32_t code = 0;
    uint8_t dat[3] = {0};
    
    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptInfraredToHost; 
    ptInfraredToHost = &gQueueToHost;
    #endif
    
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

            #ifdef USEQUEUE
            ptInfraredToHost->cmd = GETSENSOR;
            memcpy(ptInfraredToHost->data,dat,3);
            
			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
			if(xQueueSend(xTransQueue,              /* ��Ϣ���о�� */
						 (void *) &ptInfraredToHost,   /* ���ͽṹ��ָ�����ptQueueToHost�ĵ�ַ */
						 (TickType_t)10) != pdPASS )
			{
                DBG("��xTransQueue��������ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ���\r\n");                
            } 
            else
            {
                //DBG("��xTransQueue�������ݳɹ�\r\n");   
				dbh("GETSENSOR",(char *)dat,3);
            }
            #endif
                
            send_to_host(GETSENSOR,dat,3);
        }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_3);    
        
        vTaskDelay(20);
    }
}

void vTaskRs485(void *pvParameters)
{
    uint8_t buf[8] = {0};
    uint8_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};

    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptInfraredToHost; 
    ptInfraredToHost = &gQueueToHost;
    #endif

    BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
    uint32_t motora = 66;    
    
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
                #ifdef USEQUEUE
                ptInfraredToHost->cmd = DOOR_B;
                memcpy(ptInfraredToHost->data,buf,readLen);
            
    			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
    			if(xQueueSend(xTransQueue,              /* ��Ϣ���о�� */
    						 (void *) &ptInfraredToHost,   /* ���ͽṹ��ָ�����ptQueueToHost�ĵ�ַ */
    						 (TickType_t)10) != pdPASS )
    			{
                    DBG("��xTransQueue��������ʧ�ܣ���ʹ�ȴ���10��ʱ�ӽ���\r\n");                
                } 
                else
                {                 
    				dbh("DOOR_B",(char *)buf,readLen);
                }   
                #endif

                if(readLen == 8 && mau_key >= 66 )
                {
                    xReturn = xQueueSend( xTransMotorBQueue, /* ��Ϣ���еľ�� */
                                        &motora,/* ���͵���Ϣ���� */
                                        50 );        /* �ȴ�ʱ�� 0 */    
                     if(xReturn != pdPASS)
                     {
                        DBG("queue is full,reset\r\n");
                        xQueueReset(xTransMotorBQueue);
                     }
                     else
                     {
                        DBG("door B  return\r\n");
                        check_msg_queue();
                     }
                }                  
            
                send_to_host(DOOR_B,buf,readLen);
                vTaskResume(xHandleTaskQueryMotor);//����״̬��ѯ�߳�
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
    
//    uint32_t FunState = 0;
//    char *IcReaderState;
    #ifdef USEQUEUE
    QUEUE_TO_HOST_T *ptReaderToHost; 
    ptReaderToHost = &gQueueToHost;
    #endif
//    IcReaderState = ef_get_env("ICSTATE");
//    assert_param(IcReaderState);
//    FunState = atol(IcReaderState);
    
    while(1)
    {

//        if(FunState != 0x00)
        {
            CardID = bsp_WeiGenScanf();

            if(CardID != 0)
            {
                memset(dat,0x00,sizeof(dat));            
                
    			dat[0] = CardID>>24;
    			dat[1] = CardID>>16;
    			dat[2] = CardID>>8;
    			dat[3] = CardID&0XFF;    

                #ifdef USEQUEUE
                ptReaderToHost->cmd = WGREADER;
                memcpy(ptReaderToHost->data,dat,4);

    			/* ʹ����Ϣ����ʵ��ָ������Ĵ��� */
    			if(xQueueSend(xTransQueue,              /* ��Ϣ���о�� */
    						 (void *) &ptReaderToHost,   /* ���ͽṹ��ָ�����ptQueueToHost�ĵ�ַ */
    						 (TickType_t)100) != pdPASS )
    			{
                    DBG("��xTransQueue��������ʧ�ܣ���ʹ�ȴ���100��ʱ�ӽ���\r\n");                
                } 
                else
                {
                    dbh("WGREADER",(char *)dat,4);
                }
                #endif
                
                send_to_host(WGREADER,dat,4);
            }  
        }


		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_4);        
        
        vTaskDelay(100);
        
    }

}   


void vTaskQR(void *pvParameters)
{ 
    uint8_t recv_buf[512] = {0};
    uint16_t len = 0;  
    
//    uint32_t FunState = 0;
//    char *QrCodeState;

//    QrCodeState = ef_get_env("QRSTATE");
//    assert_param(QrCodeState);
//    FunState = atol(QrCodeState);
    
    while(1)
    {
//       if(FunState != 0x00)
       {
           memset(recv_buf,0x00,sizeof(recv_buf));
           len = comRecvBuff(COM3,recv_buf,sizeof(recv_buf));

           if(len > 0  && recv_buf[len-1] == 0x0A && recv_buf[len-2] == 0x0D)
           {
                DBG("QR = %s\r\n",recv_buf);
                SendAsciiCodeToHost(QRREADER,NO_ERR,recv_buf);
           }
           else
           {
                comClearTxFifo(COM3);
           }
       }

		/* �����¼���־����ʾ������������ */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
        vTaskDelay(500);        
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

//    g500usCount = 1000*10;

//    c_old_boot_times = ef_get_env("boot_times");

//    DBG("1.c_old_boot_times = %s\r\n",c_old_boot_times);

//    while(1)
//    {
//        if(g500usCount == 0)
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
    char  buf_str[512 + 1];
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



#ifdef USEQUEUE
static void vTaskMonitor(void *pvParameters)
{
  BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
  QUEUE_TO_HOST_T *ptMsg;
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* �������ȴ�ʱ��Ϊ200ms */  

  while (1)
  {
    xReturn = xQueueReceive( xTransQueue,    /* ��Ϣ���еľ�� */
                             (void *)&ptMsg,  /*�����ȡ���ǽṹ��ĵ�ַ */
                             xMaxBlockTime); /* ��������ʱ�� */
    if(pdPASS == xReturn)
    {
//        DBG("ptMsg->cmd = %02x\r\n", ptMsg->cmd);
//        dbh("ptMsg->data ", (char *)ptMsg->data,QUEUE_BUF_LEN);

        switch (ptMsg->cmd)
        {
            case GETSENSOR:
                 DBG("��������\r\n");
                break;
             case CONTROLMOTOR:
                 DBG("A�ŵ������\r\n");
                break;
            case DOOR_B:
                 DBG("B�ŵ������\r\n");
                break;
            case WGREADER:
                 DBG("����������\r\n");
                break;            
            
        }
    }    

  }    
}
#endif

//��ѯMessage_Queue�����е��ܶ���������ʣ���������
static void check_msg_queue(void)
{
    
	u8 msgq_remain_size;	//��Ϣ����ʣ���С
    u8 msgq_total_size;     //��Ϣ�����ܴ�С
    
    taskENTER_CRITICAL();   //�����ٽ���
    msgq_remain_size=uxQueueSpacesAvailable(xTransKeyQueue);//�õ�����ʣ���С
    msgq_total_size=uxQueueMessagesWaiting(xTransKeyQueue)+uxQueueSpacesAvailable(xTransKeyQueue);//�õ������ܴ�С���ܴ�С=ʹ��+ʣ��ġ�
	DBG("Total Size = %d, Remain Size = %d\r\n",msgq_total_size,msgq_remain_size);	//��ʾDATA_Msg��Ϣ�����ܵĴ�С

    taskEXIT_CRITICAL();    //�˳��ٽ���
}





