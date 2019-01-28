#ifndef _USART_H
#define _USART_H


#define  State_Port  GPIOA
#define  State_Pin   GPIO_Pin_3

void Usart_SendString( USART_TypeDef * pUSARTx, uint8_t *str);
void USART1_Config(void);
void usart_INIT(void);


void Usart_SendByte(USART_TypeDef* USARTx, uint8_t data);
#endif

