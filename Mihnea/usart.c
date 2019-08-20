#include "usart.h"

/*
 * Functie de initializare a controllerului USART
 */
void USART0_init()
{
	UCSR0A = 0x00;
	
	/* activeaza intreruperea la RXC0 */
	UCSR0B |= (1<<RXCIE0);
	
    /* seteaza baud rate la 115.2K */
    UBRR0H = 0;
    UBRR0L = 8;
    
	/* selecteaza mod asincron */
	UCSR0C &= ~(1<<UMSEL00);
	UCSR0C &= ~(1<<UMSEL01);

    /* seteaza formatul frame-ului: 8 biti de date */
	UCSR0C |= (2<<UCSZ00);
	
	/* 1 bit de stop */
    UCSR0C &= ~(1<<USBS0);
    
	/* fara paritate */
	UCSR0C &= ~(1<<UPM00);
    UCSR0C &= ~(1<<UPM01);
	
	/* porneste transmitatorul si receptorul */
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);
	
}

/*
 * Functie ce transmite un caracter prin USART
 *
 * @param data - caracterul de transmis
 */
void USART0_transmit(char data)
{
    /* asteapta pana bufferul e gol */
    while(!(UCSR0A & (1<<UDRE0)));

    /* pune datele in buffer; transmisia va porni automat in urma scrierii */
    UDR0 = data;
}

/*
 * Functie ce primeste un caracter prin USART
 *
 * @return - caracterul primit
 */
char USART0_receive()
{
    /* asteapta cat timp bufferul e gol */
    while(!(UCSR0A & (1<<RXC0)));

    /* returneaza datele din buffer */
    return UDR0;
}

/*
 * Functie ce transmite un sir de caractere prin USART
 *
 * @param data - sirul (terminat cu '\0') de transmis
 */
void USART0_print(const char *data)
{
    while(*data != '\0')
        USART0_transmit(*data++);
}

/*
 * Functie ce transmite un caracter prin USART
 *
 * @param data - caracterul de transmis
 * @param stream - nefolosit
 *
 * @return - intotdeauna returneaza 0
 */
int USART0_printf(char data, FILE *stream)
{
    /* asteapta pana bufferul e gol */
    while(!(UCSR0A & (1<<UDRE0)));

    /* pune datele in buffer; transmisia va porni automat in urma scrierii */
    UDR0 = data;
    return 0;
}
