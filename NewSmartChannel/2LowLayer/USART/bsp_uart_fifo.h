/*
*********************************************************************************************************
*
*	ģ������ : �����ж�+FIFO����ģ��
*	�ļ����� : bsp_uart_fifo.h
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2015-2020, 
*
*********************************************************************************************************
*/

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_
#include "string.h"	
#include "stm32f4xx_conf.h"


#define  USE_FreeRTOS      1

#if USE_FreeRTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
	#define DISABLE_INT()    taskENTER_CRITICAL()
	#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
	/* ����ȫ���жϵĺ� */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */
#endif



/*
	�����Ҫ���Ĵ��ڶ�Ӧ�Ĺܽţ��������޸� bsp_uart_fifo.c�ļ��е� static void InitHardUart(void)����
*/

/* ����ʹ�ܵĴ���, 0 ��ʾ��ʹ�ܣ������Ӵ����С���� 1��ʾʹ�� */
#define	UART1_FIFO_EN	1
#define	UART2_FIFO_EN	0
#define	UART3_FIFO_EN	0
#define	UART4_FIFO_EN	0
#define	UART5_FIFO_EN	0
#define	UART6_FIFO_EN	0

/* RS485оƬ����ʹ��GPIO, PG0 */
//#define RCC_RS485_TXEN 	    RCC_AHB1Periph_GPIOG
//#define PORT_RS485_TXEN     GPIOG
//#define PIN_RS485_TXEN	    GPIO_Pin_0

//#define RS485_RX_EN()	PORT_RS485_TXEN->BSRRH = PIN_RS485_TXEN
//#define RS485_TX_EN()	PORT_RS485_TXEN->BSRRL = PIN_RS485_TXEN




/* ����˿ں� */
typedef enum
{
	COM1 = 0,	/* USART1  PB6, PB7*/
	COM2 = 1,	/* USART2, PD5,PD6 �� PA2, PA3 */
	COM3 = 2,	/* USART3, PB10, PB11 */
	COM4 = 3,	/* UART4, PC10, PC11 */
	COM5 = 4,	/* UART5, PC12, PD2 */
	COM6 = 5	/* USART6, PC6, PC7 */
}COM_PORT_E;

/* ���崮�ڲ����ʺ�FIFO��������С����Ϊ���ͻ������ͽ��ջ�����, ֧��ȫ˫�� */
#if UART1_FIFO_EN == 1
	#define UART1_BAUD			38400
	#define UART1_TX_BUF_SIZE	2*1024
	#define UART1_RX_BUF_SIZE	1*1024
#endif

#if UART2_FIFO_EN == 1
	#define UART2_BAUD			9600
	#define UART2_TX_BUF_SIZE	1
	#define UART2_RX_BUF_SIZE	2*1024
#endif

#if UART3_FIFO_EN == 1
	#define UART3_BAUD			9600
	#define UART3_TX_BUF_SIZE	1*1024
	#define UART3_RX_BUF_SIZE	1*1024
#endif

#if UART4_FIFO_EN == 1
	#define UART4_BAUD			115200
	#define UART4_TX_BUF_SIZE	1*1024
	#define UART4_RX_BUF_SIZE	1*1024
#endif

#if UART5_FIFO_EN == 1
	#define UART5_BAUD			115200
	#define UART5_TX_BUF_SIZE	1*1024
	#define UART5_RX_BUF_SIZE	1*1024
#endif

#if UART6_FIFO_EN == 1
	#define UART6_BAUD			115200
	#define UART6_TX_BUF_SIZE	1*1024
	#define UART6_RX_BUF_SIZE	1*1024
#endif

/* �����豸�ṹ�� */
typedef struct
{
	USART_TypeDef *uart;		/* STM32�ڲ������豸ָ�� */
	uint8_t *pTxBuf;			/* ���ͻ����� */
	uint8_t *pRxBuf;			/* ���ջ����� */
	uint16_t usTxBufSize;		/* ���ͻ�������С */
	uint16_t usRxBufSize;		/* ���ջ�������С */
	__IO uint16_t usTxWrite;	/* ���ͻ�����дָ�� */
	__IO uint16_t usTxRead;		/* ���ͻ�������ָ�� */
	__IO uint16_t usTxCount;	/* �ȴ����͵����ݸ��� */

	__IO uint16_t usRxWrite;	/* ���ջ�����дָ�� */
	__IO uint16_t usRxRead;		/* ���ջ�������ָ�� */
	__IO uint16_t usRxCount;	/* ��δ��ȡ�������ݸ��� */

	void (*SendBefor)(void); 	/* ��ʼ����֮ǰ�Ļص�����ָ�루��Ҫ����RS485�л�������ģʽ�� */
	void (*SendOver)(void); 	/* ������ϵĻص�����ָ�루��Ҫ����RS485������ģʽ�л�Ϊ����ģʽ�� */
	void (*ReciveNew)(uint8_t _byte);	/* �����յ����ݵĻص�����ָ�� */
}UART_T;

void bsp_InitUart(void);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);
void USART_SetBaudRate(USART_TypeDef* USARTx, uint32_t BaudRate);

//void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
//void RS485_SendStr(char *_pBuf);
//void RS485_SetBaud(uint32_t _baud);

#endif


