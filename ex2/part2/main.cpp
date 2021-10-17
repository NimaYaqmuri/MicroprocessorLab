/* The macro F_CPU specifies the CPU frequency to be considered by the delay macros. */
#define F_CPU 1000000UL

/* NOTE: F_CPU is only a way for you to tell _delay_ms() how fast your MCU is running,
   it doesn't in any way change the speed of the MCU. */

/* This header file includes the apropriate IO definitions for the device. */
#include <avr/io.h>
/* The functions in this header file are wrappers around the basic busy-wait functions from <util/delay_basic.h>. */
#include <util/delay.h>

int main()
{
    // Set port A for input.
    DDRA = 0;

    // Set all bits of port B for output.
    DDRB = 0xff;

    // Desired cycle time in seconds.
    double cycle_time = 0.5;

    // Calculate delay time in milliseconds.
    double delay_time = cycle_time * 1000;

    // Wait until the button is pressed...
    while (!(PINA & 1));
    
    while (true)
    {
        // Invert all bits of port A.
        PORTB ^= 0xff;

        // Perform a delay of "delay_time" milliseconds.
        _delay_ms(delay_time);
    }

    return 0;
}
