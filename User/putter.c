#include <putter.h>

void Putter_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = Putter_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(Putter_Port, &GPIO_InitStructure);
	GPIO_SetBits(Putter_Port,Putter_Pin);
	
	GPIO_InitStructure.GPIO_Pin = Hall_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(Hall_Port, &GPIO_InitStructure);
	GPIO_ResetBits(Putter_Port,Putter_Pin);

}

void Unlock()
{
	GPIO_SetBits(Putter_Port,Putter_Pin);
}

void Lock()
{
	GPIO_ResetBits(Putter_Port,Putter_Pin);
	
}