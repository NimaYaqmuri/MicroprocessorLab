#define F_CPU 8000000UL /* Define frequency here its 8MHz */
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

//----- Headers ------------//
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
//--------------------------//

int tmp;

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


void acticate_ADC()
{
    // Set port A for input.
    DDRA = 0x00;

    // Enable the ADC.
    ADCSRA = ADCSRA
            | (1 << ADEN);

    // Determine the division factor between the XTAL frequency and the input clock to the ADC.
    // In this case: System Clock / 128
    ADCSRA = ADCSRA
            | (1 << ADPS2)
            | (1 << ADPS1)
            | (1 << ADPS0);

    // Reference voltage selection for ADC.
    // In this case: AVCC pin i.e. Vcc 5 V
    ADMUX = ADMUX
            | (1 << REFS0);
}

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

	acticate_ADC();

	sei();

	while(true)
	{
		// Start conversion.
        ADCSRA = ADCSRA
                | (1 << ADSC);		

        // Wait until the conversation is over.
        while ((ADCSRA & (1 << ADIF)) == 0)

        _delay_us(10);
		
		tmp = (ADC - 1) / 2;
		UDR = tmp;
	};

	return 0;
}