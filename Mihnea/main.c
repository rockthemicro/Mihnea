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


static void USART_flush(void)
{
	unsigned char dummy;
	while (UCSR0A & (1<<RXC0)) {
		LCD_printAt(0, "daaaa");
		dummy = UDR0;
	}
}

static int value = 0;
static char vect[2] = "0";

ISR(USART0_RX_vect) {
	value = UDR0;
	
	vect[0] += 1;
	LCD_printAt(0, vect);
	
}

int main(void)
{
	/* activeaza intreruperi globale */
	sei();
	
	LCD_init();
	LCD_printAt(64, "Mihnea");
    
	/* activare USER led */
	DDRD |= (1 << PD7);
	PORTD |= (1 << PD7);
	
	/* activare backlight LED */
	DDRC |= (1 << PC2);
	PORTC |= (1 << PC2);
	
	USART0_init();
	_delay_ms(1000);

    while(1) {
	    /* Invers?m starea pinului. */
	    PORTD ^= (1 << PD7);
		
		USART0_transmit('x');
	    _delay_ms(2000);
    }
}

