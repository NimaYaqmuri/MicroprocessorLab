/* This header file includes the apropriate IO definitions for the device. */
#include <avr/io.h>
/* The device library provides a set of default interrupt routines, which will get used if you don't define your own. */
#include <avr/interrupt.h>

// Display numbers on a 7SEG-COM-CATHODE.
const unsigned char ZERO  = 0x3f;
const unsigned char ONE   = 0x06;
const unsigned char TWO   = 0x5b;
const unsigned char THREE = 0x4f;
const unsigned char FOUR  = 0x66;
const unsigned char FIVE  = 0x6d;
const unsigned char SIX   = 0x7d;
const unsigned char SEVEN = 0x07;
const unsigned char EIGHT = 0x7f;
const unsigned char NINE  = 0xef;

const unsigned char DIGITS[10] = {ZERO, ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE};

// Timer 1 is an 16-bit timer/counter, and so it can count up to 2^16(65536) such periods before it rolls over.
int clock_source = 65536;

// Number of counts.
int number_of_counts = 65526;

// Counter reload number.
int counter_reload_number = clock_source - number_of_counts;

unsigned char cnt = 0;

// Timer 1 overflow Interrupt Service Routine(ISR).
ISR(TIMER1_OVF_vect)
{
    // Set timeout.
    TCNT1 = counter_reload_number;

    // Take remaining 10.
    cnt %= 10;

    // Set appropriate number.
    PORTD = DIGITS[cnt++];
}

int main()
{
    // Set LSB of port C for output.
    DDRC = 0x01;

    // Set necessary bits of port D for output. 
    DDRD = 0x7f;

    // Enable 7SEG-COM-CATHODE.
    PORTC = 0x00;

    // Set timer 1 prescaler to system clock / 8.
    TCCR1B = TCCR1B
            | (1 << CS11);

    // Set timeout.
    TCNT1 = counter_reload_number;

    // Unmask Timer 1 overflow interrupt
    TIMSK = TIMSK
            | (1 << TOIE1);

    // Enable interrupts.
    sei();

    // Do nothing...
    while(true){};

    return 0;
}
