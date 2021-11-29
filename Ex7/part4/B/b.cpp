#define F_CPU 8000000UL /* Define frequency here its 8MHz */
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

//----- Headers ------------//
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
//--------------------------//

void UART_init(long USART_BAUDRATE)
{
	// USART initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART Receiver: On
	// USART Transmitter: Off
	// USART Mode: Asynchronous
	UCSRB |= _BV(RXCIE) | _BV(RXEN) | (1 << TXEN); /* Turn on the transmission and reception */
	UCSRC |= _BV(URSEL) | _BV(UCSZ1) | _BV(UCSZ0); /* Use 8-bit character sizes */

	UBRRL =  BAUD_PRESCALE;       /* Load lower 8-bits of the baud rate */
	UBRRH = (BAUD_PRESCALE >> 8); /* Load upper 8-bits */
}

int rotation[4] = {0x05, 0x06, 0x0A, 0x09};
int T = 0;

ISR(USART_RXC_vect)
{
    int steps = UDR;
	for(int i = 0; i < steps; i++)
	{
		PORTD = (rotation[T++] << 4);    
		T %= 4;

		_delay_ms(100);
	}
}

int main()
{
	DDRD |= 0xF0;

	UART_init(9600);

	sei();

	while(true){};

	return 0;
}