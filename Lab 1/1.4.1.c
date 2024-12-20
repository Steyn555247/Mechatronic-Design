/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright:  TBH i don't care if you copy it
 * License:
 */
#include "MEAM_general.h"  // General utility macros

 // Function prototype for pulse
void pulsefunc(int top, float increase, float decrease);

int main(void)
{
    _clockdivide(0); // set the clock speed to 16 Mhz
    set(DDRC, PC6);  // Set PC6 as output
    int TOPvalue = 10000; // sets variable to TOP value to make LED pulse 250Khz/10000 = 23.9 Hz which is low, but it worked.
    int inctime = 300; // variable to set increase time
    int dectime = 600; // variable to set decrease time

    // Timer/Counter control register setup for PWM on Timer 3
    set(TCCR3A, COM3A1);// Set Timer 3 to non-inverting mode for OC3A (clears output on compare match, sets at BOTTOM)
    clear(TCCR3A, COM3A0);// Ensure COM3A0 is cleared to fully enable non-inverting mode on OC3A
    set(TCCR3A, WGM31);// Set WGM31 (Waveform Generation Mode) for Fast PWM
    set(TCCR3B, WGM32);// Set WGM32 for Fast PWM mode with ICR3 as the TOP value
    set(TCCR3B, WGM33);// Set WGM33 for Fast PWM mode (along with WGM31, WGM32)
    set(TCCR3B, CS30);// Set Timer 3 prescaler to 64 by setting CS30 and CS31 (clock select bits)
    set(TCCR3B, CS31);// This sets the clock prescaler to 64, allowing a slower PWM frequency

    ICR3 = TOPvalue;  // Set TOP value for ICR3 to variable TOP (which is 10000)

    // Infinite loop to keep pulsing the LED
    while (1) {
        pulsefunc(TOPvalue, inctime, dectime);  // Pulse with increase and decrease time 500ms
        _delay_ms(500); // LED is off for 500ms
    }

    return 0;  // will never reach it
}

// Function to control the LED pulse with a duty cycle increase and decrease
void pulsefunc(int top, float increase, float decrease) {
    float i;

    // Gradually increase brightness
    for (i = 0; i <= 100; i++) {
        OCR3A = (int)(i / 100.0 * top);  // Set duty cycle
        _delay_ms(increase / 100);   // Delay for smooth brightness increase in this case 500ms/100
    }

    // Gradually decrease brightness
    for (i = 100; i >= 0; i--) {
        OCR3A = (int)(i / 100.0 * top);// Set specific duty cycle
        _delay_ms(decrease / 100); // Delay for smooth brightness decrease in this case 500ms/100
    }
}
}
