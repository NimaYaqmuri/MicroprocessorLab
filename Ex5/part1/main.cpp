// The macro F_CPU specifies the CPU frequency to be considered by the delay macros.
#define F_CPU 1000000UL

// This header file includes the apropriate IO definitions for the device.
#include <avr/io.h>
// The device library provides a set of default interrupt routines, which will get used if you don't define your own.
#include <avr/interrupt.h>
// The functions in this header file are wrappers around the basic busy-wait functions from <util/delay_basic.h>.
#include <util/delay.h>

int rotation[4] = {0x05, 0x06, 0x0a, 0x09};

uint8_t T = 0;

// Timer 0 is an 8-bit timer/counter, and so it can count up to 2^8(256) such periods before it rolls over.
int clock_source = 256;

// Number of counts.
int number_of_counts = 20;

// Counter reload number.
int counter_reload_number = clock_source - number_of_counts;

int time_count = 0;

int overflow_count = 350;

bool on = false;

// Timer 0 overflow Interrupt Service Routine(ISR).
ISR(TIMER0_OVF_vect)
{    
    time_count++;

    if(time_count == overflow_count)
    {
        time_count = 0;

        PORTA = rotation[T++];    
        T %= 4;
    }
}

int main()
{
    // Set first 4 bits of port A for output.
    DDRA = DDRA
           | (0x0f);

    // Set Timer 0 prescaler to System Clock.
    TCCR0 = TCCR0
            | (1 << CS00);

    // Set timeout for Timer 0.
    TCNT0 = counter_reload_number;

    // Enables interrupts by setting the global interrupt mask.
    sei();

    // Do nothing here...
    while(true)
    {
        if (!((PINC >> PINC2) & 1))
        {
            if(!on)
            {
                on = true;

                // Enable Timer 0.
                TIMSK = TIMSK
                    | (1 << TOIE0); 
            }
            else
            {
                on = false;

                // Disable Timer 0.
                TIMSK = TIMSK & 0;
            }

            _delay_ms(500);
        }
    }

    return 0;
}