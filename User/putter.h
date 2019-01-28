#ifndef _PUTTER_H
#define _PUTTER_H
#include "stm32f10x.h"


#define Putter_Pin     GPIO_Pin_6
#define Putter_Port    GPIOA


#define Hall_Port      GPIOA
#define Hall_Pin       GPIO_Pin_5


void Putter_Init(void);
void Unlock(void);
void Lock(void);

#endif