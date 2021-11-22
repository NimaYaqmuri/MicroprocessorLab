// The macro F_CPU specifies the CPU frequency to be considered by the delay macros.
#define  F_CPU 8000000UL

//----- Headers ------------//
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "lcd.h"
#include "keypad.h"
//--------------------------//

bool login = false;

int main()
{
	DDRB = DDRB |
		   (1 << PINB3) | (1 << PINB6);

	init();
	write_str("Login page!");
	command(MOVE_CURSOR_2nd_LINE);
	write_str("Enter ID: ");
	KP_Setup();

	char admin_id  [5] = "1234";
		char admin_pass[5] = "1234";

    while(1){
		
		
		while(!login)
		{
			char id[5];
			for(uint8_t i = 0; i < 4; i++)	
			{
				id[i] = keyfind();
				write_char(id[i]);
			}	

			command(CLEAR_DISPLAY);
			write_str("Login page!");
			command(MOVE_CURSOR_2nd_LINE);
			write_str("Enter Pass: ");

			char pass[5];
			for(uint8_t i = 0; i < 4; i++)
			{
				pass[i] = keyfind();
				write_char(pass[i]);
			}	
			
			command(CLEAR_DISPLAY);

			bool check = true;
			for(int i = 0; i < 4; i++)
			{
				if(admin_id[i] != id[i]) check = false;
				if(admin_pass[i] != pass[i]) check = false;
			}

			if(check)
			// (id == admin_id && pass == admin_pass)
			{	
				write_str("Welcom!");
				login = true;

				PORTB |= (1 << PINB3);
				// _delay_ms(1000);
				// PORTB &= ~(1 << PINB3);
			}
			else
			{
				PORTB |= (1 << PINB6);
				// _delay_ms(1000);
				// PORTB &= ~(1 << PINB6);

				write_str("Try again!");
				// write_str(admin_id);
				command(MOVE_CURSOR_2nd_LINE);
				// write_str(admin_pass);
				write_str("Enter ID: ");

			}
		}
	};
}