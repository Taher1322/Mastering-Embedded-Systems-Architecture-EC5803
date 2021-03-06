/**----------------------------------------------------------------------------
 
   \file main.cpp
--                                                                           --
--              ECEN 5803 Mastering Embedded System Architecture             --
--                  Project 1 Module 4                                       --
--                Microcontroller Firmware                                   --
--                      main.cpp                                             --
--                                                                           --
-------------------------------------------------------------------------------
--
--  Designed for:  University of Colorado at Boulder
--               
--                
--  Designed by:  Tim Scherr
--  Revised by:  Student's name 
-- 
-- Version: 2.1
-- Date of current revision:  2017-09-25   
-- Target Microcontroller: Freescale MKL25ZVMT4 
-- Tools used:  ARM mbed compiler
--              ARM mbed SDK
--              Freescale FRDM-KL25Z Freedom Board
--               
-- 
-- Functional Description:  Main code file generated by mbed, and then
--                           modified to implement a super loop bare metal OS.
--
--      Copyright (c) 2015, 2016 Tim Scherr  All rights reserved.
--
*/

#define MAIN
#include "shared.h"
#undef MAIN

#define ADC_0                   (0U)
#define CHANNEL_0               (0U)
#define CHANNEL_1               (1U)
#define CHANNEL_2               (2U)
#define LED_ON                  (0U)
#define LED_OFF                 (1U)
#define ADCR_VDD                (65535U)    /*! Maximum value when use 16b resolution */
#define V_BG                    (1000U)     /*! BANDGAP voltage in mV (trim to 1.0V) */
#define V_TEMP25                (716U)      /*! Typical VTEMP25 in mV */
#define M                       (1620U)     /*! Typical slope: (mV x 1000)/oC */
#define STANDARD_TEMP           (25)


extern volatile uint16_t SwTimerIsrCounter; 


Ticker tick;             //  Creates a timer interrupt using mbed methods
 /****************      ECEN 5803 add code as indicated   ***************/
                // Add code to control red, green and blue LEDs here
                                              

Timer t;
int timer_val;

//Defining Header files 
#include "mbed.h"
#include "flow.h"
#include <math.h>

//Function Prototypes
double temperature_degree();
double density(double degree_temperature);
double conv_temp(double degree_temperature);
double viscocity(double kelvin_temperature);
double re_calculate(double viscocity, double velocity,double density,double inner_diameter_meter);
double st_calculate(double reynolds);
double velocity_calculate(double Strouhal,double frequency, double bluf_width);
double flow_calculate();

//Defining variables
uint16_t raw_adc;
double flow_gallons_minute;

//Initializing LED's
DigitalOut redLED(LED1);
DigitalOut greenLED(LED2);
DigitalOut blueLED(LED2);


void flip() 
{
    greenLED = !greenLED;
}
 

int main() {
	
		/****************      ECEN 5803 add code as indicated   ***************/
                    //  Add code to call timer0 function every 100 uS
		t.start();
    printf("Hello World!\n\r"); 
    uint32_t  count = 0;   
	
		tick.attach(&timer0, 0.0001);
    
// initialize serial buffer pointers
   rx_in_ptr =  rx_buf; /* pointer to the receive in data */
   rx_out_ptr = rx_buf; /* pointer to the receive out data*/
   tx_in_ptr =  tx_buf; /* pointer to the transmit in data*/
   tx_out_ptr = tx_buf; /*pointer to the transmit out */
    
   
  // Print the initial banner
  //  printf("\r\nHello World!\n\n\r");

    /****************      ECEN 5803 add code as indicated   ***************/
    // uncomment this section after adding monitor code.  
   /* send a message to the terminal  */                    
   /*
   UART_direct_msg_put("\r\nSystem Reset\r\nCode ver. ");
   UART_direct_msg_put( CODE_VERSION );
   UART_direct_msg_put("\r\n");
   UART_direct_msg_put( COPYRIGHT );
   UART_direct_msg_put("\r\n");

   set_display_mode();                                      
   */
	
		//Initializing the ADC - Clock, Calibration, and internal settings
		Init_ADC();
	
		//printf("Temperature Test \n\r");
    
		while (true) {
			
			count++;                   // counts the number of times through the loop
//      __enable_interrupts();
//      __clear_watchdog_timer();
       
//        serial();            // Polls the serial port
//        chk_UART_msg();     // checks for a serial port message received
//        monitor();           // Sends serial port output messages depending
                         //  on commands received and display mode
 
   /****************      ECEN 5803 add code as indicated   ***************/
   
//  readADC()
			//This functions is called in flow calculation - Read the temperature and returns the same for further calculations

//  calculate flow()
			//This function is called below to get the flow based on the parameters

//  4-20 output ()    // use TMP0 channel 3  proporional rate to flow

//  Pulse output()   // use TMP0 channel 4  propotional rate to frequency

//  LCD_Display()   // use the SPI port to send flow number

//  End ECEN 5803 code addition
			
			//Function that returns the Flow - All other function are called in this function
			flow_gallons_minute = flow_calculate();
			printf("Flow value is %f\n\r", flow_gallons_minute);
			
			timer_val = t.read();
			t.stop();
			__ASM("NOP");
			
			wait(0.1);
			
			if ((SwTimerIsrCounter & 0x1FFF) > 0x0FFF)

        {
            flip();  // Toggle Green LED
        }
				
    }
}


/*************************************************************************************************************
 *
 * Name :		  	static int adc_cal(void)
 *
 *
 *
 * Description :	This function initializes ADC register to calibrate the ADC for stable values
 *
 * Inputs: NONE
 *
 * Return: Calibration Done or fail response
 *************************************************************************************************************/
static int adc_cal(void)
{
	uint16_t calib;

	ADC0->CFG1 |= (ADC_CFG1_MODE(3)  |  // 16 bits mode
				        ADC_CFG1_ADICLK(1)|	 // Input Bus Clock divided by 2 (48Mhz / 2)
				        ADC_CFG1_ADIV(3));	 // Clock divide by 8 (3 MHz)

	ADC0->SC3  |= (ADC_SC3_AVGE_MASK |	 // Enable HW average
				        ADC_SC3_AVGS(3)   |	 // Set HW average of 32 samples
				        ADC_SC3_CAL_MASK); 	 // Start calibration process

	while (ADC0->SC3 & ADC_SC3_CAL_MASK); // Wait for calibration to end

	if (ADC0->SC3 & ADC_SC3_CALF_MASK)	// Check for successful calibration
		return -1;

	calib = 0;
	calib += ADC0->CLPS + ADC0->CLP4 + ADC0->CLP3 +
			     ADC0->CLP2 + ADC0->CLP1 + ADC0->CLP0;
	calib /= 2;
	calib |= 0x8000; 	// Set MSB
	ADC0->PG = calib;

	calib = 0;
	calib += ADC0->CLMS + ADC0->CLM4 + ADC0->CLM3 +
			     ADC0->CLM2 + ADC0->CLM1 + ADC0->CLM0;
	calib /= 2;
	calib |= 0x8000;	// Set MSB
	ADC0->MG = calib;

	return 0;
}


/*************************************************************************************************************
 *
 * Name :		  	double temperature_degree()
 *
 *
 *
 * Description :	This function calls function Measure_Temperature() which is responsible to read ADC values from PTB2(Ch 26) i.e Internal Temperature 
 * Once the function read the raw ADC some mathematical operations are performed to get the actual temperature as per the refernce manual
 *
 * Inputs: NONE
 *
 * Return: Internal temperature in degree Celcius
 *************************************************************************************************************/
double temperature_degree()
{
	double temp = Measure_Temperature();
	return temp;
}


/*************************************************************************************************************
 *
 * Name :		  double density (double degree_temperature)
 *
 *
 *
 * Description :	This function takes temperature in degrees and calculates all the density based on formula 
 * rho(density)= 1000(1 ?((T+288.9414)/(508929.2*(T+68.12963)))*(T-3.9863)^2)in kg/m^3 with T In degrees C.
 *
 * Inputs: Temperature in Degrees 
 *
 * Return: Density
 *************************************************************************************************************/

double density(double degree_temperature)
{
 double val1 = (degree_temperature -3.9863)*(degree_temperature -3.9863);
 double val2 = degree_temperature + 68.12963;
 double val3 = 508929.2 * val2;
 double val4 = degree_temperature + 288.9414;
 double val5 = val4/val3;
 double val6 = val5*val1;
 double val7 = 1 - val6;
 double val8 = 1000*val7;
 return val8;
 //printf("Density value %f\n\r", val8);
}


/*************************************************************************************************************
 *
 * Name :		  double conv_temp(double degree_temperature)
 *
 *
 *
 * Description :	This function takes temperature in degrees and converts that in Kelvin
 * Temp(In Kelvin) = Degree Temperature + 273.15 
 *
 * Inputs: Temperature in Degrees 
 *
 * Return: Temperature in Kelvin
 *************************************************************************************************************/

double conv_temp(double degree_temperature)
{
	double kelvin;
	kelvin = degree_temperature + 273.15;
	return kelvin;
}


/*************************************************************************************************************
 *
 * Name :		  double viscocity(double kelvin_temperature)
 *
 *
 *
 * Description :	This function takes temperature in kelvin and calculates viscocity based on formula 
 * viscosity = 2.4x 10^-5 * 10^(247.8/(T-140.0) kg/(m*s) = V   T is in Kelvin
 *
 * Inputs: Temperature in Kelvin 
 *
 * Return: Viscocity
 *************************************************************************************************************/
double viscocity(double kelvin_temperature)
{
	
	double val1 = kelvin_temperature-140;
	double val2 = 247.8/val1;
	double val3 = pow(10,val2);
	double val5 = 2.4* 0.00001;
	double val6 = val5*val3;
	return val6;
}


/*************************************************************************************************************
 *
 * Name :		  double re_calculate(double viscocity, double velocity,double density,double inner_diameter)
 *
 *
 *
 * Description :	This function takes viscocity, density, velocity and inner diameter in meters to calculate Reynolds numbers based on the formula 
 * Re = ( Density (in kg/(m^3)) * velocity (m/s) * PID (m)) / viscosity (kg/(m*s))
 * Here for first iteration velocity is taken as 1
 *
 * Inputs: Viscocity, Density, Velocity and Inner Diameter
 *
 * Return: Reynolds Number
 *************************************************************************************************************/

double re_calculate(double viscocity, double velocity,double density,double inner_diameter)
{
	double re = (velocity*density*inner_diameter)/viscocity;
	return re;
}


/*************************************************************************************************************
 *
 * Name :		  double st_calculate(double reynolds)
 *
 *
 *
 * Description :	This function takes Reynolds number and calculates Strouhal number based on the formula 
 * St = 0.2684 ?1.0356/SQRT(Re), where Re is the Reynolds number.
 * 
 *
 * Inputs: Reynolds Number
 *
 * Return: Strouhal Number 
 *************************************************************************************************************/

double st_calculate(double reynolds)
{
	return (0.2684 - 1.0356/sqrt(reynolds));
}


/*************************************************************************************************************
 *
 * Name :		  double velocity_calculate(double Strouhal,double frequency, double bluf_width)
 *
 *
 *
 * Description :	This function takes Strouhal number, frequency from the samples given by Professor and width of bluff body and calculates velocity based on the formula 
 * v = fd/St where f = frequency, d = widht of body and St = Strouhal number
 * 
 *
 * Inputs: Strouhal Number, Frequency, and Bluff width of body
 *
 * Return: Averahe velocity
 *************************************************************************************************************/

double velocity_calculate(double Strouhal,double frequency, double bluf_width)
{
	return ((frequency*bluf_width)/Strouhal);
}


/*************************************************************************************************************
 *
 * Name :		  double flow_calculate()
 *
 *
 *
 * Description :	This function calculates internally all the values i.e Temperature, Density, Viscocity, Reynolds number, Strouhal number, 
 * frequency from the samples given by Professor and width of bluff body, avergae velocity and returns the flowrate in gallons per minute 
 * 
 *
 * Inputs: NONE
 *
 * Return: Flowrate
 *************************************************************************************************************/

double flow_calculate()
{
	double meter_inner_diameter = 0.07366;
	double inches_inner_diameter = 2.9;
	double wave_frequency = 250;
	double bluff_width_meter = 0.0127;
	double temp_degree = temperature_degree();
	double temp_kel = conv_temp(temp_degree);
	double density_flow = density(temp_degree);
	double viscocity_flow = viscocity(temp_kel);
	double velocity = 1;
	double re_flow = re_calculate(viscocity_flow,velocity,density_flow,meter_inner_diameter);
	double st_flow = st_calculate(re_flow);
	velocity = velocity_calculate(st_flow,wave_frequency,bluff_width_meter);


	re_flow = re_calculate(viscocity_flow,velocity,density_flow,meter_inner_diameter);
	st_flow = st_calculate(re_flow);
	velocity = velocity_calculate(st_flow,wave_frequency,bluff_width_meter);


	re_flow = re_calculate(viscocity_flow,velocity,density_flow,meter_inner_diameter);
	st_flow = st_calculate(re_flow);
	velocity = velocity_calculate(st_flow,wave_frequency,bluff_width_meter);

	//printf("Final velocity %f\n\r", velocity);
	double flow = 2.45*velocity*3.28084*inches_inner_diameter*inches_inner_diameter;
	return flow;
}

/*************************************************************************************************************
 *
 * Name :		  void Init_ADC(void) 
 *
 *
 *
 * Description :	This function initialized ADC, calibrates ADC and defines all the register parameters 
 *
 * Inputs: NONE
 *
 * Return: NONE
 *************************************************************************************************************/

void Init_ADC(void) 
	{

		//Giving clock to ADC peripheral 
		SIM->SCGC6 |= (1UL << SIM_SCGC6_ADC0_SHIFT);
		
		//Calling the calibration function to calibrate teh ADC before reading values
		int cal_res;
	
		do
		{
				cal_res = adc_cal();
		}while(cal_res == -1);
		
		//Configuring the CFG register as - Low power, Clock with no prescaler, Long sampling time, Mode 3 = 16 bit resolution, and bus clock
		
		ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADIV(0) | ADC_CFG1_ADLSMP_MASK |
		ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0); // 16 bit conversion mode

		//Defining the default voltage reference
		ADC0->SC2 = ADC_SC2_REFSEL(0); // Select default voltage reference pins

	}

/*************************************************************************************************************
 *
 * Name :		 unsigned Measure_Temperature(void)
 *
 *
 *
 * Description :	This function calculates reads RAW ADC and calculates the temperature in degrees
 *
 * Inputs: NONE
 *
 * Return: Temperature in Degrees
 *************************************************************************************************************/
	
unsigned Measure_Temperature(void) {
			volatile unsigned temp_adc=0;

			ADC0->SC1[0] = 26; // start conversion on Temperature channel 26

			while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
			; // Wait until conversion is complete
			temp_adc = ADC0->R[0];
			
			float v2 = temp_adc*3300.0f/ADCR_VDD;                            //Convert v2 to millivolts
			float temp = 25.0f-(v2- V_TEMP25)*1000.0f/M;                     //Convert the ADC into temperature
			
			
			printf("Temperature value is %f\n\r", temp);
			return temp;
}