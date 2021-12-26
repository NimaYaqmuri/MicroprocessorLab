// The macro F_CPU specifies the CPU frequency to be considered by the delay macros.
#define F_CPU 1000000UL

// This header file includes the apropriate IO definitions for the device.
#include <avr/io.h>
// The functions in this header file are wrappers around the basic busy-wait functions from <util/delay_basic.h>.
#include <util/delay.h>
// The device library provides a set of default interrupt routines, which will get used if you don't define your own.
#include <avr/interrupt.h>
// Standard IO facilities.
#include <stdio.h>


#define LCD_DDR  DDRA  // Define LCD Data Port Direction.
#define LCD_PORT PORTA // Define LCD Data Port.

#define RS PC0 // Define Register Select Pin.
#define EN PC1 // Define Enable Signal Pin.

// Define Data Pins.
#define D0 PC4
#define D1 PC5
#define D2 PC6
#define D3 PC7

// Send for 4 bit initialization of LCD.
// Note that we can provide any command with lower nibble as 2 i.e. 0x32, 0x42 etc.
// Except 0x22 to initialize display in 4-bit mode.
#define LCD_4BIT_INIT 0x02

#define LCD_4BIT_1LINE 0x20 // Function Set: 4-bit, 1 Line, 5×7 Dots.
#define LCD_4BIT_2LINE 0x28 // Function Set: 4-bit, 2 Line, 5×7 Dots.
#define LCD_8BIT_1LINE 0x30 // Function Set: 8-bit, 1 Line, 5×7 Dots.
#define LCD_8BIT_2LINE 0x38 // Function Set: 8-bit, 2 Line, 5×7 Dots.

#define DISPLAY_OFF_CURSOR_OFF      0x08 // Clearing display without clearing DDRAM content.
#define DISPLAY_ON_CURSOR_ON        0x0e
#define DISPLAY_ON_CURSOR_OFF       0x0c
#define DISPLAY_ON_CURSOR_BLINKING  0x0f

// Shift the cursor right (e.g. data gets written in an incrementing order, left to right).
#define INCREMENT_CURSOR 0x06 

#define SHIFT_DISPLAY_LEFT  0x18 // Shift entire display left.
#define SHIFT_DISPLAY_RIGHT 0x1c // Shift entire display right.

#define MOVE_CURSOR_LEFT  0x10 // Move cursor left  by one character.
#define MOVE_CURSOR_RIGHT 0x14 // Move cursor right by one character.

#define CLEAR_DISPLAY 0x01 // Clear Display (also clear DDRAM content).

#define MOVE_CURSOR_1st_LINE 0x80 // Force the cursor to the beginning of the 1st line.
#define MOVE_CURSOR_2nd_LINE 0xc0 // Force the cursor to the beginning of the 2nd line.

#define LCD_Data PORTB
#define LCD_Config PORTD

#define LEFT_SENSOR  PINB
#define RIGHT_SENSOR PIND

// 0: FORWARD
// 1: LEFT
// 2: RIGHT
int mode = 0;

void command(uint8_t cmd)
{
    // Sending upper 4 bits. 
    LCD_PORT = (LCD_PORT & 0x0f) | (cmd & 0xf0); 
	
    // RS = 0, command register.
    LCD_PORT &= ~ (1 << RS);		
	
    // Enable pulse.
    LCD_PORT |= (1 << EN);

	_delay_us(1);

    // Disable pulse.
	LCD_PORT &= ~ (1 << EN);

	_delay_us(200);

    // Sending lower 4 bits. 
	LCD_PORT = (LCD_PORT & 0x0f) | (cmd << 4);

    // Enable pulse.
	LCD_PORT |= (1 << EN);

	_delay_us(1);

    // Disable pulse.
	LCD_PORT &= ~ (1 << EN);

	_delay_ms(2);
}

// Write character on LCD.
void write_char(uint8_t data)
{
    // Sending upper byte. 
	LCD_PORT = (LCD_PORT & 0x0f) | (data & 0xf0);

    // RS = 1, data reg.
	LCD_PORT |= (1 << RS);

    // Enable pulse.
	LCD_PORT |= (1 << EN);

	_delay_us(1);

    // Enable pulse.
	LCD_PORT &= ~ (1 << EN);

	_delay_us(200);

    // Sending lower byte.
	LCD_PORT = (LCD_PORT & 0x0f) | (data << 4);

	LCD_PORT |= (1 << EN);

	_delay_us(1);

    // Disable pulse.
	LCD_PORT &= ~ (1 << EN);

	_delay_ms(2);
}

// Write string on LCD.
void write_str(char *str)		
{
    for (char *it = str; *it; it++)
        write_char(*it);
}

void clear(void)
{
    command(CLEAR_DISPLAY);

    _delay_ms(3);

    command(MOVE_CURSOR_1st_LINE); 
}

// LCD Initialize function.
void init(void)
{
    // Configure pins as output.
    LCD_DDR = LCD_DDR
              | (1 << RS)
              | (1 << EN)
              | (1 << D0)
              | (1 << D1)
              | (1 << D2)
              | (1 << D3);

    // Wait for LCD to become ready (docs say 15ms+).
    _delay_ms(20);

    command(LCD_4BIT_INIT);
    command(LCD_4BIT_2LINE);
    command(DISPLAY_ON_CURSOR_OFF);

    _delay_ms(3);
}

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
    DDRC = DDRC | (0x0F);

    // Set port B and D for inputs.
    DDRB = 0x00;
    DDRD = 0x00;

    // Initialize LCD
    init();

    mode = 0;
    PORTC = 1 << PORTC3;
    
    clear();
    write_str("FORWARD");

    while(true)
    {
        if ((LEFT_SENSOR & 0x7) && (mode != 1))
        {
            clear();
            write_str("LEFT");

            if     (LEFT_SENSOR & 0x01)
            { 
                PORTC = (1 << PORTC2);
            }
            else if(LEFT_SENSOR & 0x02)
            {
                PORTC = (1 << PORTC2);
                }
            else if(LEFT_SENSOR & 0x04)
            {
                PORTC = (1 << PORTC2);
            }

            mode = 1;
        }
        else if ((RIGHT_SENSOR & 0x7) && (mode != 2))
        {
            clear();
            write_str("RIGHT");

            if     (RIGHT_SENSOR & 0x01)
            {
                PORTC = (1 << PORTC1) | (1 << PORTC3);
            }
            else if(RIGHT_SENSOR & 0x02)
            {
                PORTC = (1 << PORTC1) | (1 << PORTC3);
            }
            else if(RIGHT_SENSOR & 0x04)
            {
                PORTC = (1 << PORTC1) | (1 << PORTC3);
            }

            mode = 2;
        }
        else if(!(LEFT_SENSOR & 0x7) && !(RIGHT_SENSOR & 0x7) && mode != 0)
        {
            clear();
            write_str("FORWARD");

            PORTC = (1 << PORTC3);
            mode = 0;         
        }
    };

    return 0;
}