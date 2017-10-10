#include <htc.h>
#include <stdio.h>
#include "lcd.h"
#include "usart.h"

//Chip Settings
__CONFIG(1,0x0200);
__CONFIG(2,0X1E1F);
__CONFIG(3,0X8100 & PBADDIS); // Disable Port B Analogue use.
__CONFIG(4,0X00C1);
__CONFIG(5,0XC00F);

#define _XTAL_FREQ 4000000

void pause (int j)
{
	int i;
	for(i=0;i<=j;++i)
	{
		__delay_ms(1);
	}
}

void NoteOn (int Pitch, int Velocity, int MIDIch)
{
		USARTWriteByte(MIDIch+144); // NoteOn on Channel 10	+144 correct decimal for noteon
		USARTWriteByte(Pitch); // Control Value on Channel 1
		USARTWriteByte(Velocity); // Velocity Value
}

void NoteOff (int Pitch,  int MIDIch)
{
		USARTWriteByte(MIDIch+128); // NoteOn on Channel 10	+128 makes correct decimal for noteoff
		USARTWriteByte(Pitch); // Control Value on Channel 1
		USARTWriteByte(0x00); // Velocity Value
}

void PanChange (int Pan,  int MIDIch)
{
		USARTWriteByte(MIDIch+176); // Control Change on Correct MIDI channel e.g 0+176 = 0xB0
		USARTWriteByte(0x0A); // Pan Controller selected
		USARTWriteByte(Pan); // Velocity Value
}

void ModulationLevel (int Modulation,  int MIDIch)
{
		USARTWriteByte(MIDIch+176); // Control Change on Correct MIDI channel e.g 0+176 = 0xB0
		USARTWriteByte(0x01); // Modulation Controller selected
		USARTWriteByte(Modulation); // Velocity Value
}

void ProgramChange(int MIDIch, int instrument)
{
		USARTWriteByte(MIDIch+192); // Program Change on correct MIDIchannel e/g 0 +192 = 0xC0
		USARTWriteByte(instrument); // instrument variable
}

void Aftertouch(int MIDIch, int Pitch, int Pressure)
{
		USARTWriteByte(MIDIch+160); // Program Change on correct MIDIchannel e/g 0 +160 = 0xA0
		USARTWriteByte(Pitch); // Note Pitch
		USARTWriteByte(Pressure); //veltotal value used for aftertouch function
}


void init(void)
{
	ADCON0 = 0; //defaults to off
	ADCON1 = 0b1101; 
	ADIE=0;	//not interrupt driven
	USARTInit();
    TRISD=0;
	TRISB = 0b00110001; //Enable RB0, RB4 & RB5 as inputs
	TRISA = 0b00111111;//Ebable RA0-RA5 as inputs
	TRISC = 0;
	lcd_init();
	ADFM=1; //right justification
}

void main(void)
{
	int Pitch; //Decimal of total ADC
	int Veltotal; //Decimal of total ADC
	int Modulation; //Decimal of total ADC
	int Pan; //Decimal of total ADC
	unsigned char outString[15] = "";
	unsigned char adc_left, adc_right, last_value;
	unsigned char MIDI;
	unsigned char ledtest;

	char state_machine = 0; //RB0 Variable
	char state_machine1 = 0; //RA4 Variable
	char state_machine2 = 0; //RA5 Variable
	char state_machine3 = 0; //RB5 Variable
	char state_machine4 = 0; //RB5 Variable

	unsigned char channel = 1;
	unsigned char MIDIchannel = 0;
	int instrument = 0; //program change variable
	unsigned char count = 0;
	char counter=0;
	init();

    lcd_goto(0);	// select first line
	lcd_puts("Mills MIDI:");
	lcd_goto(0x40);	// select second line
	lcd_puts("The Only Choice");
	PORTB = ledtest++;
    pause(3000);
	lcd_clear();



	while(1)
	{
		//MIDI Edit Select
		if((RB0==0)&&(state_machine==0))
		{
			state_machine=1;
			channel++;
			if(channel >4)
				{
					channel = 1;
				}
		}
		if ((RB0==1)&&(state_machine==1))
		{
			state_machine=0;
		}

		//Note On/Note Off
		if ((RA4==0)&&(state_machine1==0))
		{
			state_machine1=1;
			PanChange(Pan,MIDIchannel);//send pan value to pan function
			ModulationLevel (Modulation,  MIDIchannel); //send reverb value
			NoteOn(Pitch,Veltotal, MIDIchannel);
			Aftertouch(MIDIchannel,Pitch,Veltotal);
			
		}
		if((RA4==1)&&(state_machine1==1))
		{
			state_machine1=0;
			NoteOff(Pitch, MIDIchannel);
		}
	
		//MIDI Channel Select
		if ((RA5==1)&&(state_machine2==0))
		{
			state_machine2=1;
			MIDIchannel++;
			if(MIDIchannel >15)
				{
					lcd_goto(0x40);	// select first line
					lcd_puts("               "); // Clear line
					MIDIchannel = 0;
				}
		}
		if((RA5==0)&&(state_machine2==1))
		{
			state_machine2=0;
		}

		//MIDI Instrument Increment
		if ((RB5==1)&&(state_machine3==0))
		{
			state_machine3=1;
			instrument++;
			if(instrument >127)
				{
					instrument = 0;
				}

			//Instrument Number Display
			lcd_clear();
            lcd_goto(0);	// select first line
	        lcd_puts("MIDI Instrument");
			sprintf(outString,"Number: %d", instrument);
			lcd_goto(0x40);	// select second line
			lcd_puts(outString);
			ProgramChange(MIDIchannel,instrument);
			pause(1500); 
		}
		if((RB5==0)&&(state_machine3==1))
		{
			state_machine3=0;
		}


		//MIDI Instrument Decrement
		if ((RB4==1)&&(state_machine4==0))
		{
			state_machine4=1;
			instrument--;
			if(instrument <0)
				{
					instrument = 127;
				}

			//Instrument Number Display
			lcd_clear();
            lcd_goto(0);	// select first line
	        lcd_puts("MIDI Instrument");
			sprintf(outString,"Number: %d", instrument);
			lcd_goto(0x40);	// select second line
			lcd_puts(outString);
			ProgramChange(MIDIchannel,instrument);
			pause(1500); 

		}
		if((RB4==0)&&(state_machine4==1))
		{
			state_machine4=0;
		}


		//MIDI Channel Number Display
		sprintf(outString,"MIDIChannel =%d", MIDIchannel);
		lcd_goto(0x40);	// select second line
		lcd_puts(outString);


		//Velocity Channel
		if(channel ==1)
		{
			ADCON0 = 0b00000111; 	// Select channel 1 of ADCON0, GODONE = 1
			NOP();
			NOP(); 	
			NOP();
			NOP();
		    while(GODONE)continue;
			ADIF=0;
			adc_left=ADRESL;
			adc_right = ADRESH;
			Veltotal = 256*adc_right+adc_left; //adresL has a bitvalue of up to 255, adresH is a multiple of 256
			Veltotal = Veltotal/8; //selects first channel with range of 127
			
			if(adc_left!=last_value)
			{      
		    	lcd_goto(0);	// select first line
				lcd_puts("               "); // Clear line if the value is different
			}

			last_value=adc_left;
		    pause(250);

			sprintf(outString,"Volume=%d", Veltotal);
			lcd_clear();
	    	lcd_goto(0);	// select first line
			lcd_puts(outString);
			PORTB = Veltotal;
		}

		//Pitch Channel
		if(channel ==2)
		{	
			ADCON0 = 0b00001011; 	// Select channel 2 of ADCON0, GODONE = 1
			NOP();
			NOP(); 	
			NOP();
			NOP();
		    while(GODONE)continue;
			ADIF=0;
			adc_left=ADRESL;
			adc_right = ADRESH;
			Pitch = 256*adc_right+adc_left; //adresL has a bitvalue of up to 255, adresH is a multiple of 256

			Pitch = Pitch/8; // range of 127	
			if(adc_left!=last_value)
			{      
		    	lcd_goto(0);	// select first line
				lcd_puts("               "); // Clear line if the value is different
			}


			sprintf(outString,"Pitch=%d", Pitch);
	    	lcd_goto(0);	// select first line
			lcd_puts("               "); //clear line
	    	lcd_goto(0);	// select first line
			lcd_puts(outString);


			PORTB = Pitch;
		}

		//Pan Channel
		if(channel ==3)
		{
			ADCON0 =0b00001111; 	// Select channel 3 of ADCON0, GODONE = 1
			NOP();
			NOP(); 	
			NOP();
			NOP();
		    while(GODONE)continue;
			ADIF=0;
			adc_left=ADRESL;
			adc_right = ADRESH;
			Pan = 256*adc_right+adc_left; //adresL has a bitvalue of up to 255, adresH is a multiple of 256

			Pan = Pan/8; // range of 127	
			if(adc_left!=last_value)
			{      
		    	lcd_goto(0);	// select first line
				lcd_puts("               "); // Clear line if the value is different
			}

			sprintf(outString,"Pan=%d", Pan);
	    	lcd_goto(0);	// select first line
			lcd_puts("               "); //clear line
	    	lcd_goto(0);	// select first line
			lcd_puts(outString);

			PORTB = Pan;
		}

		if(channel ==4)
		{
			ADCON0 =0b00000011; 	// Select channel 0 of ADCON0, GODONE = 1
			NOP();
			NOP(); 	
			NOP();
			NOP();
		    while(GODONE)continue;
			ADIF=0;
			adc_left=ADRESL;
			adc_right = ADRESH;
			Modulation= 256*adc_right+adc_left; //adresL has a bitvalue of up to 255, adresH is a multiple of 256

			Modulation = Modulation/8; // range of 127

			if(adc_left!=last_value)
			{      
		    	lcd_goto(0);	// select first line
				lcd_puts("               "); // Clear line if the value is different
			}


			sprintf(outString,"Modulation=%d",Modulation);
	    	lcd_goto(0);	// select first line
			lcd_puts("               "); //clear line
	    	lcd_goto(0);	// select first line
			lcd_puts(outString);
		} 
	}
} 
