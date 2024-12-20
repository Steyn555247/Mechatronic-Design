/* Name: main.c
 * Author: Steyn Knollema
 * Description: 2.1.2
 * Copyright: I dont care if you copy it
 * Used ChatGPT as a debugger
 */

#include "MEAM_general.h"  // General utility macros
#include "m_usb.h" // Initialize the USB print

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13) // got this from ChatGPT to make the numbers print on new row each time.

int main(void)
{
	m_usb_init(); // initialize USB information transfer
	clear(DDRC, PC7); // make sure PC7 is used as input
	set(PORTC, 7); // turn on internal pullup resistor on PC7
	
	set(DDRB, PB1); //set PB1 as output

	set(TCCR3B, CS32); // set timer 3 with prescaler of 1024
	set(TCCR3B, CS30); // set timer 3 with prescaler of 1024

	int previousState = 1; //set variable Previoussstate to check if LED has been set or not
	while (1){ // infinite loop
		if (bit_is_clear(PINC, 7)) { // if the input from PC7 is low -> due to internall pullup resistor means that if button is pressed 
			if (previousState != 0) { // only if the previous state was not 0 as well
				set(PORTB, PB1); // set PORTB as ouptut meaning LED turns on
				PRINTNUM(TCNT3); // print timer 3 to usb
				//PRINTNUM(1); // and print 1 
				
				previousState = 0; // previous stated updates so that i can be reused for the loop
			}
		}
		else { // if the input from PC7 is not low -> is high due to pullup. so means that button is not pressed
			if (previousState != 1) { // print if the state of the button has changed (so if button was pressed before)
				clear(PORTB, PB1); // if PC7 is high, clear PORTB1 output to 0 meaning LED turns off.
				PRINTNUM(TCNT3); // print number of timer to the putty
				//PRINTNUM(0); // print number 0
				previousState = 1; // turn previous state to 1 so that next time it will start checking if button is pressed again
			}
		}
	}
}


