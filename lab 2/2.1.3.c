/* Name: main.c
 * Author: Steyn Knollema
 * Description: 2.1.3
 * Copyright: I dont care if you copy it
 * Used ChatGPT as a debugger
 * 
 * Didn't use software to debounce switch since the RC filter already makes sure the switch is debounced
 */

#include "MEAM_general.h"  // General utility macros
#include "m_usb.h" // Initialize the USB print

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13) // got this macro from chatgpt to make the printing on new lines
int main(void)
{
	m_usb_init(); // initialie USB for communication
	clear(DDRC, PC7);// set PC7 as an input
	set(PORTC, 7); // turn on internal pullup resistor on PC7
	
	set(DDRB, PB1); //set PB1 as output

	set(TCCR3B, CS32); // set timer 3 with prescaler of 1024
	set(TCCR3B, CS30); // set timer 3 with prescaler of 1024

	int previousState = 1; //set previousState variable to check if state of PC7 changes and thus if the button is depressed or compressed again
	int timestart = 0; // timer variable for first press that is used to calculate the time difference in the end
	int lastTime = 0; // timer variable for second press that is used with time start to calculate it in the ned aswell
	int ButtonPressed = 0; //Flag to track if the firt button was pressed or not

	while (1){
		if (bit_is_clear(PINC, 7)) { // if the input from PC7 is low 
			if (previousState != 0) { // check if the stage of the button has changed
				set(PORTB, PB1); //turn on LED
				if(ButtonPressed == 0){ // when the first press is detected (when hte flag is set)
					timestart = TCNT3; // set time start to timer 3
					m_usb_tx_string("Time start is:"); // print text for clarification & debugging
					PRINTNUM(timestart); // print the time start number (for checking if calculation is correct) using PRINTNUM macro
					ButtonPressed = 1; // set a flag to indicate that the button has been pressed and thus will go thorugh else
				}
				else // when the second press is detected
				{
					clear(PORTB, PB1); // Turn the LED off (this is a change from 2.1.2 but is better for clarification)
					lastTime = TCNT3; // set last time to timer 3 time. this is at a different time moment that in above if
					m_usb_tx_string("Time stop is:"); // print text for clarification and checking answer
					PRINTNUM(lastTime); // print the time for checking caluclation using the PRINTNUM macro
					m_usb_tx_string("Time difference is: "); // text for clarification
					PRINTNUM(lastTime - timestart); // print the difference between last time and time start to get mashing using PRINTNUM macro
					ButtonPressed = 0; // set button pressed to 0 to indicate it is off. so that time starts starts after
				}
				previousState = 0; // update previous state to show press



			}
		}
		else{ // if button is not pressed (PC7 is hgih)
			previousState = 1; // reset previous state to detect next press
		
				
	
		}
		
	}
}


