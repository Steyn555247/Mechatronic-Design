/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright:  TBH i don't care if you copy it
 * License: 
 */
#include "MEAM_general.h"  // General utility macros

 // Function prototype for pulse
void pulse(int top, float increasetime, float decreasetime);

int main(void)
{
    _clockdivide(0); // set the clock speed to 16 Mhz
    set(DDRC, PC6);  // Set PB5 as output
    int TOP = 10000;

    // Timer/Counter control register setup for PWM on Timer 3
    set(TCCR3A, COM3A1);// Set Timer 3 to non-inverting mode for OC3A (clears output on compare match, sets at BOTTOM)
    clear(TCCR3A, COM3A0);// Ensure COM3A0 is cleared to fully enable non-inverting mode on OC3A

    set(TCCR3A, WGM31);//
    set(TCCR3B, WGM32);// 
    set(TCCR3B, WGM33);//

    set(TCCR3B, CS30);// Set Timer 3 prescaler to 64 by setting CS30 and CS31 (clock select bits)
    set(TCCR3B, CS31);// This sets the clock prescaler to 64, allowing a slower PWM frequency

    ICR3 = TOP;  // Set TOP value for Timer 3

    // Infinite loop to keep pulsing the LED
    while (1) {
        pulse(TOP, 500, 500);  // Pulse with increase and decrease time 500ms
        _delay_ms(500); // LED is off for 500ms
    }

    return 0;  // End of main
}

// Function to control the LED pulse with a duty cycle increase and decrease
void pulse(int top, float increasetime, float decreasetime) {
    float i;

    // Gradually increase brightness
    for (i = 0; i <= 100; i++) {
        OCR3A = (int)(i / 100.0 * top);  // Set duty cycle
        _delay_ms(increasetime / 100);   // Delay for smooth brightness increase in this case 500ms/100
    }

    // Gradually decrease brightness
    for (i = 100; i >= 0; i--) {
        OCR3A = (int)(i / 100.0 * top);// Set specific duty cycle
        _delay_ms(decreasetime / 100); // Delay for smooth brightness decrease in this case 500ms/100
    }
}
