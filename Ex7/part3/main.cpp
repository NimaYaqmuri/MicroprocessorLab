// The macro F_CPU specifies the CPU frequency to be considered by the delay macros.
#define  F_CPU 8000000UL

//----- Headers ------------//
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
#include "lcd.h"
#include "keypad.h"
//--------------------------//


int rotation[4] = {0x05, 0x06, 0x0A, 0x09};
int T = 0;

int main()
{
	DDRD |= 0x0F;

	init();
	write_str("Write steps and");
	command(MOVE_CURSOR_2nd_LINE);
	write_str("press C: ");
	KP_Setup();

	int size = 16;
	char str[16];
	int index = 0;

    while(true)
	{
		str[index++] = keyfind();
		write_char(str[index - 1]);

		while(str[index - 1] != Key_C)
		{
			str[index++] = keyfind();
			write_char(str[index - 1]);
		}

		str[index -1] = 0;
		clear();
		
		for(int i = 0; i < atoi(str); i++)
		{
			write_char('1' + i);
			command(MOVE_CURSOR_1st_LINE);

			PORTD = rotation[T++];    
			T %= 4;

			_delay_ms(100);
		}

		_delay_ms(100);

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