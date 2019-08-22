/******************************************************************************

                  版权所有 (C), 2013-2023, 深圳博思高科技有限公司

 ******************************************************************************
  文 件 名   : main.c
  版 本 号   : 初稿
  作    者   : 张舵
  生成日期   : 2019年7月9日
  最近修改   :
  功能描述   : 主程序模块
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月9日
    作    者   : 张舵
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "def.h"

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/
//任务优先级   


#define LED_TASK_PRIO	    ( tskIDLE_PRIORITY)

#define HANDSHAKE_TASK_PRIO	( tskIDLE_PRIORITY)


#define READER_TASK_PRIO	( tskIDLE_PRIORITY + 1)
#define QR_TASK_PRIO	    ( tskIDLE_PRIORITY + 1)

#define INFRARED_TASK_PRIO	( tskIDLE_PRIORITY + 4)

#define CMD_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define MOTOR_TASK_PRIO		( tskIDLE_PRIORITY + 3)
#define RS485_TASK_PRIO	    ( tskIDLE_PRIORITY + 3)

#define START_TASK_PRIO		( tskIDLE_PRIORITY + 4)


//任务堆栈大小    
#define LED_STK_SIZE 		128
#define MOTOR_STK_SIZE 		512 
#define CMD_STK_SIZE 		1024*2
#define INFRARED_STK_SIZE 	512
#define RS485_STK_SIZE 		1024*1
#define START_STK_SIZE 	    128
#define QR_STK_SIZE 		512
#define READER_STK_SIZE     512
#define HANDSHAKE_STK_SIZE  256




//事件标志
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
 * 模块级变量                                   *
 *----------------------------------------------*/
//任务句柄
static TaskHandle_t xHandleTaskLed = NULL;      //LED灯
static TaskHandle_t xHandleTaskMotor = NULL;    //电机控制
static TaskHandle_t xHandleTaskCmd = NULL;      //android通讯
static TaskHandle_t xHandleTaskInfrared = NULL; //红外感映
static TaskHandle_t xHandleTaskReader = NULL;   //韦根读卡器
static TaskHandle_t xHandleTaskQr = NULL;       //二维码读头
static TaskHandle_t xHandleTaskRs485 = NULL;
static TaskHandle_t xHandleTaskStart = NULL;    //看门狗
static TaskHandle_t xHandleTaskHandShake = NULL;    //看门狗


static EventGroupHandle_t xCreatedEventGroup = NULL;



/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

//任务函数
static void vTaskLed(void *pvParameters);
static void vTaskMortorToHost(void *pvParameters);
//static void vTaskKey(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskInfrared(void *pvParameters);
static void vTaskRs485(void *pvParameters);
static void vTaskReader(void *pvParameters);
static void vTaskQR(void *pvParameters);
static void vTaskStart(void *pvParameters);

//上送开机次数
static void vTaskHandShake(void *pvParameters);


static void AppTaskCreate(void);
static void AppObjCreate (void);
static void App_Printf(char *format, ...);
//static void AppEventCreate (void);


int main(void)
{    
    //硬件初始化
    bsp_Init();  

    //记录开机次数
    RecordBootTimes();
    
	/* 创建任务 */
	AppTaskCreate();

	/* 创建任务通信机制 */
	AppObjCreate();

    
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();
    
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    //跟android握手
    xTaskCreate((TaskFunction_t )vTaskHandShake,
                (const char*    )"vHandShake",       
                (uint16_t       )HANDSHAKE_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )HANDSHAKE_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskHandShake);  

    //创建LED任务
    xTaskCreate((TaskFunction_t )vTaskLed,         
                (const char*    )"vTaskLed",       
                (uint16_t       )LED_STK_SIZE, 
                (void*          )NULL,              
                (UBaseType_t    )LED_TASK_PRIO,    
                (TaskHandle_t*  )&xHandleTaskLed);   

    //创建电机信息返回任务
    xTaskCreate((TaskFunction_t )vTaskMortorToHost,     
                (const char*    )"vTMTHost",   
                (uint16_t       )MOTOR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MOTOR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskMotor);     

    //跟android通讯串口数据解析
    xTaskCreate((TaskFunction_t )vTaskMsgPro,     
                (const char*    )"cmd",   
                (uint16_t       )CMD_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )CMD_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskCmd);      

    //红外传感器状态上送
    xTaskCreate((TaskFunction_t )vTaskInfrared,     
                (const char*    )"vTaskInfrared",   
                (uint16_t       )INFRARED_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )INFRARED_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskInfrared);    


    //485任务
    xTaskCreate((TaskFunction_t )vTaskRs485,     
                (const char*    )"vTaskRs485",   
                (uint16_t       )RS485_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )RS485_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskRs485);  

    //韦根读卡器
    xTaskCreate((TaskFunction_t )vTaskReader,     
                (const char*    )"vTaskReader",   
                (uint16_t       )READER_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )READER_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskReader);    

    //二维码扫码模块
    xTaskCreate((TaskFunction_t )vTaskQR,     
                (const char*    )"vTaskQR",   
                (uint16_t       )QR_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )QR_TASK_PRIO,
                (TaskHandle_t*  )&xHandleTaskQr);                  

    //看门狗
	xTaskCreate((TaskFunction_t )vTaskStart,     		/* 任务函数  */
                (const char*    )"vTaskStart",   		/* 任务名    */
                (uint16_t       )START_STK_SIZE,        /* 任务栈大小，单位word，也就是4字节 */
                (void*          )NULL,           		/* 任务参数  */
                (UBaseType_t    )START_TASK_PRIO,       /* 任务优先级*/
                (TaskHandle_t*  )&xHandleTaskStart );   /* 任务句柄  */                

}


/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate (void)
{
	/* 创建事件标志组 */
	xCreatedEventGroup = xEventGroupCreate();
	
	if(xCreatedEventGroup == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        App_Printf("创建事件标志组失败\r\n");
    }

	/* 创建互斥信号量 */
    gxMutex = xSemaphoreCreateMutex();
	
	if(gxMutex == NULL)
    {
        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
        App_Printf("创建互斥信号量失败\r\n");
    }    

}

/*
*********************************************************************************************************
*	函 数 名: vTaskStart
*	功能说明: 启动任务，等待所有任务发事件标志过来。
*	形    参: pvParameters 是在创建该任务时传递的形参
*	返 回 值: 无
*   优 先 级: 4  
*********************************************************************************************************
*/
static void vTaskStart(void *pvParameters)
{
	EventBits_t uxBits;
	const TickType_t xTicksToWait = 100 / portTICK_PERIOD_MS; /* 最大延迟1000ms */
	
	/* 
	  开始执行启动任务主函数前使能独立看门狗。
	  设置LSI是32分频，下面函数参数范围0-0xFFF，分别代表最小值1ms和最大值4095ms
	  下面设置的是4s，如果4s内没有喂狗，系统复位。
	*/
	bsp_InitIwdg(4000);
	
	/* 打印系统开机状态，方便查看系统是否复位 */
	App_Printf("=====================================================\r\n");
	App_Printf("=系统开机执行\r\n");
	App_Printf("=====================================================\r\n");
	
    while(1)
    {        
		/* 等待所有任务发来事件标志 */
		uxBits = xEventGroupWaitBits(xCreatedEventGroup, /* 事件标志组句柄 */
							         TASK_BIT_ALL,       /* 等待TASK_BIT_ALL被设置 */
							         pdTRUE,             /* 退出前TASK_BIT_ALL被清除，这里是TASK_BIT_ALL都被设置才表示“退出”*/
							         pdTRUE,             /* 设置为pdTRUE表示等待TASK_BIT_ALL都被设置*/
							         xTicksToWait); 	 /* 等待延迟时间 */
		
		if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
		{            
		    IWDG_Feed(); //喂狗			
		}
	    else
		{
			/* 基本是每xTicksToWait进来一次 */
			/* 通过变量uxBits简单的可以在此处检测那个任务长期没有发来运行标志 */
		}
    }
}



//LED任务函数 
void vTaskLed(void *pvParameters)
{
    while(1)
    {

        LED1=!LED1;  
        LED2=!LED2; 
        LED3=!LED3; 
        LED4=!LED4; 
        
		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_0);        

        vTaskDelay(300);
        
    }

}   

//motor to host 任务函数
void vTaskMortorToHost(void *pvParameters)
{
//    uint8_t buf[8] = {0};
//    uint8_t readLen = 0;
//    uint16_t iCRC = 0;
//    uint8_t crcBuf[2] = {0};
//    while (1)
//    {       
//        if(bsp_Usart4_RecvOne(buf) == 1)
//        {  
//            
//           if(bsp_Usart4_RecvOne(buf+1) == 1)
//           {    
//                if(buf[1] == 0x03)//读取状态
//                {
//                   readLen=bsp_Usart4_RecvAtTime(buf + 2, 5, 20);                   
//                }
//                else if(buf[1] == 0x06)//读取执行状态
//                {
//                    readLen=bsp_Usart4_RecvAtTime(buf + 2, 6, 20);
//                }                

//                if(readLen == 5 || readLen == 6) //接收到数据才上送
//                {
//                    iCRC = CRC16_Modbus(buf, readLen);  

//                    crcBuf[0] = iCRC >> 8;
//                    crcBuf[1] = iCRC & 0xff;  

//                    if(crcBuf[1] == buf[readLen] && crcBuf[0] == buf[readLen+1])
//                    {    
//                        send_to_host(CONTROLMOTOR,buf,readLen+2);
//                    }
//                }
//           }
//        }
//        
//		/* 发送事件标志，表示任务正常运行 */        
//		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
//        
//        vTaskDelay(10);
//    }

    uint8_t buf[8] = {0};
    uint8_t readLen = 0;
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
            }            
        }
            
//        if(comGetChar(COM4, buf) == 1)
//        {              
//           if(comGetChar(COM4, buf+1) == 1)
//           {    
//                if(buf[1] == 0x03)//读取状态
//                {
//                   readLen=comRecvBuff(COM4,buf + 2, 5);                   
//                }
//                else if(buf[1] == 0x06)//读取执行状态
//                {
//                    readLen=comRecvBuff(COM4,buf + 2, 6);
//                }                

//                if(readLen == 5 || readLen == 6) //接收到数据才上送
//                {
//                    iCRC = CRC16_Modbus(buf, readLen);  

//                    crcBuf[0] = iCRC >> 8;
//                    crcBuf[1] = iCRC & 0xff;  

//                    if(crcBuf[1] == buf[readLen] && crcBuf[0] == buf[readLen+1])
//                    {    
//                        send_to_host(CONTROLMOTOR,buf,readLen+2);
//                    }
//                }
//           }
//        }

        
        /* 发送事件标志，表示任务正常运行 */        
        xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_1);
        
        vTaskDelay(100);
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
				/* K1键按下 打印任务执行情况 */
				case KEY_SET_PRES:	

					App_Printf("=================================================\r\n");
					App_Printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
					vTaskList((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);
                    
					App_Printf("\r\n任务名       运行计数         使用率\r\n");
					vTaskGetRunTimeStats((char *)&pcWriteBuffer);
					App_Printf("%s\r\n", pcWriteBuffer);                    
					break;				
				/* K2键按下，打印串口操作命令 */
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
				
				/* 其他的键值不处理 */
				default:   
				App_Printf("KEY_default\r\n");
					break;
			}
		}

        /* 发送事件标志，表示任务正常运行 */
//		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_2);
		
		vTaskDelay(20);
	}   

}
#endif




void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {          
        deal_Serial_Parse();    
	    deal_rx_data();

		/* 发送事件标志，表示任务正常运行 */        
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
            send_to_host(GETSENSOR,dat,3);
        }

		/* 发送事件标志，表示任务正常运行 */        
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
//        if(comGetChar(COM5, buf) == 1)
//        {    
//           if(comGetChar(COM5, buf+1) == 1)
//           {
//                if(buf[1] == 0x03)//读取状态
//                {                              
//                   readLen=comRecvBuff(COM5,buf + 2, 5);                   
//                }
//                else if(buf[1] == 0x06)//读取执行状态
//                {                               
//                    readLen=comRecvBuff(COM5,buf + 2, 6);
//                }   
//				
//                if(readLen == 5 || readLen == 6) //接收到数据才上送
//                {
//                    iCRC = CRC16_Modbus(buf, readLen);  

//                    crcBuf[0] = iCRC >> 8;
//                    crcBuf[1] = iCRC & 0xff;  

//                    if(crcBuf[1] == buf[readLen] && crcBuf[0] == buf[readLen+1])
//                    {    
//                        send_to_host(DOOR_B,buf,readLen+2);
//                    }
//                }
//           }
//        }

        readLen = RS485_Recv(COM5,buf,8);       

        if(readLen == 7 || readLen == 8)
        {            
            iCRC = CRC16_Modbus(buf, readLen-2);  

            crcBuf[0] = iCRC >> 8;
            crcBuf[1] = iCRC & 0xff;  

            if(crcBuf[1] == buf[readLen-2] && crcBuf[0] == buf[readLen-1])
            {    
                send_to_host(DOOR_B,buf,readLen);
            }            
        }

        
		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_6);
        
        vTaskDelay(100);
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


		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_4);        
        
        vTaskDelay(20);
        
    }

}   


void vTaskQR(void *pvParameters)
{ 
    uint8_t recv_buf[245] = {0};
    uint8_t dat[256] = {0};
    uint16_t len = 0;
    
//    while(1)
//    {
//       memset(recv_buf,0x00,sizeof(recv_buf));  
//       memset(dat,0x00,sizeof(dat));  
//       len = bsp_Usart3_RecvAtTime(recv_buf,100,2000);

//       if(len > 0  && recv_buf[len-1] == 0x0A && recv_buf[len-2] == 0x0D)
//       {
//            asc2bcd(dat, recv_buf, len, 0);
//            
//            send_to_host(QRREADER,dat,ceil(len/2));
//       }

//		/* 发送事件标志，表示任务正常运行 */        
//		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
//        vTaskDelay(10);        
//    }

    while(1)
    {
       memset(recv_buf,0x00,sizeof(recv_buf));  
       memset(dat,0x00,sizeof(dat));  
       len = comRecvBuff(COM3,recv_buf,sizeof(recv_buf));

       if(len > 0  && recv_buf[len-1] == 0x0A && recv_buf[len-2] == 0x0D)
       {
            asc2bcd(dat, recv_buf, len, 0);
            
            send_to_host(QRREADER,dat,ceil(len/2));
       }

		/* 发送事件标志，表示任务正常运行 */        
		xEventGroupSetBits(xCreatedEventGroup, TASK_BIT_5);  
        vTaskDelay(300);        
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

    /* interger to string */
    sprintf(c_new_boot_times,"%012ld", i_boot_times);

    asc2bcd(bcdbuf,(uint8_t *)c_new_boot_times , 12, 0);

    send_to_host(HANDSHAKE,bcdbuf,6);

    vTaskDelete( NULL ); //删除自己

}



/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
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

	/* 互斥信号量 */
	xSemaphoreTake(gxMutex, portMAX_DELAY);

    printf("%s", buf_str);

   	xSemaphoreGive(gxMutex);
}




