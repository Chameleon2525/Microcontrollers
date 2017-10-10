/*
 *	LCD interface example
 *	
 *	This code will interface to a standard LCD controller
 *	like the Hitachi HD44780. It uses it in 4 bit mode, with
 *	the hardware connected as follows (the standard 14 pin 
 *	LCD connector is used):
 *				
 *	PORTD bits 0-3 are connected to the LCD data bits 4-7 (high nibble)	RD:0-3
 *	PORTD bit 4 is connected to the LCD RS input (register select) RD:4
 *	PORTD bit 5 is connected to the LCD R/W bit (enable)    RD:5
 *	PORTD bit 6 is connected to the LCD EN bit (enable)		RD:6
 *	PORT RD7 is a control line which us used to switch power to the LCD module
 *	To use these routines, set up the port I/O (TRISA, TRISB) then TRISD
 *	call lcd_init(), then other routines as required.
 *	
 */

#include	<htc.h>
#include	"lcd.h"

#define _XTAL_FREQ 4000000		//We're using a 4Mhz Crystal Oscillator
#define	LCD_RS RD4 				//was RA3 on non ROHS board
#define	LCD_RW RD5 				//was RA2 on non ROHS board
#define LCD_EN RD6 				//was RA1 on non ROHS board
#define	LCD_PWR RD7				//now connected to control the LCD's power line
#define LCD_DATA PORTD		    //all lines of LCD connected to portD

//macros needed
#define	LCD_STROBE()				((LCD_EN = 1),(LCD_EN=0))

/* write a byte to the LCD in 4 bit mode */
void lcd_write(unsigned char c)
{
	unsigned char Ln, Hn;
	
	Hn = PORTD & 0xD0;					//store the existing high nibble as it has the port controlling bits
	Ln =  c >> 4 ;							//store upper byte of the character to send to the LCD as the first of two data bytes

	__delay_us(40);							//delay for proper timing
	LCD_DATA =Hn + Ln;					//now combine the existing port control bits with the upper data bits		
	LCD_STROBE();								//send to the display
	
	Ln = c & 0x0F;							//now get the lower byte of the character to send to the LCD as the second of two bytes
	LCD_DATA = Hn + Ln;					//now combine the existing port control bits with the upper data bits
	LCD_STROBE();								//send to the display
}

/* Clear and home the LCD */
void lcd_clear(void)
{
	LCD_RS = 0; // Specifies a command write
	lcd_write(0x1);
	__delay_ms(2);
}

/* write a string of chars to the LCD */
void lcd_puts(const char * s)
{
	LCD_RS = 1;	// Specifies a data write
	while(*s)
		lcd_write(*s++);
}

/* write one character to the LCD */
void lcd_putch(char c)
{
	LCD_RS = 1;		// Specifies a data write
	lcd_write( c );
}

/* Go to the specified position */
void lcd_goto(unsigned char pos)
{
	LCD_RS = 0;
	lcd_write(0x80+pos); //0x80 = decimal 128
}
	
/* initialise the LCD - put into 4 bit mode */
void lcd_init()
{
	LCD_RS = 0;
	LCD_EN = 0;
	LCD_RW = 0;
	LCD_PWR = 1;				//turns on the LCD power and it should remain on
	
	__delay_ms(15);			// wait 15mSec after power applied,

	//LCD_DATA = 0x3; 	//can't use since RD7 must remain high to keep display on
	LCD_DATA = 0x83;	//So do this..

	LCD_STROBE();
	__delay_ms(5);
	LCD_STROBE();
	__delay_us(200);
	LCD_STROBE();
	__delay_us(200);

	//	LCD_DATA = 2;		//can't use since RD7 must remain high to keep display on (set 4 bit mode)
	LCD_DATA = 0x82;		//So do this..	

	LCD_STROBE();

	lcd_write(0x28); 		// Set interface length (decimal 40)
	lcd_write(0xF); 		// Display On, Cursor On, Cursor Blink
	lcd_clear();			// Clear screen
	lcd_write(0x6); 		// Set entry Mode
}
