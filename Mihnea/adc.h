/*
 * adc.h
 *
 * Created: 8/29/2019 9:54:01 PM
 *  Author: alexandru.sontica
 */ 


#ifndef ADC_H_
#define ADC_H_

void adc_init();

void adc_select_sensor(unsigned int sensor_number);

void adc_start_conversion();

unsigned int adc_get_value();

double adc_full_read_temperature();
double adc_full_read_light();
double adc_full_read_humidity();

#endif /* ADC_H_ */