/*
 * serial_comm_transmit.c
 *
 * Created: 09/02/2021 14:59:43
 * Author : Richie Ilot
 */ 

#define F_CPU 1600000UL
#define FOSC 1843200 //clk speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#include <avr/io.h>
#include <avr/interrupt.h>

/**
*	Note to self: this program contains both transmitter and retriever.
*				  Reason is to make the program more mobile to become transmitter/retriever.
**/

//USART setup function
void USART_setup(unsigned int ubrr){
	//set baud rate
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;
	
	//enable transmit (change RXEN0 to 1 if retrieve is enabled)
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);
	
	//set 8 bit char and 2 stop bit
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

//USART retriever function
unsigned char USART_retrieve(void){
	return UDR0;
/*
	while(!(UCSR0A & (1<<RXC0))){
		return UDR0;
	}*/
}

//USART transmitter function
void USART_transmit(unsigned char data){
	UDR0 = data;
/*
	while( !(UCSR0A & (1<<UDRE0)) ){
		UDR0 = data;
	}
	*/
}

//pin and ADC setup for transmitter
void transmitter_setup(void){
	DDRC = 0b000000000; //sets all C pins as input
	/*
	ADMUX is ADC Multiplexer Selection Register.
	Setting bit 6 (REFS0) to high will select the 
	voltage reference for ADC to AVcc (Table 23-3 in datasheet).
	And setting bit 5 (ADLAR) to '1' will adjust the result to the left.
	*/
	ADMUX |= (1<<REFS0)|(1<<ADLAR);
	/*
	ADCSRA is ADC Control and Status Register A.
	Setting bit 7 (ADEN - ADC Enable) to high will enables the AD conversion
	Setting bit 6 (ADSC - ADC start conversion) to high will perform the AD conversion.
	Setting bit 5 (ADATE - ADC Auto Trigger Enable) to high will enable the auto triggering of the ADC.
	Setting bit 3 (ADIE - ADC Interrupt Enable) to high will enable interrupt for ADC is activated.
	Setting bit 2-0 (ADPS0-2) to high will select the ADC division factor for the prescaler to 128.
	*/
	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
}

void retriever_setup(void){
	DDRD = 0b11110000; //sets D pin 4-7 to output
}
int main(void)
{
	cli();
	DDRD = 0b11110000;
	DDRC = 0b00000000;
	UBRR0H = (unsigned char) (MYUBRR >> 8);
	UBRR0L = (unsigned char) MYUBRR;
		
	//enable transmit (change RXEN0 to 1 if retrieve is enabled)
	UCSR0B = (1<<TXEN0)|(0<<RXEN0);
		
	//set 8 bit char and 2 stop bit
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	ADMUX |= (1<<REFS0)|(1<<ADLAR);
	ADCSRA |= (1<<ADEN)|(1<<ADSC)|(1<<ADATE)|(1<<ADIE)|(1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2);
	
/*	transmitter_setup();
	retriever_setup();
	USART_setup();
	*/
	while(!(UCSR0A & (1<<RXC0))){
		PORTD=UDR0;
	}

	sei();
	
    while (1) 
    {
		//PORTD = USART_retrieve();
    }
}

ISR(ADC_vect){
//	USART_transmit(ADCH);
	while ( !( UCSR0A & (1<<UDRE0)) ){
		UDR0 = ADCH;
	}
}