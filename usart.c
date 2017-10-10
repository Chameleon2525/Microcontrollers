/*****************************************************************

Most Basic USART (RS232 Serial) Communication Support File.
Simple reading and writing of data without using
Interrupts.

BAUD RATE:57600 Bits per Second
CRYSTAL Frequency: 4MHz

Target Chip: PIC18F4520
Target Compiler: HI-TECH C For PIC18 (http://www.htsoft.com/)
Project: MPLAP Project File

*****************************************************************/

#include <htc.h>

#include "usart.h"

void USARTInit()
{

	// Baud Rate = 57600 Bits per Second
	// Ref : PIC18F4520 Datasheet Page 201
	
	// THESE MUST EB SET Too!
	TRISB0=0;
	TRISC6=1;
	TRISC7=1;
    // 4 Mhz Crystal at 31250: 4MHz/4(n+1)
    SPBRG=32;
	//TXSTA REG
	TXEN=1;
	BRGH=1;	
	//RCSTA REG
	SPEN=1;	// SERIAL PORT ENABLE BIT - configures RX/TX as serialports
	CREN=1;	//Enable Receiver (RX)
			//Continuous Receive Enable bit
			//
	//BAUDCON
	BRG16=1; // 16 bit word used for baud rate generator



}

void USARTWriteByte(char ch)
{
	//Wait for TXREG Buffer to become available
	while(!TXIF);
	//Write data
	TXREG=ch;
}

void USARTWriteString(const char *str)
{
	while((*str)!='\0')
	{
		//Wait for TXREG Buffer to become available
		while(!TXIF);

		//Write data
		TXREG=(*str);

		//Next goto char
		str++;
	}
}

/*
Writes a line of text to USART and goes to new line
The new line is Windows style CR/LF pair.
This will work on Hyper Terminal Only NOT on Linux
*/

void USARTWriteLine(const char *ln)
{
	USARTWriteString(ln);
	//USARTWriteString("\r\n");
	USARTWriteString("\n");
}

void USARTWriteInt(int val,unsigned char field_length)
{
	if(val<0) 
	{
		USARTWriteByte('-');	//Write '-' sign for negative numbers.
		val=(val*(-1));				//Make it positive.
	}

	//Convert Number To String and pump over Tx Channel.
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
		str[i]=val%10;
		val=val/10;
		i--;
	}
	if(field_length>5)
		while(str[j]==0) j++;
	else
		j=5-field_length;
	
	for(i=j;i<5;i++)
	{
		USARTWriteByte('0'+str[i]);
	}
}


unsigned char USARTReadByte()
{

	while(!RCIF);	//Wait for a byte
	
	return RCREG;
}
