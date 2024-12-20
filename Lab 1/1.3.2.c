/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright:  TBH i don't care if you copy it
 * License: 
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#define Maxtime 6250 // to get 20Hz (using prescaler to 250 kHz). 250kHz * 0.05 sec = 12.500 counts. Since use of toggle 12.500/2 = 6250


int main(void)
{
	_clockdivide(0); // sets the speed of the clock to 16Mhz
	set(DDRB, 1); // DDRB = Ox02;, sets the DDRB register to output
	TCCR3B = 0x04; //Scales down timer to 62.5khz (0000 0100)

	for(;;){ //start infinite for loop
		if (TCNT3>Maxtime) // if the time of timer is higher than MaxTime
		{
			toggle(PORTB, 1); //toggle PORTB
			TCNT3 = 0; //reset TCNT3 the timer back to 0
		}

 }

return 0;  // never reached
}
