#include <htc.h>
extern volatile char counter;

void interrupt my_isr(void){
	if((TMR0IE)&&(TMR0IF)){
	counter++;
	TMR0IF=0;
	if(counter == 255)
	{
		INT0IF = 0;
		counter = 0;
	}
}
