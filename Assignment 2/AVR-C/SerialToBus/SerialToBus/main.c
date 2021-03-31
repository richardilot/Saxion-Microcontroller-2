/* 
 * SerialToBus.c
 *
 * Created: 02/03/2021 14:29:05
 * Author : Richie Ilot
 */

#define F_CPU 16000000
#define BAUD 9600
#define MYUBRR ((F_CPU/16/BAUD)-1)

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

char volt[] = "";
float sensor_value = 0;
float voltage = 0.0;
int tx_buffer_size = 32;
	
//USART initialization setup
void USART_init()
{
	//set BAUD rate
	UBRR0H = (MYUBRR>>8);
	UBRR0L = MYUBRR;
	
	//enable serial transmit
	UCSR0B |= (1<<TXEN0);
	
	//set 8 bit char and 2 stop bit
	UCSR0C |= (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
}
	
//AD converter function
int ADC_convert()
{
	//set pin C as input
	DDRC = 0b00000000;
	
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
	ADCSRA |= (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADATE)|(1<<ADSC);
}

//serial communication transmit
void USART_transmit(char *data)
{
	while(*data)
	{
		//while new data is not detected, don't set anything
		while(!(UCSR0A & (1<<UDRE0)));
		//iterates the char array
		UDR0 = (*data++);
	}
}
	
int main(void)
{
	ADC_convert();
	USART_init();
	
    while (1) 
    {
		//read Analog value from arduino analog pins
		sensor_value = ADCH;
		
		//converts raw data to voltage
		voltage = (5*sensor_value)/64;
		
		snprintf(volt, tx_buffer_size, "Voltage = %.2f Volt\r", voltage);
		USART_transmit(volt);
		_delay_ms(250);
		
	}
}