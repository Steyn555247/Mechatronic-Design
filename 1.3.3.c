/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright:  TBH i don't care if you copy it
 * License: 
 */

#include "MEAM_general.h"  // includes the resources included in the MEAM_general.h file
#define dutycycle 50 // define dutycycle and give value that will be used in the while loop

int main(void)
{
	_clockdivide(0); // sets the speed of the clock to 16Mhz
	DDRC |= 0x40; //port C6 as output
	set(TCCR3B, WGM33);//sets WGM33 in TCCR3B to high (1000)
	set(TCCR3B, WGM32);//Sets WGM32 in TCCR3B to high (0100)
	set(TCCR3A, WGM31);//Sets WGM31 in TCCR3A to high (0010) in total sets TCCR in 1110 ->  Upto ICR3, PWM mode
	clear(TCCR3A, WGM30);//to be sure clears WGM30 (so total is (1110))
	set(TCCR3A, COM3A1);//sets output compare to non-inverting mode
	
	set(TCCR3B, CS31); // prescaler to 250khz (0000 0100) -> prescaler of 64
	set(TCCR3B, CS30);

	ICR3 = 15999; //Sets the TOP value for 1kHz PWM with prescaler of 64
	
	while (1) {
		if (dutycycle == 0) {
			OCR3A = 0; // 0% duty cycle (always off)
		}
		else if (dutycycle == 50) {
			OCR3A = ICR3 / 2; //50% duty cycle
		}else if (dutycycle == 100){
			OCR3A = ICR3; //100% dutycycle
		}
		//main loop that keeps repeating as the PWM is handled by hardware
	}



return 0;  // never reached
}
