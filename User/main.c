#include "stm32f10x.h"
#include "delay.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "math.h"
#include "usart.h"
#include "stm32_iic.h"
#include "stdio.h"
#include "string.h"
#include "putter.h"
#include "mpu9250.h"


extern float Pitch,Roll,Yaw;

extern float acc[3];
                

int main(void)
{  
	 // Putter_Init();  
  ///  SystemInit();
   // USART1_Config();  //串口初始化
   // i2cInit();      //IIC总线的初始化
   // GSM_Init();
	  Led_Config();
   // mpu_init();
	 // set();
    while(1)
    {
      //GSM_OK();
//      Get_Gesture();	
//			printf("Pitch:%f    ",Pitch);
//			printf("Roll:%f    ",Roll);
//			printf("Yaw:%f \r\n",Yaw);
		}
}
