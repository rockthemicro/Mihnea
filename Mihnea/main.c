/*
 * Mihnea.c
 *
 * Created: 7/30/2019 8:56:36 PM
 * Author : alexandru.sontica
 */ 

#include "lcd.h"
#include <avr/io.h>			/* Include AVR std. library file */
#include <util/delay.h>		/* Include Delay header file */
#include <stdbool.h>		/* Include standard boolean library */
#include <string.h>			/* Include string library */
#include <stdio.h>			/* Include standard IO library */
#include <stdlib.h>			/* Include standard library */
#include <avr/interrupt.h>	/* Include avr interrupt header file */
#include "usart.h"			/* Include USART header file */

#define SREG    _SFR_IO8(0x3F)

#define DEFAULT_BUFFER_SIZE		160
#define DEFAULT_TIMEOUT			10000

/* Connection Mode */
#define SINGLE				0
#define MULTIPLE			1

/* Application Mode */
#define NORMAL				0
#define TRANSPERANT			1

/* Application Mode */
#define STATION				1
#define ACCESSPOINT			2
#define BOTH_STATION_AND_ACCESPOINT	3

int main(void)
{
	LCD_init();
	LCD_printAt(64, "Mihnea");
    
	/* activare USER led */
	DDRD |= (1 << PD7);
	PORTD |= (1 << PD7);
	
	/* activare backlight LED */
	DDRC |= (1 << PC2);
	PORTC |= (1 << PC2);
	
	/* activare RST si EN pt ESP8266 */
	DDRB |= (1 << PB7);
	DDRB |= (1 << PB6);
	PORTB |= (1 << PB7);
	PORTB |= (1 << PB6);
	
	USART0_init();
	_delay_ms(1000);
	
	LCD_printAt(0, "1");
	
	
	USART0_transmit('A');
	LCD_printAt(0, "1.1");
	USART0_transmit('T');
	LCD_printAt(0, "1.2");
	USART0_transmit('\r');
	LCD_printAt(0, "1.3");
	USART0_transmit('\n');
	
	//USART0_print("AT\r\n");
	LCD_printAt(0, "2");
	
	char a = USART0_receive();
	LCD_printAt(0, "3");
	
	char b = USART0_receive();
	LCD_printAt(0, "4");
	_delay_ms(3000);
	
	
	char c[3];
	c[0] = a;
	c[1] = b;
	c[2] = '\0';
	LCD_printAt(0, c);
	
    while(1) {
	    /* Invers?m starea pinului. */
	    PORTD ^= (1 << PD7);
	    _delay_ms(500);
    }
}

