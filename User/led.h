#ifndef _LED_H
#define _LED_H

#include "stm32f10x.h"
#include "includes.h"

#define GSM_LED_OK_PORT           GPIOA
#define GSM_LED_OK_PIN            GPIO_Pin_4
//#define MPU_LED_NORMAL_PORT       GPIOA
//#define MPU_LED_NORMAL_PIN        GPIO_Pin_4

#define GSM_LED_FAIL_PORT         GPIOA
#define GSM_LED_FAIL_PIN          GPIO_Pin_8
//#define MPU_LED_ABNORMAL_PORT     GPIOA
//#define MPU_LED_ABNORMAL_PIN      GPIO_Pin_5

#define BEEP_PORT                 GPIOA
#define BEEP_PIN                  GPIO_Pin_7

void Led_Config(void);
void GSM_OK(void);
void GSM_FAIL(void);
void Body_Normal(void);
void Body_Abnormal(void);
void BEEP(u32 time);

#endif