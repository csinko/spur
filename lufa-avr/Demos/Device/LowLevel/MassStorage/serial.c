#include "serial.h"

// initialize the hardware uart: 57600 baud, 8 bits, no parity, 2 stop bits
void serialBegin()
{
	const unsigned int baud = 57600;
	UBRR1 = (F_CPU / 4 / baud - 1) / 2;
	UCSR1A = (1<<U2X1);		// set double transmission speed
	UCSR1B = (1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1);
	UCSR1C = (1<<USBS1)|(3<<UCSZ10);
}

// output a single byte to uart
void serialWrite(uint8_t data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR1A & (1<<UDRE1)) )
	;
	/* Put data into buffer, sends the data */
	UDR1 = (uint8_t)data;
}

// output a string to uart
void serialWriteArray(char data[], unsigned int len)
{
	for(int i = 0; i < len; i++)
	{
		serialWrite(*(data + i));
	}
}