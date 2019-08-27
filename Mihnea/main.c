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

void send_request();

ISR(PCINT1_vect)
{
	if ((PINB & (1<<PB2)) == 0) {
		/* Inversam starea pinului. */
		PORTD ^= (1 << PD7);
		
		send_request();
	}
}

void shift_left(char *v, int first, int last)
{
	int i;
	
	for (i = 0; i < last - 1; i++) {
		v[i] = v[i + 1];
	}
}

/* citire pana la primul >\r\n
 * > este semnul trimis de ESP inainte ca noi sa putem incepe
 * sa-i transmitem datele pentru un AT+CIPSEND
 */
int read_until_bigger()
{
	char v[4] = "";
	int cnt = 0;
	
	while (1) {
		v[2] = USART0_receive();
		cnt++;
		
		if (strcmp(v, ">\r\n") == 0)
			return cnt;
		
		shift_left(v, 0, 4);
	}
	
	return 0;
}

/* folosit pana la orice citire de OK\r\n */
int read_until_ok()
{
	char v[5] = "";
	int cnt = 0;
	
	while (1) {
		v[3] = USART0_receive();
		cnt++;
		
		if (strcmp(v, "OK\r\n") == 0)
			return cnt;
		
		shift_left(v, 0, 4);
	}
	
	return 0;
}

/*
 * folosit pana la citirea \r\n\r\n, pentru citirea completa a
 * requesturilor http GET
 */
int read_until_2crlf()
{
	char v[5] = "";
	int cnt = 0;
	
	while (1) {
		v[3] = USART0_receive();
		cnt++;
		
		if (strcmp(v, "\r\n\r\n") == 0)
		return cnt;
		
		shift_left(v, 0, 4);
	}
	
	return 0;	
}

#define REQUEST \
"GET /api/salut HTTP/1.1\r\n\
Host: 192.168.4.3:8080\r\n\r\n"

#define REQUEST_LEN strlen(REQUEST)

void send_request()
{
	char buf[64];
	
	USART0_print("AT+CIPSTART=0,\"TCP\",\"192.168.4.3\",8080\r\n");
	read_until_ok();
	
	sprintf(buf, "AT+CIPSEND=0,%d\r\n", REQUEST_LEN);
	USART0_print(buf);
	read_until_ok();
	
	USART0_print(REQUEST);
	read_until_ok();

	/* daca ma intereseaza vreodata raspunsul serverului, in sectiunea asta
	 * trebuie sa incep sa citesc ce mi se trimite
	 */
	
	/* probabil Spring face close de unul singur, deci nu e neaparat nevoie
	 * de CIPCLOSE
	 */
	USART0_print("AT+CIPCLOSE=0\r\n");
	read_until_ok();
}

int main(void)
{
	/* activate intreruperi globale */
	sei();
	
	/* configurarea butonului de la PD6 ca intrare */
	DDRB &= ~(1<<PB2);
	/* configurarea rezistentei de pull-up a PB2 */
	PORTB |= (1<<PB2);
	
	/* activare intrerupere pentru butonul PB2 */
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<<PCINT10);
	
	LCD_init();
	USART0_init();
    _delay_ms(1000);
	
	/* activare USER led */
	DDRD |= (1 << PD7);
	PORTD |= (1 << PD7);
	
	/* activare backlight LED */
	DDRC |= (1 << PC2);
	PORTC |= (1 << PC2);
	
	
	USART0_print("ATE0\r\n");
	read_until_ok();
	
	USART0_print("AT+CIPMUX=1\r\n");
	read_until_ok();
	
	USART0_print("AT+CIPSERVER=1,80\r\n");
	read_until_ok();
	
	USART0_print("AT\r\n");
	read_until_ok();
	
	LCD_printAt(0, "ok");
	
	while(1) {
	}
}

