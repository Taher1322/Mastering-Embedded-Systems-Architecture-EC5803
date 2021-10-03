/*
 * adc.h
 *
 *  Created on: Sep 30, 2021
 *      Author: ujjai
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>
#include "MKL25Z4.h"

//void Init_ADC(void);
void Init_ADC(void);
float Read_temp(void);
uint16_t adc_read(uint8_t ch);
static int adc_cal(void);
unsigned Measure_Temperature(void);

#endif /* ADC_H_ */
