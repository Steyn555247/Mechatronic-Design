/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright: TBH i don't care if you copy it
 * License: 
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#include <util/delay.h> // includes util/delay.h
#include <stdint.h> // for using uint8_t type

int main(void)
{
set(DDRB, PB; // Set PB1 as output
uint8_t dutyCycle = 0; // Duty cycle in percentage (50% for half on, half off, 0 for off and 100 for on)

while (1){

// Turn on the LED for the "on" portion of the cycle
	if(dutyCycle > 0){
		
		PORTB |= (1 <<PB1); //turn on LED by setting PB1 High
		_delay_ms(dutyCycle * 5); // On time multiplied by 5 to simulate percentage over 500ms
}


	if(dutyCycle < 100){ // Turn off the LED for the "off" portion of the cycle

		PORTB &= ~(1 <<PB1); //Turn off LED by setting PB1 low
		_delay_ms((100 - dutyCycle) *5); //Off time * by 5
	}

	// You can adjust the duty cycle by manually changing the dutyCycle variable. dutyCycle=0; means fully off, dutyCycle =100; means fully on.

 }

return 0;  // never reached
}
