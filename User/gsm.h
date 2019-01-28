#ifndef _GSM_H
#define _GSM_H

#include "stm32f10x.h"

#define  USART2_GPIO_CLK   RCC_APB2Periph_GPIOA  
#define  USART2_TX_PORT    GPIOA 
#define  USART2_TX_PIN     GPIO_Pin_2

#define  USART2_RX_PORT    GPIOA
#define  USART2_RX_PIN     GPIO_Pin_3

#define  RESET_PORT        GPIOB
#define  RESET_PIN         GPIO_Pin_9
#define  RESET_CLK         RCC_APB2Periph_GPIOB


#define  SLEEP_PORT        GPIOB
#define  SLEEP_PIN         GPIO_Pin_8

void GSM_Config(void);
uint8_t Check_Gsm(uint32_t wait_time, uint32_t repeat_time);
u8 Call(char *phone,uint32_t wait_time);
void Send_Text_Sms(char *phone,char *content);
void Set_Text_Mode(void);
uint8_t GSM_cmd(char *cmd, char *reply,uint32_t waittime );
void clean(void);
void GSM_Init(void);
void Send_Message(char *phone,char *content);
void sleep(void);
void wakeup(void);
void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
u8 Check_Message(void);

#endif