#ifndef _MPU9250_H
#define _MPU9250_H

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

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (100)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)
//    
#define q30  1073741824.0f
#define SENSOR_MAX_G 2.0f       //constant g        // tobe fixed to 8g. but IMU need to  correct at the same time  
#define SENSOR_MAX_W 2000.0f    //deg/s  
#define ACC_SCALE  (SENSOR_MAX_G/32768.0f)  
#define GYRO_SCALE  (SENSOR_MAX_W/32768.0f)

#define  Pitch_error  1.0
#define  Roll_error   -2.0
#define  Yaw_error    0.0
extern float Pitch,Roll,Yaw;
extern float temp;
extern float acc[3];

void Get_Gesture(void);
void set(void);


#endif