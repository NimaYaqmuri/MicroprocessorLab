// The macro F_CPU specifies the CPU frequency to be considered by the delay macros.
#define F_CPU 1000000UL

// This header file includes the apropriate IO definitions for the device.
#include <avr/io.h>
// The device library provides a set of default interrupt routines, which will get used if you don't define your own.
#include <avr/interrupt.h>

int rotation[4] = {0x05, 0x06, 0x0a, 0x09};

int T = 0;

// Timer 0 is an 8-bit timer/counter, and so it can count up to 2^8(256) such periods before it rolls over.
int clock_source = 256;

// Number of counts.
int number_of_counts = 20;

// Counter reload number.
int counter_reload_number = clock_source - number_of_counts;

int time_count = 0;

int min = 50;
int max = 1500;

int overflow_count = max;

bool on = false;
bool first_time = true;
bool clockwise = true;
bool speedup = true;
int cnt = 0;


// Timer 0 overflow Interrupt Service Routine(ISR).
ISR(TIMER0_OVF_vect)
{
    if(on)
    {
        time_count++;

        if(time_count == overflow_count)
        {
            time_count = 0;

            if(speedup)
            {
                overflow_count -= 50;

                if(overflow_count == min)
                {
                    speedup = false;
                }
            }
            else
            {
                overflow_count += 50;

                if(overflow_count == max)
                {
                    speedup = true;

                    clockwise ^= 1;
                    cnt++;

                    if(cnt == 2)
                    {
                        cnt = 0;

                        on = false; 
                    }
                }
            }

            PORTA = rotation[T]; 

            T = (clockwise) ? ((T + 1) % 4) : ((T + 3) % 4);
        }
    }
}

ISR(INT0_vect)
{
    if(!on)
    {
        on = true;
    }
    else
    {
        on = false;
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

    // Enable Timer 0.
    TIMSK = TIMSK
        | (1 << TOIE0); 

    // Enable external interrupt 0.
    GICR = (1 << INT0);        

    // Interrupt 0 is triggered by a rising edge.
    MCUCR = (1 << ISC00) | (1 << ISC01); 

    // Enables interrupts by setting the global interrupt mask.
    sei();

    // Do nothing here...
    while(true){};

    return 0;
}