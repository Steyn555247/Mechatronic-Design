/* Name: main.c
 * Author: Steyn Knollema
 * Description: Heartbeat that slowly fades out
 * Copyright: I dont care if you copy it
 * Used ChatGPT as a debugger
 */

#include "MEAM_general.h"  
#include "m_usb.h" // Initialize usb print



int main(void){
	
	m_usb_init();
	clear(DDRC, PC7); // set PC7 as input pint
	set(PORTC, 7); // set pullup resistor on PC7
	set(DDRB, PB1); //set PB1 as output

	while (1) {
		if (bit_is_set(PINC, PC7)) { // if the bit is set, pull up resistor pulls pin up -> on current flowing therefore no light
			set(PORTB, PB1); // set PB1 which connects to the LED
			m_usb_tx_string("No light detected, LED is ON \r\n"); // print text for clarification
			//_delay_ms(500); // delay for clearness
		}
		else { // if pc7 is not high -> it is low due to phototransistor letting through current and pulling down PC7
			clear(PORTB, PB1); // clear PB1 which turns of LED
			m_usb_tx_string("Light is detectedLED is OFF \r\n"); // print text for clarification
			//_delay_ms(500);
		}
	}

	return(0);
}
