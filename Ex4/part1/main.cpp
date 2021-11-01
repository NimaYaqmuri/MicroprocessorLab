// This header file includes the apropriate IO definitions for the device. */
#include <avr/io.h>
// The functions in this header file are wrappers around the basic busy-wait functions from <util/delay_basic.h>. */
#include <util/delay.h>


#define LCD_DDR  DDRA  // Define LCD Data Port Direction.
#define LCD_PORT PORTA // Define LCD Data Port.

#define RS PA0 // Define Register Select Pin.
#define EN PA1 // Define Enable Signal Pin.

// Define Data Pins.
#define D0 PA4
#define D1 PA5
#define D2 PA6
#define D3 PA7

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
    command(DISPLAY_ON_CURSOR_ON);

    _delay_ms(3);
}

int main()
{
    char s[] = "Hello Nima!";

    init();

    write_str(s);

    // Do nothing...
    while(true){}

    return 0;
}