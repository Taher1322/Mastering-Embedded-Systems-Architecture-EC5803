/*************************************************************************
*
* File name : main.c
* Description : Contains main function
* Author: Gaurang Krishnakant Rane/ Taher Ujjainwala
* Tools : mbed, Keil
*
**************************************************************************/

//Defining Header files 
#include "mbed.h"
#include "MMA8451Q.h"

//Defining variable to calculate time elapsed 
float timer_val;

//Object for timer
Timer t;

 //Defining the Accerlerometer Pin outs
#if   defined (TARGET_KL25Z) || defined (TARGET_KL46Z)
  PinName const SDA = PTE25;
  PinName const SCL = PTE24;
#elif defined (TARGET_KL05Z)
  PinName const SDA = PTB4;
  PinName const SCL = PTB3;
#elif defined (TARGET_K20D50M)
  PinName const SDA = PTB1;
  PinName const SCL = PTB0;
#else
  #error TARGET NOT DEFINED
#endif

//I2C address of Accelerometer
#define MMA8451_I2C_ADDRESS (0x1d<<1)



int main(void)
{
		t.start();        //Starting the timer
    MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);    //Initializing the Acccelerometer
    //Defining Leds as PWM out pins for control
	  PwmOut rled(LED1); 
    PwmOut gled(LED2);
    PwmOut bled(LED3);


    while (true) {
				//Variables to read X, Y and Z axis
        float x, y, z;
        
				//Reading the absolute values of Accelerometer - Values on one motion is positive and same axis opposite motion is negative 
			  //To control the Led either way axis - Absolute values are taken
        x = abs(acc.getAccX());
        y = abs(acc.getAccY());
        z = abs(acc.getAccZ());
			
				//Controling the led's
        rled = 1.0f - x;
        gled = 1.0f - y;
        bled = 1.0f - z;
        wait(0.1f);
			
				//Printing the Accelerometer values for debugging and understanding
        printf("Accelerometer  Values: \n\r"); 
        printf("X: %f\r\n", x);
        printf("Y: %f\r\n", x);
        printf("Z: %f\r\n", x);
			
				//Reading the timer value to calculate CPU cycle
				timer_val = t.read();
				t.stop();
    }
}
