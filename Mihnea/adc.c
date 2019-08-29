#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <math.h>

#include "adc.h"

/*
 * senzorul de temperatura este pe PA0
 * senzorul de luminozitate este pe PA2
 * senzorul de umiditate a solului este pe PA4
 */
#define TEMP_SENSOR		0
#define LIGHT_SENSOR	2
#define HUMI_SENSOR		4

void adc_init()
{
	ADMUX = 0;

	/* selectare canal 1 (conform tabelului 23-4) */
	ADMUX |= (1<<MUX0);

	/* referinta la AVCC (tensiunea de alimentare) */
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);

	/* ajustare rezultat la dreapta; bitii 0:7 in ADCL, 8:9 in ADCH */
	ADMUX &= ~(1<<ADLAR);

	ADCSRA = 0;

	/* enable convertor */
	ADCSRA |= (1<<ADEN);
}

void adc_select_sensor(unsigned int sensor_number)
{
	/* curatam bitii de selectare a portului */
	ADMUX &= ~(1<<MUX0);
	ADMUX &= ~(1<<MUX1);
	ADMUX &= ~(1<<MUX2);
	
	/* ne asiguram ca nu afectam ceilalti biti prin pastrarea maxim a primilor 3 biti */
	sensor_number = sensor_number & 0x3;
	
	/* punem pe 1 bitii lui ADMUX care sunt pe 1 in sensor_number */
	ADMUX |= sensor_number;
}

void adc_start_conversion()
{	
	ADCSRA |= (1<<ADSC);
}

/*
 * ADC = (Vin * 1024) / Vref
 * Vref este Vcc, adica 5V
 * Vin este dat de senzor
 */
unsigned int adc_get_value()
{
	/* asteptam finalizarea conversiei */
	while ((ADCSRA & (1<<ADIF)) == 0) {};

	/*
	 * conversia e gata, obtine rezultatul; value este in [0; 1024) probabil
	 */
	unsigned int value = ADC;

	return value;	
}

double adc_full_read_temperature()
{
	adc_select_sensor(TEMP_SENSOR);
	adc_start_conversion();
	double value = (double) adc_get_value();
	
	/* masuram tensiunea data de senzor, in milivolti; Vin reiese din formula din
	 * comentariul de la adc_get_value
	 */
	double Vin = (value * 5000.0) / 1024.0;
	
	/* formula obtinuta din documentatia senzorului */
	double temperature = (Vin - 500.0) / 10.0;
	
	return temperature;
}

/* http://www.esp32learning.com/code/esp32-and-ga1a12s202-light-sensor-example.php tabel cu valori lux
 */
double adc_full_read_light()
{
	adc_select_sensor(LIGHT_SENSOR);
	adc_start_conversion();
	/* iesirea senzorului este in scara logaritmica */
	double value = (double) adc_get_value();
	
	/* valoarea maxima a numarului de lux, adica 10^5, adica 50.000 lux
	 * senzorul poate masura intre 3 si 55.000 lux, dar alegem sa ne raportam la 50.000
	 * lux vine de la luminous flux
	 * 1 lux = 1 lumen / m^2
	 */
	double logRange = 5.0;
	
	/* valoarea maxima data de convertorul digital */
	double rawRange = 1024.0;
	
	/* practic, obtinem un procentaj din maximul dat de senzor, pt ca impartim value la rawRange;
	 * acel procentaj il inmultim cu exponentul maxim al logaritmului, adica cu logRange
	 */
	double logLux = (value * logRange) / rawRange;
	
	/* numarul de lux; maxim 50.000 */
	double lux = pow(10, logLux);
	
	return lux;
}

double adc_full_read_humidity()
{
	adc_select_sensor(HUMI_SENSOR);
	adc_start_conversion();
	double value = (double) adc_get_value();

	/* pur si simplu exprimam umiditatea ca procentaj din maximul posibil dat de senzor */
	return (value * 100) / 1024.0;
}