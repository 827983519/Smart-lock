/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>

#define beep              0x01
#define gsm               0x01
#define gsm1              0x01

/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/
OS_FLAG_GRP  flag_grp; 
OS_FLAG_GRP  beep_grp; 
OS_FLAG_GRP  gsm_grp; 
OS_TMR my_tmr;
/*
*********************************************************************************************************
*                                                 TCB
*********************************************************************************************************
*/

static  OS_TCB   AppTaskStartTCB;

static OS_TCB   AppTaskMPUTCB;
static OS_TCB   AppTaskLEDTCB;
OS_TCB   AppTaskBLETCB;
static OS_TCB   AppTaskLOCKTCB;
static OS_TCB   AppTaskGSMTCB;
static OS_TCB   AppTaskBEEPTCB;
static OS_TCB   AppTaskCHECKTCB;
//static OS_TCB   AppTaskMESSAGETCB;

/*
*********************************************************************************************************
*                                                STACKS
*********************************************************************************************************
*/

static  CPU_STK  AppTaskStartStk[APP_TASK_START_STK_SIZE];

 
__align(8) static  CPU_STK  AppTaskMPUStk [APP_TASK_MPU_STK_SIZE];
static CPU_STK AppTaskLEDStk[APP_TASK_LED_STK_SIZE];
static CPU_STK AppTaskBLEStk[APP_TASK_BLE_STK_SIZE];
static CPU_STK AppTaskLOCKStk[APP_TASK_LOCK_STK_SIZE];
static CPU_STK AppTaskGSMStk[APP_TASK_GSM_STK_SIZE];
static CPU_STK AppTaskBEEPStk[APP_TASK_BEEP_STK_SIZE];
static CPU_STK AppTaskCHECKStk[APP_TASK_CHECK_STK_SIZE];
//static CPU_STK AppTaskMESSAGEStk[APP_TASK_MESSAGE_STK_SIZE];

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);
static  void  AppTaskMPU ( void * p_arg );
static  void  AppTaskLED ( void * p_arg );
static  void  AppTaskCHECK( void * p_arg); 
static  void  AppTaskBLE ( void * p_arg );
static  void  AppTaskLOCK( void * p_arg );
static  void  AppTaskGSM ( void * p_arg );
static  void  AppTaskBEEP ( void * p_arg ); 


void TmrCallback (OS_TMR *p_tmr, void *p_arg);
/*
*********************************************************************************************************
*                                      main()
*
* Description : This is the standard entry point for C code.  It is assumed that your code will call
*               main() once you have performed all necessary initialization.
*
* Arguments   : none
*
* Returns     : none
*********************************************************************************************************
*/
extern char r[60];
u8 lock = 1;             //lock,0 for unlock
u8 alarm = 0;            //beep
u8 notification = 0;     //gsm
u8 mode = 0;             //message or call
u8 notification1 = 1;    //gsm,1 for already Alert
u8 delay_minute = 60; 
u8 count = 0;      //timer
u8 sensitivity = 1;
u8 phone_number[13];
int  main (void)
{
    OS_ERR  err;
    OSInit(&err);                                               /* Init uC/OS-III.                                      */
    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_START_PRIO,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_START_STK_SIZE,
                 (OS_MSG_QTY  ) 20u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);

    OSStart(&err);                                              /* Start multitasking (i.e. give control to uC/OS-III). */
		
		
}


static  void  AppTaskStart (void *p_arg)
{

    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;


   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */
    CPU_Init();
    
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;        /* Determine nbr SysTick increments                     */
    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    Mem_Init();                                                 /* Initialize Memory Management Module                  */
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

    CPU_IntDisMeasMaxCurReset();
    
	   OSFlagCreate ((OS_FLAG_GRP  *)&flag_grp,        //指向事件标志组的指针
							     (CPU_CHAR     *)"FLAG For GSM",  //事件标志组的名字
						       (OS_FLAGS      )0,                //事件标志组的初始值
				     		   (OS_ERR       *)&err);					  //返回错误类型
								
		 OSFlagCreate ((OS_FLAG_GRP  *)&beep_grp,        //指向事件标志组的指针
							     (CPU_CHAR     *)"FLAG For beep",  //事件标志组的名字
							     (OS_FLAGS      )0,                //事件标志组的初始值
									 (OS_ERR       *)&err);						
     OSFlagCreate ((OS_FLAG_GRP  *)&gsm_grp,        //指向事件标志组的指针
							     (CPU_CHAR     *)"FLAG For gsm",  //事件标志组的名字
							     (OS_FLAGS      )0,                //事件标志组的初始值
									 (OS_ERR       *)&err);																	 
	   OSTmrCreate ((OS_TMR              *)&my_tmr,             //软件定时器对象
									 (CPU_CHAR            *)"MySoftTimer",       //命名软件定时器
									 (OS_TICK              )60,                  //定时器初始值,频率为1HZ
									 (OS_TICK              )0,                  //定时器周期重载值
									 (OS_OPT               )OS_OPT_TMR_ONE_SHOT, //周期性定时
									 (OS_TMR_CALLBACK_PTR  )TmrCallback,         //回调函数
									 (void                *)0,       //传递实参给回调函数
									 (OS_ERR              *)&err);               
							 
  	OSTaskCreate((OS_TCB     *)&AppTaskLEDTCB,                                             
                 (CPU_CHAR   *)"App Task LED",
                 (OS_TASK_PTR ) AppTaskLED,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_LED_PRIO,
                 (CPU_STK    *)&AppTaskLEDStk[0],
                 (CPU_STK_SIZE) APP_TASK_LED_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LED_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
    OSTaskCreate((OS_TCB     *)&AppTaskMPUTCB,               
                 (CPU_CHAR   *)"App Task MPU",
                 (OS_TASK_PTR ) AppTaskMPU,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_MPU_PRIO,
                 (CPU_STK    *)&AppTaskMPUStk[0],
                 (CPU_STK_SIZE) APP_TASK_MPU_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_MPU_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
		OSTaskCreate((OS_TCB     *)&AppTaskCHECKTCB,               
                 (CPU_CHAR   *)"App Task CHECK",
                 (OS_TASK_PTR ) AppTaskCHECK,
                 (void       *) 0,
                 (OS_PRIO     ) APP_TASK_CHECK_PRIO,
                 (CPU_STK    *)&AppTaskCHECKStk[0],
                 (CPU_STK_SIZE) APP_TASK_CHECK_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_CHECK_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
	  OSTaskCreate((OS_TCB     *)&AppTaskBLETCB,               
                 (CPU_CHAR   *)"App Task BLE",
                 (OS_TASK_PTR ) AppTaskBLE,
                 (void       *) 0,
									 (OS_PRIO     ) APP_TASK_BLE_PRIO,
                 (CPU_STK    *)&AppTaskBLEStk[0],
                 (CPU_STK_SIZE) APP_TASK_BLE_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_BLE_STK_SIZE,
                 (OS_MSG_QTY  ) 20u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
	
								 
	 OSTaskCreate((OS_TCB     *)&AppTaskLOCKTCB,               
                 (CPU_CHAR   *)"App Task LOCK",
                 (OS_TASK_PTR ) AppTaskLOCK,
                 (void       *) 0,
									 (OS_PRIO     ) APP_TASK_LOCK_PRIO,
                 (CPU_STK    *)&AppTaskLOCKStk[0],
                 (CPU_STK_SIZE) APP_TASK_LOCK_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_LOCK_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
							
    OSTaskCreate((OS_TCB     *)&AppTaskGSMTCB,               
                 (CPU_CHAR   *)"App Task GSM",
                 (OS_TASK_PTR ) AppTaskGSM,
                 (void       *) 0,
									 (OS_PRIO     ) APP_TASK_GSM_PRIO,
                 (CPU_STK    *)&AppTaskGSMStk[0],
                 (CPU_STK_SIZE) APP_TASK_GSM_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_GSM_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
						
		OSTaskCreate((OS_TCB     *)&AppTaskBEEPTCB,               
                 (CPU_CHAR   *)"App Task BEEP",
                 (OS_TASK_PTR ) AppTaskBEEP,
                 (void       *) 0,
									 (OS_PRIO     ) APP_TASK_BEEP_PRIO,
                 (CPU_STK    *)&AppTaskBEEPStk[0],
                 (CPU_STK_SIZE) APP_TASK_BEEP_STK_SIZE / 10,
                 (CPU_STK_SIZE) APP_TASK_BEEP_STK_SIZE,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
								 
//    OSTaskCreate((OS_TCB     *)&AppTaskMESSAGETCB,               
//                 (CPU_CHAR   *)"App Task MESSAGE",
//                 (OS_TASK_PTR ) AppTaskMESSAGE,
//                 (void       *) 0,
//                 (OS_PRIO     ) APP_TASK_MESSAGE_PRIO,
//                 (CPU_STK    *)&AppTaskMESSAGEStk[0],
//                 (CPU_STK_SIZE) APP_TASK_MESSAGE_STK_SIZE / 10,
//                 (CPU_STK_SIZE) APP_TASK_MESSAGE_STK_SIZE,
//                 (OS_MSG_QTY  ) 0u,
//                 (OS_TICK     ) 0u,
//                 (void       *) 0,
//                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
//                 (OS_ERR     *)&err);
			
		OSTaskDel ( & AppTaskStartTCB, & err );
		
}

void TmrCallback (OS_TMR *p_tmr, void *p_arg) //软件定时器MyTmr的回调函数
{
	  OS_ERR      err; 
	  count++;
	  if(count < delay_minute)
    {
			OSTmrStart ((OS_TMR   *)&my_tmr, 
                     (OS_ERR   *)err);
		} 
    else		
	  {
			notification1 = 1;
			count = 0;
			Usart_SendString(USART3,"GSM function on");
		}
}


static  void  AppTaskMPU ( void * p_arg )
{
    OS_ERR      err;
    CPU_SR_ALLOC();
    (void)p_arg;
    while (1) {                                          /* Task body, always written as an infinite loop.       */  
			OS_CRITICAL_ENTER();
			Get_Gesture();
		  //printf("Roll:%d    ",abs(Roll));	
		 // printf("Roll:%f    ",Roll);	
			OS_CRITICAL_EXIT();
       //	printf(" %d    ",GPIO_ReadInputDataBit(Hall_Port,Hall_Pin));		
		  OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
    }				
}

static void  AppTaskCHECK( void * p_arg)
{
	  OS_ERR    err;
	  u8 i = 0,j = 0;
	  u8 message[30];
    (void)p_arg;
	   
	while(1)
{
   j = Check_Message();
	if(j)
	{
		if(strstr(r,"beep")!= NULL)
		{
			GPIO_SetBits(BEEP_PORT,BEEP_PIN);
	  }
		if(strstr(r,"stop")!= NULL)
		{
			GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
	  }
		if(strstr(r,"Attitude")!= NULL)
		{
			sprintf(message,"Pitch:%.1f Roll:%.1f",Pitch,Roll);
			Send_Message(phone_number,message);
	  }
		clean();
	}
	
if(lock == 1)
	{
		 if((sensitivity == 1 && abs(Roll) > 60) || (sensitivity == 2 && abs(Roll) > 35) || (sensitivity == 3 && abs(Roll) > 50))
		 {  
			 if(alarm == 1)
			{

				OSFlagPost ((OS_FLAG_GRP  *)&beep_grp,                             
                  (OS_FLAGS      )beep,
                  (OS_OPT        )OS_OPT_POST_FLAG_SET,
                  (OS_ERR       *)&err);
			}
			
					  
			if(notification == 1 && notification1 == 1)
			{
		   OSFlagPost ((OS_FLAG_GRP  *)&gsm_grp,                             
                  (OS_FLAGS      )gsm1,
                  (OS_OPT        )OS_OPT_POST_FLAG_SET,
                  (OS_ERR       *)&err);            
			  
				 notification1 = 0;
        Usart_SendString(USART3,"Timer begin");				
			}
		}
		}
	if(lock == 0 && GPIO_ReadInputDataBit(Hall_Port,Hall_Pin) == 0)
	{
		  i++;
		  if(i == 40)
			{
				lock = 1;
				i = 0;
				printf("Lock!!");
			}
	}

		  OSTimeDly(100,OS_OPT_TIME_DLY,&err); 	
		}
	}


	
	
static  void  AppTaskLED ( void * p_arg )
{
	OS_ERR err;
	uint8_t i = 0;
  (void)p_arg;
	OSTimeDly(2000,OS_OPT_TIME_DLY,&err);  
	if(GSM_cmd("AT+CNMI=2,2,0,0,0\r\n", "OK",500))
			//	printf("MME");
	while(1){
		
  	i = Check_Gsm(500,1);
	  
		if(i == 1)
		{
			
			OSFlagPost ((OS_FLAG_GRP  *)&flag_grp,                             
                  (OS_FLAGS      )gsm,
                  (OS_OPT        )OS_OPT_POST_FLAG_SET,
                  (OS_ERR       *)&err);
			OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
      			
			//OSTaskSuspend ( 0, & err );           
		}
		else
		{		
			//GSM_FAIL();
			Usart_SendString(USART3,"Can't detect GSM");
			//reset();
			OSTimeDly(3000,OS_OPT_TIME_DLY,&err); 
		}
}
	}

static void  AppTaskBLE ( void * p_arg )
	{
	 OS_ERR err;
	 u8 *command;
	 char i = 0;
	 static u8 phone = 0;
	 OS_MSG_SIZE  size;
	 CPU_SR_ALLOC();
	 (void)p_arg;
	 
	 while(1)
	 {
		 
		command = OSTaskQPend ((OS_TICK        )0,                    
								(OS_OPT         )OS_OPT_PEND_BLOCKING, 
								(OS_MSG_SIZE   *)&size,           
								(CPU_TS        *)0,                 
								(OS_ERR        *)&err);
    
		 if(command[0] == 'A' && command[1] == 'T' && (command[3] == '\0' || command[3] == ' '))
		 {
			 switch(command[2])
			 {
				 case '1':
				 {
	 	  OSTaskSemPost((OS_TCB  *)&AppTaskLOCKTCB,                             
										(OS_OPT   )OS_OPT_POST_NONE,                             
										(OS_ERR  *)&err);
				//Usart_SendString(USART3,"Unlock now");
					 break;
				 }
				 
				 case '2':
				 {
					 if(phone == 1)
						 notification = 1 - notification;
					 else
						 Usart_SendString(USART3,"Phone number unset");
					 OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
					 if(notification == 1)
					 {
						 Usart_SendString(USART3,"Reminder on");
						 count = 0;
						 OSTmrStop ((OS_TMR   *)&my_tmr,
                        OS_OPT_TMR_NONE,
                        NULL,
                        (OS_ERR *) &err);                    
					 }
					 else
					 {
					 Usart_SendString(USART3,"Reminder off");
					 }
					 break;
				 }
				 case '3':
				 {
					 alarm = 1 - alarm;
					 if(alarm  == 1)
					 Usart_SendString(USART3,"Alarm on");
					 else
					 Usart_SendString(USART3,"Alarm off");
					 break;
				 }
				 
				 case '4':
				 {
					 mode = 1 - mode;
					 if(mode == 0)
				   Usart_SendString(USART3,"Call mode");
					 else
					 Usart_SendString(USART3,"Message mode");
					 break;
				 }
				 
				 case '5':
				 {
					 if(command[3] == ' ')
					 {
						 if(command[6] != '\0')
							 Usart_SendString(USART3,"The number should be 1-99");
             else if(command[5] != '\0')
						 {
							 delay_minute = (command[4] - 48) * 10 + (command[5] - 48);
						   printf("delay minute:%d",delay_minute);
						 }
             else if(command[4] != '\0')
						 {
  						 delay_minute = command[4] - 48;
						   printf("delay minute:%d",delay_minute);
						 }
						 else
							 printf("Wrong command");
					 }
					 else
						Usart_SendString(USART3,"Wrong command"); 
					 
					 break;
				 }
				 
				 	 case '6':
				 {
					  sensitivity++;
				  	if(sensitivity == 4)
							sensitivity = 1;
						printf("Sensitivity:%d",sensitivity);
					 break;
				 }
				 
				 	 case '7':
				 {
						printf("delay:%d",delay_minute);
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					  printf("sensitivity:%d",sensitivity);
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 if(alarm  == 1)
					 Usart_SendString(USART3,"Alarm on");
					 else
					 Usart_SendString(USART3,"Alarm off");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
           if(mode == 0)
				   Usart_SendString(USART3,"Call mode");
					 else
					 Usart_SendString(USART3,"Message mode");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 if(notification == 1)
					 Usart_SendString(USART3,"Reminder on");
					 else
					 Usart_SendString(USART3,"Reminder off");
					  OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 if(notification1 == 1)
					 Usart_SendString(USART3,"GSM work");
					 else
					 Usart_SendString(USART3,"Intervals");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 printf("Phone:%s",phone_number);
					 break;
				 }
				 
				  case '8':
				 {
					 if(command[3] == ' ')
				 {
					 for(i = 4;i < 15;i++)
					 {
						 if(command[i] < '0' || command[i] > '9')
							 break;
					 }
					 if(i == 15)
					 {
						 phone = 1;
						 phone_number[11] = '\r';
						 phone_number[12] = '\n';
  						for(i = 4;i < 15;i++)
						   phone_number[i - 4] = command[i];
						  Usart_SendString(USART3,"Phone number set"); 
					 }
					 else
						Usart_SendString(USART3,"Phone number Wrong"); 
				 }
				    else
						Usart_SendString(USART3,"Wrong command"); 
					 break;		 
			  }
					case '9':
					{
						printf("%d",abs(Roll));
						break;
					}
				  	 case '\0':
				 {
					 printf("AT1:Unlock");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
				   printf("AT2:Reminder on/off");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 printf("AT3:Alarm on/off");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
           printf("AT4:Message/Call");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 printf("AT5:Set interval");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 printf("AT6:Set sensitivity");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 printf("AT7:Print info");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 printf("AT8:Set phone number");
					 OSTimeDly(50,OS_OPT_TIME_DLY,&err);
					 
					 break;
				 }
				 default:
				 {
					  Usart_SendString(USART3,"Wrong command");
					 break;
				 }
			 }	  
      }
	   else
		 {printf("Wrong command!");}
		 for(i = 0;i < 15;i++)
		  command[i] = '\0';
		OSTimeDly(100,OS_OPT_TIME_DLY,&err); 
	 }
 }
	

	static  void  AppTaskLOCK( void * p_arg )
	{
	  OS_ERR err;
	  (void)p_arg;
		
		while(1)
		{
				OSTaskSemPend ((OS_TICK   )0,                     
									 (OS_OPT    )OS_OPT_PEND_BLOCKING,  
									 (CPU_TS   *)0,                 
									 (OS_ERR   *)&err);  
         OSTaskSuspend ( &AppTaskLEDTCB, & err ); 
         Unlock();
			  // printf("Unlock function");
			   lock = 0;
			   OSTimeDly(5000,OS_OPT_TIME_DLY,&err);
         Lock();
	    	 OSTimeDly(1000,OS_OPT_TIME_DLY,&err);
         OSTaskResume ( &AppTaskLEDTCB, & err ); 			
        // printf("Lock function");			
		}
	}
	
	
static  void  AppTaskGSM ( void * p_arg )
	{
		OS_ERR err;
		u8 i = 0;
		(void)p_arg;
		OSFlagPend ((OS_FLAG_GRP *)&flag_grp,                  
                (OS_FLAGS     )gsm,
                (OS_TICK      )0,
                (OS_OPT       )OS_OPT_PEND_FLAG_SET_ALL|OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING,
                (CPU_TS      *)0,
                (OS_ERR      *)&err);	
		Usart_SendString(USART3,"GSM OK");
		while(1)
		{
			OSFlagPend ((OS_FLAG_GRP *)&gsm_grp,                  
                (OS_FLAGS     )gsm1,
                (OS_TICK      )0,
                (OS_OPT       )OS_OPT_PEND_FLAG_SET_ALL|OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING,
                (CPU_TS      *)0,
                (OS_ERR      *)&err);	
			   if(mode == 0)
				 {
					i =  Call(phone_number , 3000);
					 if(i)
					 { 
						 Usart_SendString( USART3, "Sucessful call");
						  OSTmrStart ((OS_TMR   *)&my_tmr, 
                     (OS_ERR   *)err);   
				
					 }
	         else
           {
						 Usart_SendString( USART3, "Can't call");
						 notification1 = 1;
					 }		    
				 }
						
				 if(mode == 1)	
				 {
					  Send_Message(phone_number,"The bike is abnormal");
					  OSTmrStart ((OS_TMR   *)&my_tmr, 
                     (OS_ERR   *)err);   
						OSTimeDly(1000,OS_OPT_TIME_DLY,&err); 
	
				 }		
					}
			}

			
	static void  AppTaskBEEP ( void * p_arg )
	{
	 OS_ERR err;
	 (void)p_arg;
		
		while(1)
		{	
			OSFlagPend ((OS_FLAG_GRP *)&beep_grp,                  
                (OS_FLAGS     )beep,
                (OS_TICK      )0,
                (OS_OPT       )OS_OPT_PEND_FLAG_SET_ALL|OS_OPT_PEND_FLAG_CONSUME|OS_OPT_PEND_BLOCKING,
                (CPU_TS      *)0,
                (OS_ERR      *)&err);	
   
			BEEP(300);
   	//  Usart_SendString(USART3,"Beep function");
		}
	}
	
