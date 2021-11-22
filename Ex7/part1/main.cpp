// The macro F_CPU specifies the CPU frequency to be considered by the delay macros.
#define  F_CPU 8000000UL

//----- Headers ------------//
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "keypad.h"
//--------------------------//

//Timer0 setup
void Timer0_Setup(void)
{
	TCNT0 = 0;
	BitSet(TCCR0, WGM01);
	BitSet(TCCR0, CS01);
	BitSet(TCCR0, CS00);
	BitSet(TIMSK, OCIE0);	
}

//Timer0 interrupt service routine
ISR(TIMER0_COMP_vect)
{
	command(MOVE_CURSOR_2nd_LINE);
	write_char(keyfind());  
}

int main()
{
	//Setup
	init();
	write_str("Press any key...");
	command(MOVE_CURSOR_2nd_LINE);
	KP_Setup();
	Timer0_Setup();
	sei();

    while(1){};
}