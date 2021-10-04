#define F_CPU 16000000UL

/* The functions in this header file are wrappers around the basic busy-wait functions from <util/delay_basic.h>. */
#include <util/delay.h>
/* This header file declares the interface to some simple library routines suitable for handling the data EEPROM contained in the AVR microcontrollers. */
#include <avr/eeprom.h>

// Define desired size of the string.
const unsigned char SIZE = 200;

int main()
{
    // The string we want to send from the sender to the receiver.
    char string[SIZE] = "AVR is a family of microcontrollers developed since 1996 by Atmel, acquired by Microchip Technology in 2016. These are modified Harvard architecture 8-bit RISC single-chip microcontrollers. That's it";

    // Set all bits of port A for output.
    DDRA = 0xff;

    // Initialize port A to 0. 
    PORTA = 0;

    // Loops until the eeprom is no longer busy.
    eeprom_busy_wait();

    // Write a block of SIZE bytes to EEPROM address 0 from the string.
    eeprom_write_block(string, 0, SIZE);
    
    // Perform a delay of 1ms milliseconds
    _delay_us(1);

    // Write the string in port A.
    for (int i = 0; string[i] != 0; i++)
    {
        // Write the i'th byte from the string to port A.
        for(int j = 0; j != 8; j++)
        {
            PORTA = (string[i] >> j) & 1;
            _delay_us(200);
        }

        // Perform a delay of 1 milliseconds
        // _delay_us(100);
    }

    // Set port A to 0. 
    PORTA = 0;

    return 0;
}
