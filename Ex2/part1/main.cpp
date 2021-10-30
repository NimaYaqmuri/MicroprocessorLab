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
    // Set first bit of port A for output.
    DDRA = 0x01;

    // Desired cycle time in seconds.
    double cycle_time = 0.5;

    // Calculate delay time in milli seconds.
    double delay_time = cycle_time * 1000;

    while (true)
    {
        // Invert first bit of port A.
        PORTA ^= 1;

        // Perform a delay of "delay_time" milliseconds.
        _delay_ms(delay_time);
    }

    return 0;
}
