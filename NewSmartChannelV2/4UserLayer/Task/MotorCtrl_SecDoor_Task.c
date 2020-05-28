/******************************************************************************

                  ��Ȩ���� (C), 2013-2023, ���ڲ�˼�߿Ƽ����޹�˾

 ******************************************************************************
  �� �� ��   : MotorCtrl_SecDoor_Task.c
  �� �� ��   : ����
  ��    ��   :  
  ��������   : 2020��4��28��
  ����޸�   :
  ��������   : �ڶ���������Ʋ���
  �����б�   :
  �޸���ʷ   :
  1.��    ��   : 2020��4��28��
    ��    ��   :  
    �޸�����   : �����ļ�

******************************************************************************/


/*----------------------------------------------*
 * ����ͷ�ļ�                                   *
 *----------------------------------------------*/
#include"MotorCtrl_SecDoor_Task.h"
#include "bsp_uart_fifo.h"
#include "comm.h"

/*----------------------------------------------*
 * �궨��                                       *
 *----------------------------------------------*/
#define MOTOR_SEC_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define MOTOR_SEC_STK_SIZE 		(configMINIMAL_STACK_SIZE*8)



/*----------------------------------------------*
 * ��������                                     *
 *----------------------------------------------*/
const char *MotorCtrlSecDoorTaskName = "vSecMotorTask";      

/*----------------------------------------------*
 * ģ�鼶����                                   *
 *----------------------------------------------*/
TaskHandle_t xHandleTaskMotorCtrlSecDoor = NULL;  

/*----------------------------------------------*
 * �ڲ�����ԭ��˵��                             *
 *----------------------------------------------*/
static void vTaskMotorCtrlSecDoor(void *pvParameters);





void CreateMotorCtrlSecDoorTask(void)
{
    //���������Ϣ��������
    xTaskCreate((TaskFunction_t )vTaskMotorCtrlSecDoor,     
                (const char*    )MotorCtrlSecDoorTaskName,   
                (uint16_t       )MOTOR_SEC_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_SEC_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotorCtrlSecDoor);     

}

static void vTaskMotorCtrlSecDoor(void *pvParameters)
{  
    BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
    uint32_t i = 0;
    uint8_t buf[8] = {0};
    uint16_t readLen = 0;
    uint16_t iCRC = 0;
    uint8_t crcBuf[2] = {0};

    MOTORCTRL_QUEUE_T *ptMotor; 
	/* ��ʼ���ṹ��ָ�� */
	ptMotor = &gSecMotorCtrlQueue;

    /* ���� */
    ptMotor->cmd = 0;
    memset(ptMotor->data,0x00,MOTORCTRL_QUEUE_BUF_LEN); 
      

    uint8_t ReadStatus[MOTORCTRL_QUEUE_BUF_LEN] = { 0x01,0x03,0x07,0x0C,0x00,0x01,0x45,0x7D };
    
    while (1)
    {   

        //��ȡ������ִ����λ��ָ���ȡ��������ִ��״̬��ѯ
        xReturn = xQueueReceive( gxMotorSecDoorCtrlQueue,    /* ��Ϣ���еľ�� */
                                 (void *)&ptMotor,          /*�����ȡ���ǽṹ��ĵ�ַ */
                                 (TickType_t)100);          /* ��������ʱ�� */
        if(pdTRUE == xReturn)
        {
            //��Ϣ���ճɹ������ͽ��յ�����Ϣ
            dbh("B Recv��",(char *)ptMotor->data, MOTORCTRL_QUEUE_BUF_LEN);
            RS485_SendBuf(COM5, ptMotor->data,MOTORCTRL_QUEUE_BUF_LEN);//����B���  
        }
        else
        {
            //����Ĭ�����ݰ�
            RS485_SendBuf(COM5, ReadStatus,MOTORCTRL_QUEUE_BUF_LEN);//��ѯB���״̬
        }                            

        vTaskDelay(50);
        
        readLen = RS485_Recv(COM5,buf,8);       
        
        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            {                                
                send_to_host(CONTROLMOTOR_B,buf,readLen);              
                Motro_B = 0;
            }
        } 
        else
        {
            if(i++ == READ_MOTOR_STATUS_TIMES)
            {
                i = 0;
                DBG("door b connect error!\r\n"); 
                SendAsciiCodeToHost(ERRORINFO,MOTOR_B_ERR,"Motor B fault");
            }
            
        }

        /* �����¼���־����ʾ������������ */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
      
    }

}





