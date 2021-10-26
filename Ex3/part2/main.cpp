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

// Timer 0 is an 8-bit timer/counter, and so it can count up to 2^8(256) such periods before it rolls over.
int clock_source0 = 256;

// Number of counts.
int number_of_counts0 = 100;

// Counter reload number.
int counter_reload_number0 = clock_source0 - number_of_counts0;

// Timer 1 is an 16-bit timer/counter, and so it can count up to 2^16(65536) such periods before it rolls over.
int clock_source1 = 65536;

// Number of counts.
int number_of_counts1 = 65530;

// Counter reload number.
int counter_reload_number1 = clock_source1 - number_of_counts1;

unsigned char cnt0 = 0;
unsigned char cnt1 = 0;
unsigned char cnt2 = 0;
unsigned char cnt3 = 0;

bool seg0 = true;
bool seg1 = false;
bool seg2 = false;
bool seg3 = false;

unsigned char which_7seg = 0;

ISR(TIMER0_OVF_vect)
{
    if(seg0)
    {
        // Set appropriate number.
        PORTD = DIGITS[cnt0];

        // Enable target 7SEG-COM-CATHODE and disable others.
        PORTC = 0x07;

        seg0 = false;
        seg1 = true;
    } 
    else if(seg1)
    {
        // Set appropriate number.
        PORTD = DIGITS[cnt1];

        // Enable target 7SEG-COM-CATHODE and disable others.
        PORTC = 0x0b;
        
        seg1 = false;
        seg2 = true;
    } 
    else if(seg2)
    {
        // Set appropriate number.
        PORTD = DIGITS[cnt2];

        // Enable target 7SEG-COM-CATHODE and disable others.
        PORTC = 0x0d;
        
        seg2 = false;
        seg3 = true;
    } 
    else if(seg3)
    {
        PORTD = DIGITS[cnt3];

        // Enable target 7SEG-COM-CATHODE and disable others.
        PORTC = 0x0e;
        
        seg3 = false;
        seg0 = true;
    }

    
}

// Timer 1 overflow Interrupt Service Routine(ISR).
ISR(TIMER1_OVF_vect)
{
    // Set timeout.
    TCNT1 = counter_reload_number1;

    // Increment.
    cnt0++;
    cnt1 += cnt0 / 10;
    cnt2 += cnt1 / 10;
    cnt3 += cnt2 / 10;

    // Take remaining 10.
    cnt0 %= 10;
    cnt1 %= 10;
    cnt2 %= 10;
    cnt3 %= 10;
}

int main()
{
    // Set 4 bits of port C for output.
    DDRC = 0x0f;

    // Set 7 bits of port D for output. 
    DDRD = 0x7f;

    // Enable 7SEG-COM-CATHODE.
    PORTC = 0x00;

    // Set timer 0 prescaler to system clock / 8.
    TCCR0 = TCCR0
            | (1 << CS01);
    
    // Set timer 1 prescaler to system clock / 8.
    TCCR1B = TCCR1B
            | (1 << CS11);


    // Set timeout for timer 0.
    TCNT0 = counter_reload_number0;

    // Set timeout for timer 1.
    TCNT1 = counter_reload_number1;

    // Unmask Timer 0 & Timer 1 overflow interrupts.
    TIMSK = TIMSK
            | (1 << TOIE0)
            | (1 << TOIE1);

    // Enable interrupts.
    sei();

    // Do nothing...
    while(true){};

    return 0;
}
