#include "mpu9250.h"


float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
unsigned long sensor_timestamp;
short gyro[3], accel[3], sensors;
unsigned char more;
long quat[4];

//误差纠正

//NRF接收数据
u8 RxNrfBuf[32];
//NRF发送数据
u8 TxNrfBuf[32];

struct rx_s 
{
    unsigned char header[3];
    unsigned char cmd;
};


struct hal_s 
{
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned short report;
    unsigned short dmp_features;
    unsigned char motion_int_mode;
    struct rx_s rx;
};


static struct hal_s hal = {0};

volatile unsigned char rx_new;

static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};

float Pitch,Roll,Yaw;
float temp;
float acc[3];
                                          

void delay(int i)
{
	int j,k;
	for(j = 0;j < i ;j++)
		for(k = 0;k < 1000;k++);
}	





/*****************************************/

/*********************************************************************/

//声明相关变量


static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}



static  unsigned short inv_orientation_matrix_to_scalar(const signed char *mtx)
{
    unsigned short scalar;

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}

void set()
{
	//mpu_set_sensor
        if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))
        {
           
        }
      
        
        //mpu_configure_fifo
        if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))
        {
            //printf("mpu_configure_fifo complete ......\n");
        }
      
        
        //mpu_set_sample_rate
        if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))
        {
            //printf("mpu_set_sample_rate complete ......\n");
        }
      
        
        //dmp_load_motion_driver_firmvare
        if(!dmp_load_motion_driver_firmware())
        {
          
        }
        
        
        //dmp_set_orientation
        if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))
        {
          
        }
       
        //dmp_enable_feature
        if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
            DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
            DMP_FEATURE_GYRO_CAL))
        {
            
        }
      
        
        //dmp_set_fifo_rate
        if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))
        {
          
        }
    
        
        //不开自检，以水平作为零度
        //开启自检以当前位置作为零度
        //run_self_test();
        
        if(!mpu_set_dmp_state(1))
        {
          
        }
      
}



void Get_Gesture()
{
		    dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);			
        if (sensors & INV_WXYZ_QUAT )
       {
            q0 = quat[0] / q30;
            q1 = quat[1] / q30;
            q2 = quat[2] / q30;
            q3 = quat[3] / q30;
            
            Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3 + Pitch_error; // pitch
            Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3 + Roll_error; // roll
             Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3 + Yaw_error;
				    	acc[0]= (float)accel[0] *ACC_SCALE * 9.80665f ;       				
					    acc[1]= (float)accel[1] *ACC_SCALE * 9.80665f ;  				     
					    acc[2]= (float)accel[2] *ACC_SCALE * 9.80665f ;  
        }
	         
       
}

