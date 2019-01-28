#include "gsm.h"
#include "string.h"
#include "led.h"
#include "includes.h"

extern int num;
extern char r[60];

//unsigned char *phone = "17130849536\r\n";


static void Nvic_Config()
{  
	 NVIC_InitTypeDef NVIC_Initstructure;
	
	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); 
   NVIC_Initstructure.NVIC_IRQChannel =USART2_IRQn ;
   NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0; 	
	 NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE ;
	 NVIC_Init(&NVIC_Initstructure);
	
    }
	



void GSM_Config()
{ 
	  USART_InitTypeDef USART_InitStructure;    
  NVIC_InitTypeDef NVIC_InitStructure;     
    GPIO_InitTypeDef GPIO_InitStructure;    //?????????,?????GPIO    
   //?????RCC??    
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); //??UART3??GPIOB???    
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    
    
   //?????GPIO???    
   // Configure USART2 Rx (PB.11) as input floating      
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
   // Configure USART2 Tx (PB.10) as alternate function push-pull    
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    
    
   //????    
   USART_InitStructure.USART_BaudRate = 9600;    
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
   USART_InitStructure.USART_StopBits = USART_StopBits_1;    
   USART_InitStructure.USART_Parity = USART_Parity_No;    
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    
    
    
   // Configure USART3     
   USART_Init(USART3, &USART_InitStructure);//????3    
    
  // Enable USART1 Receive interrupts ????????    
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    
   //??????????????    
   USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);    
    
   // Enable the USART3     
   USART_Cmd(USART3, ENABLE);//????3    
	 USART_ClearFlag(USART3, USART_FLAG_TC);
    
   //??????    
   //Configure the NVIC Preemption Priority Bits       
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);    
    
   // Enable the USART3 Interrupt     
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;    
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 	    
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
   NVIC_Init(&NVIC_InitStructure);
     }

		 
		 
	void Usart_SendByte(USART_TypeDef* USARTx, uint8_t data)             
	{
	
		 USART_SendData(USARTx,data);
	  while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);      

		 
   }
	
				
	void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str)
{
	unsigned int k=0;
    do 
    {
        Usart_SendByte( pUSARTx, *(str + k) );
        k++;
    } while(*(str + k)!='\0');
}
			
/**********************************************/

void reset()
{
	 OS_ERR err;
	GPIO_ResetBits(SLEEP_PORT,SLEEP_PIN);
	 OSTimeDly(1,OS_OPT_TIME_DLY,&err); 
	GPIO_SetBits(SLEEP_PORT,SLEEP_PIN);
}

void sleep()
{
	GPIO_ResetBits(SLEEP_PORT,SLEEP_PIN);
}

void wakeup()
{
	GPIO_SetBits(SLEEP_PORT,SLEEP_PIN);
}

void clean(void)
{
    uint16_t i = 60;
	  while(i)
		  r[i--]=0;
	    num = 0;
}

 uint8_t GSM_cmd(char *cmd, char *reply,uint32_t waittime )
{    
	  CPU_SR_ALLOC();
	  OS_ERR err;
	  int i = 0;
    clean();
    OS_CRITICAL_ENTER();	
    Usart_SendString (USART1,cmd);
    OS_CRITICAL_EXIT();	
    if(reply == 0)                      
    {
        return 1;
    }	
	  OSTimeDly(waittime,OS_OPT_TIME_DLY,&err);		
		if(strstr(r,reply)!= NULL)
		{clean();
	    return 1;}
	   else
		 {clean();	 
			return 0;
     }			 
}

uint8_t Check_Gsm(uint32_t wait_time, uint32_t repeat_time)
{
	int i = 0;
	uint8_t a = 0;
	a = GSM_cmd("AT\r\n","OK",wait_time);
	while((a == 0) && (i < repeat_time))
	 {
		 a = GSM_cmd("AT\r\n","OK",wait_time);
		 i++;
	 }
	 if(a != 1)
    return 0;
	 else
		return 1;
}

void Set_Text_Mode(void)
{
	GSM_cmd("ATE0\r\n","OK",500);	
	GSM_cmd("AT+CMGF=1\r\n","OK",500);
}

void Send_Text_Sms(char *phone,char *content)
{
 char temp[30]="AT+CMGS=";	
 strcat(temp,(char*)phone); 
	while(GSM_cmd(temp,">",500)!=1);
	Usart_SendString(USART1,content);     //发送短信内容
	Usart_SendByte(USART1 ,0X1A);  //发送结束符
	Usart_SendString(USART1,"\r\n");
}


u8 Call(char *phone , uint32_t wait_time)
{
   char temp[30]="ATD";
   char i;	
   strcat(temp,(char*)phone);
	 i = GSM_cmd(temp,"OK",wait_time);
	 return i;
	
}

void Send_Message(char *phone,char *content)
{
	  Set_Text_Mode();
		Send_Text_Sms(phone,content);
}

u8 Check_Message()
{
	OS_ERR err;
	if(num!=0)
	{
		OSTimeDly(100,OS_OPT_TIME_DLY,&err);
		if(strstr(r,"CMT")!= NULL)
		{
			return 1;
		}
		}
  return 0;	
}

int fputc(int ch, FILE *f)
{
	USART_SendData(USART3, (uint8_t) ch);

	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART3->SR & USART_FLAG_RXNE));

    return ((int)(USART3->DR & 0x1FF));
}


