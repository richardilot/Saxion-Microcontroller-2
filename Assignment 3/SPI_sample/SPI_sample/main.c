/*
 * SPIexample
 * The example shows how to work with the  SPI Expander chip MCP23s08 as output
 * The example also remembers you how to work with the USART
 * Created: 9-10-2015 07:35:40 /april 2016 / februari 2019
 * Author : Tom IJsselmuiden/Hans Stokkink
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define SS 2        // pin 10 arduino uno
#define MOSI 3      // pin 11 arduino uno
#define MISO 4      // pin 12 arduino uno
#define SCK 5       // pin 13 arduino uno
#define TXD 1

#define ENABLE_SS PORTB &= ~(1 << SS)
#define DISABLE_SS PORTB |= (1 << SS)
#define ENABLE_SS2 PORTB &= ~(1 << PORTB1)  //pin 9 arduino = pin portb1 for other spi chips
#define DISABLE_SS2 PORTB |= (1 << PORTB1)

#define  FOSC 16000000
#define  BAUD 9600
#define  MYUBBR FOSC/16/BAUD -1

#define UART_BUFF_SIZE 10


#define A 0
#define B 1



#define READ 1
#define WRITE 0

#define IODIR 0
#define OUTP_LATCH 10

//void uart_init(void);
void USART_Init(void);
void init(void);
uint8_t spi_tranceiver (uint8_t data);
void USART_Transmit(uint8_t data);

int16_t Temp_read(void);
void IOEXP_IODIR(uint8_t data);
void IOEXP_datalatch(uint8_t data);

uint8_t RX_buf[UART_BUFF_SIZE];
uint8_t TX_buf[UART_BUFF_SIZE];
uint8_t RX_index = 0;
uint8_t TX_index = 0;

ISR(USART_RX_vect)
{
	RX_buf[RX_index++] = UDR0;
	USART_Transmit(RX_buf[RX_index-1]);
}

int main(void)
{
	int x=0;
	init();
	/* Clearing buffers */
	memset(RX_buf,0,sizeof(RX_buf));
	memset(TX_buf,0,sizeof(TX_buf));
	
	/* Enable interrupts */ 
	sei();
	
	/* set all pins of I/0 expander to output */
	IOEXP_IODIR(0);
	/* set lower 4 bits (leds) high*/
	IOEXP_datalatch(0x0f);
	_delay_ms(1000);
	/* set higher 4 bits (leds) high*/
	IOEXP_datalatch(0xf0);
	_delay_ms(1000);
	int16_t data = 0xaa;
   IOEXP_datalatch(data);   //set led
    while (1) 
    {	 
		data++;
		//int16_t data = 0xaa;
		IOEXP_datalatch(data);   //set led
		USART_Transmit(data);    // give same value to USART
		_delay_ms(100);
    }
}

void init()
{
	USART_Init();
	/* Set SS, MOSI and SCK output, all others input */
	DDRB = (1<<SS)|(1<<MOSI)|(1<<SCK)|(1 << PORTB1);
	/* LED PIN */ 
	DDRB |= (1 << PORTB0);
	/* Set TXD as an output */
	DDRD = (1 << TXD);
	
	/* Set the slave select pin (Active low) */ 
	DISABLE_SS;
	DISABLE_SS2;
	
	/* Enable SPI, Master, set clock rate fosc/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
	}

// function to initialize UART
void USART_Init(void)
{
	
	// Set baud rate:
	UBRR0=103;                 //UBRR= Fosc /(16*9600) -1 =103.166= 103

	// enable receiver and transmitter
	UCSR0B |=(1<<RXEN0 |(1 <<TXEN0));

	// Set frame format : 8 data 2 stop bit

	UCSR0C = (1<<USBS0 )|(3<<UCSZ00);
}

void USART_Transmit( uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

/* spi data buffer load and send */
uint8_t spi_tranceiver (uint8_t data)
{
	// Load data into the buffer
	SPDR = data;
	
	//Wait until transmission complete
	while(!(SPSR & (1<<SPIF) ));
	
	// Return received data
	return(SPDR);
}



/* IO expander data direction set */
void IOEXP_IODIR(uint8_t data)
{
	uint8_t r_data = 0;
					// Make slave select go low
				ENABLE_SS;
				/* Send device address + r/w */
				spi_tranceiver((1<<6)|WRITE);
				/* Send command */
				spi_tranceiver(IODIR);

				spi_tranceiver(data);
				// Make slave select go high
				DISABLE_SS;
				
}


/* IO expander latch  set */
void IOEXP_datalatch(uint8_t data)
{
	// Make slave select go low
	ENABLE_SS;
	/* Send device address + r/w */
	spi_tranceiver((1<<6)|WRITE);
	/* Send command */
	spi_tranceiver(OUTP_LATCH);
	spi_tranceiver(data);
	// Make slave select go high
	DISABLE_SS;
	
}



