/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright: I got it from ChatGPT so you can get it from me. (yeah that rhymes)
 * License: 
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#define MaxTime 6250


int main(void)
{
	_clockdivide(0); // sets the speed of the clock to 16Mhz
	set(DDRB, 1); // DDRB = Ox02;, sets the DDRB register to output
	TCCR3B = 0x04; //Scales down timer to 62.5khz (0000 0100)

	for(;;){ //start infinite for loop
		if (TCNT3>MaxTime) // if the time of timer is higher than MaxTime
		{
			toggle(PORTB, 1); //toggle PORTB
			TCNT3 = 0; //rest TCNT3
		}

 }

return 0;  // never reached
}