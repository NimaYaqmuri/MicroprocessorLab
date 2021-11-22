//----- Headers ------------//
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
//--------------------------//

//----- Auxiliary definitions -----//
enum KeypadKey_t
{
	Key_Pressed	 = 0xFF,
	Key_None	 = 0x00,

	Key_0		 = '0',
	Key_1		 = '1',
	Key_2		 = '2',
	Key_3		 = '3',
	Key_4		 = '4',
	Key_5		 = '5',
	Key_6		 = '6',
	Key_7		 = '7',
	Key_8		 = '8',
	Key_9		 = '9',

	Key_A		 = 'A',
	Key_B		 = 'B',
	Key_C		 = 'C',
	Key_D		 = 'D',
	Key_E		 = 'E',
	Key_F		 = 'F',

	Key_Hashtag  = '#',
	Key_Asterisk = '*',
	Key_Plus     = '+',
	Key_Hyphen   = '-',
	Key_divide   = '/',
	Key_Equals   = '='
};
//---------------------------------//

//----- Configuration --------------------------//
#define KP_DDR DDRC 	//Define Keypad Data Port Direction.

//Define Keypad Data Port.
#define KP_PRT  PORTC
#define KP_PRT0 PORTC0
#define KP_PRT1 PORTC1
#define KP_PRT2 PORTC2
#define KP_PRT3 PORTC3
#define KP_PRT4 PORTC4
#define KP_PRT5 PORTC5
#define KP_PRT6 PORTC6
#define KP_PRT7 PORTC7

//Define Keypad Data Pin.
#define KP_PIN  PINC
#define KP_PIN0 PINC0
#define KP_PIN1 PINC1
#define KP_PIN2 PINC2
#define KP_PIN3 PINC3
#define KP_PIN4 PINC4
#define KP_PIN5 PINC5
#define KP_PIN6 PINC6
#define KP_PIN7 PINC7

//Rows
#define Keypad_R0 PC0
#define Keypad_R1 PC1
#define Keypad_R2 PC2
#define Keypad_R3 PC3

//Columns
#define Keypad_C0 PC4
#define Keypad_C1 PC5
#define Keypad_C2 PC6
#define Keypad_C3 PC7

#define KEY_PRT 	PORTC
#define KEY_DDR		DDRC
#define KEY_PIN		PINC
//----------------------------------------------//

//----- I/O Macros -----
//General use bit manipulating commands
#define BitSet(x, y)    (  x |=   (1UL << y)          )
#define BitClear(x, y)  (  x &= (~(1UL << y))         )
#define BitToggle(x, y) (  x ^=   (1UL << y)          )
#define BitCheck(x, y)  (  x &    (1UL << y) ? 1 : 0  )
//----------------------

//----- Auxiliary data -----------
//ISR debounce states (1 <= x <= 255)
#define Keypad_ISR_Max		10

volatile char __KP_ISR_Buffer[Keypad_ISR_Max];

#define __Keypad_Rows		4
#define __Keypad_Columns	4

const char __KP_Map[4][4] PROGMEM =
{
	{Key_7, Key_8, Key_9,      Key_divide},
	{Key_4, Key_5, Key_6,      Key_Asterisk},
	{Key_1, Key_2, Key_3,      Key_Hyphen},
	{Key_C, Key_0, Key_Equals, Key_Plus}
};
//--------------------------------

//----- Prototypes -----------------//
static uint8_t ReadRows(void);
static uint8_t ReadColumns(void);
static void ToggleRC(void);
//----------------------------------//

//----- Functions ------------------------//
//Setup for keypad. 
void KP_Setup(void)
{
	//Set outputs.
	KP_DDR |= 0xF0;

	//Configure pull-up resistors and low level outputs.
	KP_PRT |= 0x0F;
}


//Check if key is pressed. Returns Key_Pressed or Key_None. 
enum KeypadKey_t KP_KeyPressed(void)
{
	//Read port
	uint8_t port = ~ReadRows();
	
	//If port has any value, then a key was pressed
	if (port)
		port = Key_Pressed;

	return static_cast<KeypadKey_t>(port);
}

//Wait until no key is held. 
void KP_WaitRelease(void)
{
	while (KP_KeyPressed() != Key_None);
}

//Read keypad's rows. 
static inline uint8_t ReadRows(void)
{
	volatile uint8_t port = 0xF0;

	//Read each pin to its respected place
	port |= (BitCheck(KP_PIN, Keypad_R0) << Keypad_R0);
	port |= (BitCheck(KP_PIN, Keypad_R1) << Keypad_R1);
	port |= (BitCheck(KP_PIN, Keypad_R2) << Keypad_R2);
	port |= (BitCheck(KP_PIN, Keypad_R3) << Keypad_R3);
	
	return port;
}

//Read keypad's columns. 
static inline uint8_t ReadColumns(void)
{
	volatile uint8_t port = 0x0F;

	//Read each pin to its respected place
	port |= (BitCheck(KP_PIN, Keypad_C0) << Keypad_C0);
	port |= (BitCheck(KP_PIN, Keypad_C1) << Keypad_C1);
	port |= (BitCheck(KP_PIN, Keypad_C2) << Keypad_C2);
	port |= (BitCheck(KP_PIN, Keypad_C3) << Keypad_C3);
	
	return port;
}

//Toggle all keypad pins. 
static void ToggleRC(void)
{
	//Toggle pin mode (Inputs <-> Outputs)
	KP_DDR ^= 0xFF;
	
	//Toggle pin level (High <-> Low)
	KP_PRT ^= 0xFF;
}
//-----------------------------------------//

uint8_t colloc, rowloc;

char keyfind()
{
	while(1)
	{
		do
		{
			KEY_PRT &= 0x0F;
			asm("NOP");
			colloc = (KEY_PIN & 0x0F);
		}while(colloc != 0x0F);
		
		do
		{
			do
			{
				_delay_ms(20);						//20ms key debounce time
				colloc = (KEY_PIN & 0x0F);
			}while(colloc == 0x0F);
			
			_delay_ms (40);							//20 ms key debounce time
			colloc = (KEY_PIN & 0x0F);
		}while(colloc == 0x0F);

		KEY_PRT = 0xEF;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 0;
			break;
		}

		KEY_PRT = 0xDF;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 1;
			break;
		}
		
		KEY_PRT = 0xBF;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 2;
			break;
		}

		KEY_PRT = 0x7F;
		asm("NOP");
		colloc = (KEY_PIN & 0x0F);
		if(colloc != 0x0F)
		{
			rowloc = 3;
			break;
		}
	}

	
	if(colloc == 0x0E)
	return pgm_read_byte(&(__KP_Map[0][rowloc]));
	
	else if(colloc == 0x0D)
	return pgm_read_byte(&(__KP_Map[1][rowloc]));
	
	else if(colloc == 0x0B)
	return pgm_read_byte(&(__KP_Map[2][rowloc]));
	
	else
	return pgm_read_byte(&(__KP_Map[3][rowloc]));
}