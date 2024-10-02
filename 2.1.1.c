/* Name: main.c
 * Author: Steyn Knollema
 * Description:Switch that turns on LED
 * Copyright: I dont care if you copy it
 * Used ChatGPT as a helper for debugging
 */

#include "MEAM_general.h"  // General utility macros
#include "m_usb.h" // Initialize the USB print

#define PRINTNUM(x) m_usb_tx_uint(x); m_usb_tx_char(10); m_usb_tx_char(13) // send X over usb followed by newline and carriage return for making it more clear

int main(void)
{
	m_usb_init(); // initialize USB transfer
	clear(DDRC, PC7); // make sure that PC7 is used as input port
	set(PORTC, 7); // turn on internal pullup resistor on PC7
	
	set(DDRB, PB1); //set PB1 as output

	
	while (1){
		if (bit_is_clear(PINC, 7)) { // if the input register for Port C bit 7 (PC7) is clear. Uses pullup resistor so means if it is low
			set(PORTB, PB1); // then set portB, PB1 as an output port making the LED turn on
			PRINTNUM(1); // Print one to the USB terminal

		}
		else { // Else (if button is not pressed) pullup resistor will pull up -> Bit is not clear
			clear(PORTB, PB1); // PB1 will not be used as output port thus LED will be off
			PRINTNUM(0); // print 0 to usb terminal
		}
	}
}


