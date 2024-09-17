/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright:  TBH i don't care if you copy it
 * License: 
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#define Maxtime 6250


int main(void)
{
	_clockdivide(0); // sets the speed of the clock to 16Mhz
	set(DDRB, 1); // DDRB = Ox02;, sets the DDRB register to output
	TCCR3B = 0x03; //Scales down timer to 250KHz (0000 0011)

	for(;;){ //start infinite for loop
		if (TCNT3>Maxtime) // if the time of timer is higher than MaxTime
		{
			toggle(PORTB, 1); //toggle PORTB
			TCNT3 = 0; //rest TCNT3
		}

 }

return 0;  // never reached
}
