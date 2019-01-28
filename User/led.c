#include "led.h"


void Led_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GSM_LED_OK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GSM_LED_OK_PORT, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = MPU_LED_ABNORMAL_PIN;
//	GPIO_Init(MPU_LED_ABNORMAL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GSM_LED_FAIL_PIN;
	GPIO_Init(GSM_LED_FAIL_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);
	
	GPIO_SetBits(GSM_LED_FAIL_PORT,GSM_LED_FAIL_PIN);
  GPIO_SetBits(GSM_LED_OK_PORT,GSM_LED_OK_PIN);
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN);

//	GPIO_SetBits(MPU_LED_ABNORMAL_PORT,MPU_LED_ABNORMAL_PIN);
//	GPIO_SetBits(MPU_LED_NORMAL_PORT,MPU_LED_NORMAL_PIN);
}

void BEEP(u32 time)
{
	  OS_ERR err;
	  uint8_t i = 0;
	 //  for(i = 0;i < 10;i++)
	// {	
			GPIO_SetBits(BEEP_PORT,BEEP_PIN);
	    OSTimeDly(time,OS_OPT_TIME_DLY,&err);
	    GPIO_ResetBits(BEEP_PORT,BEEP_PIN);
			OSTimeDly(time,OS_OPT_TIME_DLY,&err);
	// }
		}

	

void GSM_OK()
{
	GPIO_SetBits(GSM_LED_FAIL_PORT,GSM_LED_FAIL_PIN);
	GPIO_ResetBits(GSM_LED_OK_PORT,GSM_LED_OK_PIN);
}

void GSM_FAIL()
{
	GPIO_SetBits(GSM_LED_OK_PORT,GSM_LED_OK_PIN);
	GPIO_ResetBits(GSM_LED_FAIL_PORT,GSM_LED_FAIL_PIN);
}

//void Body_Normal()
//{
//	GPIO_SetBits(MPU_LED_ABNORMAL_PORT,MPU_LED_ABNORMAL_PIN);
//}


//void Body_Abnormal()
//{
//	GPIO_ResetBits(MPU_LED_ABNORMAL_PORT,MPU_LED_ABNORMAL_PIN);
//}
