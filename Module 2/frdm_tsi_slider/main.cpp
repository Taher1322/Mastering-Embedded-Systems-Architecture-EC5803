/*************************************************************************
*
* File name : main.c
* Description : Contains main function to read the Touch sensor value and change the brightness of LED
* Author: Gaurang Krishnakant Rane/ Taher Ujjainwala
* Tools : mbed, Keil
*
**************************************************************************/

#include "mbed.h"
#include "tsi_sensor.h"
 
 
Timer t;

/* This defines will be replaced by PinNames soon */
#if defined (TARGET_KL25Z) || defined (TARGET_KL46Z)
  #define ELEC0 9
  #define ELEC1 10
#elif defined (TARGET_KL05Z)
  #define ELEC0 9
  #define ELEC1 8
#else
  #error TARGET NOT DEFINED
#endif
 
Serial pc(USBTX, USBRX);
float analog_val=0;

int main(void) {
	//Starting the Timer
	 t.start();
    pc.printf("Hello World!\n");
		//Defining PWM for Green Led
    PwmOut led(LED_GREEN);
		
		// Defining Slider object
    TSIAnalogSlider tsi(ELEC0, ELEC1, 40);;
    
    while (true) {
				
				//Reading the Touch TSI value -- Ranges from 0 - 1
        analog_val = tsi.readPercentage();
        pc.printf("Touch value is %f\n", analog_val);
			
				//Controlling the brightness of the LED using the touch values
        led = 1.0 - tsi.readPercentage();
			
				//Delay for 100ms
        wait(0.1);
				//Reading the timer values since start
				float timer_val = t.read();
				t.stop();
    }
}
