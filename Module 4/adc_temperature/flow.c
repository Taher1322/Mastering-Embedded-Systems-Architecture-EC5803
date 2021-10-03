#include "flow.h"
#include <stdio.h>
//#include "clock_config.h"
#include "MKL25Z4.h"

#define ADC_POS (2)

/*
void Init_ADC(void) {

	//int cal_res;
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;

    // Select analog for pin
    PORTB->PCR[ADC_POS] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[ADC_POS] |= PORT_PCR_MUX(0);

//    do
//    {
//    cal_res = adc_cal();
//    } while (cal_res == -1);

    // Low power configuration, long sample time, 16 bit single-ended conversion, bus clock input
    ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0);
    // Software trigger, compare function disabled, DMA disabled, voltage references VREFH and VREFL
    ADC0->SC2 = ADC_SC2_REFSEL(0);


}

float Read_temp(void){
    float n, temp;

    ADC0->SC1[0] = 0x00; // start conversion on channel 0

    // Wait for conversion to finish
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
        ;
    // Read result, convert to floating-point
    n = (float) ADC0->R[0];

    printf("Value of N is %f\n\r" ,n);

    // Calculate temperature (Celsius) using polynomial equation
    // Assumes ADC is in 16-bit mode, has VRef = 3.3 V


    temp = -36.9861 + n*(0.0155762 + n*(-1.43216E-06 + n*(7.18641E-11
        + n*(-1.84630E-15 + n*(2.32656E-20 + n*(-1.13090E-25))))));
    return temp;
}
*/

void Init_ADC(void) {

SIM->SCGC6 |= (1UL << SIM_SCGC6_ADC0_SHIFT);
ADC0->CFG1 = ADC_CFG1_ADLPC_MASK | ADC_CFG1_ADIV(0) | ADC_CFG1_ADLSMP_MASK |
ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0); // 16 bit conversion mode

ADC0->SC2 = ADC_SC2_REFSEL(0); // Select default voltage reference pins
}

unsigned Measure_Temperature(void) {
volatile unsigned temp_adc=0;

ADC0->SC1[0] = 26; // start conversion on IR channel

while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
; // Wait until conversion is complete
temp_adc = ADC0->R[0];

return temp_adc;
}

