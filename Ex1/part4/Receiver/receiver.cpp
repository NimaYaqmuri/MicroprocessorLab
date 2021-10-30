#define F_CPU 16000000UL

/* This header file includes the apropriate IO definitions for the device. */
#include <avr/io.h>
/*The string functions perform string operations on NULL terminated strings. */
#include <string.h>
/* The functions in this header file are wrappers around the basic busy-wait functions from <util/delay_basic.h>. */
#include <util/delay.h>

/* This is an avr-gcc library for the HD44780 character LCD display: https://github.com/aostanin/avr-hd44780.git */
#include <stdarg.h>
#include <stdio.h>

/* Define LCD data port direction */
#define LCD_DDR  DDRA 
/* Define LCD data port */
#define LCD_PORT PORTA

/* Define Register Select pin */
#define LCD_RS PA0
/* Define Enable signal pin */
#define LCD_EN PA1
#define LCD_D0 PA4
#define LCD_D1 PA5
#define LCD_D2 PA6
#define LCD_D3 PA7

// send for 4 bit initialization of LCD
#define LCD_4BIT_INITIALIZATION 0x02
// 2 line, 5*7 matrix in 4-bit mode
#define ONE_LINE_4BIT_MODE 0x20
#define LCD_4BIT_matrix 0x28
#define DISPLAY_ON_CURSOR_OFF 0x0C
#define INCREMENT_CURSOR 0x06
#define CLEAR_DISPLAY 0x01
#define CURSOR_HOME_POSITION 0x80

void lcd_command(uint8_t cmd)
{
    // Sending upper byte. 
    LCD_PORT = (LCD_PORT & 0x0F) | (cmd & 0xF0); 
	
    // RS = 0, command register.
    LCD_PORT &= ~ (1 << LCD_RS);		
	
    // Enable pulse
    LCD_PORT |= (1 << LCD_EN);

	_delay_us(1);

    // Disable pulse
	LCD_PORT &= ~ (1 << LCD_EN);

	_delay_us(200);

    // Sending lower byte. 
	LCD_PORT = (LCD_PORT & 0x0F) | (cmd << 4);

    // Enable pulse
	LCD_PORT |= (1 << LCD_EN);

	_delay_us(1);

    // Disable pulse
	LCD_PORT &= ~ (1 << LCD_EN);

	_delay_ms(2);
}

void lcd_write(uint8_t data)
{
    // Sending upper byte. 
	LCD_PORT = (LCD_PORT & 0x0F) | (data & 0xF0);

    // RS=1, data reg.
	LCD_PORT |= (1 << LCD_RS);

    // Enable pulse
	LCD_PORT |= (1 << LCD_EN);

	_delay_us(1);

    // Enable pulse
	LCD_PORT &= ~ (1 << LCD_EN);

	_delay_us(200);

    // Sending lower byte.
	LCD_PORT = (LCD_PORT & 0x0F) | (data << 4);

	LCD_PORT |= (1 << LCD_EN);

	_delay_us(1);

    // Disable pulse
	LCD_PORT &= ~ (1 << LCD_EN);

	_delay_ms(2);
}

/* Send string to LCD function */
void lcd_puts(char *str)		
{
    for (char *it = str; *it; it++){
        lcd_write(*it);
    }
}

void lcd_clear(void)
{
    lcd_command(CLEAR_DISPLAY);

    _delay_ms(2);

    lcd_command(CURSOR_HOME_POSITION); 
}

/* LCD Initialize function */
void lcd_init(void)
{
    // Configure pins as output
    LCD_DDR = 0xFF;
    // LCD_DDR = LCD_DDR
    //             | (1 << LCD_RS)
    //             | (1 << LCD_EN)
    //             | (1 << LCD_D0)
    //             | (1 << LCD_D1)
    //             | (1 << LCD_D2)
    //             | (1 << LCD_D3);

    // Wait for LCD to become ready (docs say 15ms+)
    _delay_ms(20);

    lcd_command(LCD_4BIT_INITIALIZATION);		
    lcd_command(LCD_4BIT_matrix);              
    lcd_command(DISPLAY_ON_CURSOR_OFF);              
    lcd_command(INCREMENT_CURSOR);
    lcd_clear();

    _delay_ms(3);
}

int main()
{
    // Define desired size of the string.
    const unsigned char SIZE = 200;

    // The string we want to receive from the sender to the receiver.
    char str[SIZE];
    
    // Set all bits of port C for input.
    DDRC = 0;

    // hangs here waiting while the port C pins is not low.
    while (PINC == 0){}

    // Read the string from port C.
    for (int i = 0; i != SIZE; i++)
    {
        // Read the value on the port C pins.
        str[i] = PINC;

        // Perform a delay of 1 milliseconds
        _delay_ms(1);
    }

    // Initialize LCD
    lcd_init();

    int substr_size = 10;

    for (int i = 0; i != 200 / substr_size; i++)
    {
        char substr[substr_size];

        lcd_clear();

        lcd_puts(strncpy(substr, str + i * substr_size, substr_size));

        _delay_ms(100);
    }

    return 0;
}