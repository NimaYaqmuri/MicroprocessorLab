#define F_CPU 8000000UL /* Define frequency here its 8MHz */
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

//----- Headers ------------//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "keypad.h"
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

bool login = false;

int main()
{
	DDRD |= 0x02;

	UART_init(9600);

	init();
	write_str("Login page!");
	command(MOVE_CURSOR_2nd_LINE);
	write_str("Enter ID: ");
	KP_Setup();

	sei();

	char admin_id  [5] = "1234";
	char admin_pass[5] = "1234";

	while(!login)
	{
		char id[5];
		for(uint8_t i = 0; i < 4; i++)	
		{
			id[i] = keyfind();
			write_char(id[i]);
		}	

		clear();
		write_str("Login page!");
		command(MOVE_CURSOR_2nd_LINE);
		write_str("Enter Pass: ");

		char pass[5];
		for(uint8_t i = 0; i < 4; i++)
		{
			pass[i] = keyfind();
			write_char(pass[i]);
		}	
		
		clear();

		bool check = true;
		for(int i = 0; i < 4; i++)
		{
			if(admin_id[i] != id[i]) check = false;
			if(admin_pass[i] != pass[i]) check = false;
		}

		if(check)
		{	
			write_str("Welcom!");
			login = true;

			_delay_ms(200);
			clear();
		}
		else
		{
			write_str("Try again!");
			_delay_ms(200);
			
			clear();
			write_str("Login page!");
			command(MOVE_CURSOR_2nd_LINE);
			write_str("Enter ID: ");
		}
	}


	write_str("Write steps and");
	command(MOVE_CURSOR_2nd_LINE);
	write_str("press C: ");

	int size = 16;
	char str[16];
	int index = 0;
	char tmp;

    while(true)
	{
		str[index++] = keyfind();
		write_char(str[index - 1]);

		while(str[index - 1] != Key_C)
		{
			str[index++] = keyfind();
			write_char(str[index - 1]);
		}

		str[index - 1] = 0;
		clear();

		UDR = atoi(str);

		write_str("Press C to");
		command(MOVE_CURSOR_2nd_LINE);
		write_str("continu: ");
		tmp = keyfind();

		while(tmp != Key_C)
			tmp = keyfind();
		write_char(tmp);

		for(int i = 0; i < index; i++)
			str[i] = 0;
		index = 0;

		clear();
		write_str("Write steps and");
		command(MOVE_CURSOR_2nd_LINE);
		write_str("press C: ");
	}

	return 0;
}