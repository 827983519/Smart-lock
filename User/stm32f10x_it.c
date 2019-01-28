#include <includes.h>
#include "stm32f10x_it.h"
#include "gsm.h"
#include "usart.h"

char r[60];
int num = 0;
extern  OS_TCB   AppTaskBLETCB;

void USART1_IRQHandler(void)
{
  OS_ERR err;
	OSIntEnter();
	if(USART_GetITStatus (USART1, USART_IT_RXNE) != RESET)
	{
		r[num++] = USART_ReceiveData(USART1);
	//	if(num > 30)
	//		num = 8;
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
	OSIntExit();
}

void USART3_IRQHandler(void)
{
	OS_ERR err;
	static int num1;
	u8 clear;
  static u8 bluetooth[9];
	OSIntEnter();
	if(USART_GetITStatus (USART3, USART_IT_RXNE) != RESET)
	{

		bluetooth [num1++] = USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
	}
	if(USART_GetITStatus (USART3, USART_IT_IDLE) != RESET)
	{
		  clear = USART3->SR;
		  clear = USART3->DR;
		  OSTaskQPost ((OS_TCB      *)&AppTaskBLETCB,                
								 (void        *)bluetooth,             
								 (OS_MSG_SIZE  )num1,  
								 (OS_OPT       )OS_OPT_POST_FIFO,               
								 (OS_ERR      *)&err);                          
								 
		num1 = 0;

	}
	OSIntExit();
}