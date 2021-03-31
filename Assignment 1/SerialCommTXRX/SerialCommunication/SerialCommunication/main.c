/*
 * SerialCommunication.c
 *
 * Created: 04/03/2021 14:27:14
 * Author : Richie Ilot
 */ 
#define F_CPU 1600000
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
	cli();
	DDRB = 0b11111111;
	DDRC = 0b00000000;
	UBRR0H = (MYUBRR >> 8);
	UBRR0L = MYUBRR;
		
	//enable transmit and receive
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
		
	//set 8 bit char
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	ADMUX |= (1<<REFS0)|(0<<ADLAR);
	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS0)|(0<<ADPS1)|(1<<ADPS2);
	sei();
	
    while (1) 
    {
		while(!(UCSR0A & (1<<RXC0)));
		PORTB = UDR0;
	}
}

ISR(ADC_vect){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = ADCH;
}