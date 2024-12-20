/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright: TBH I don't care if you copy it
 * License: 
 */
#include "MEAM_general.h"  // General utility macros

 // Function prototype for pulse
void pulsefunction(int top, float max_intensity, float increasetime, float decreasetime);
// function for heartbeat pattern
void heartbeat(int top);

int main(void)
{
    _clockdivide(0); // set the clock speed to 16 Mhz
    set(DDRC, PC6);  // Set PB5 as output
    int TOPvalue = 10000; // sets variable to TOP value to make LED pulse 250Khz/10000 = 23.9 Hz which is low, but it worked.

    // Timer/Counter control register setup for PWM on Timer 3
    set(TCCR3A, COM3A1);// Set Timer 3 to non-inverting mode for OC3A (clears output on compare match, sets at BOTTOM)
    clear(TCCR3A, COM3A0);// Ensure COM3A0 is cleared to fully enable non-inverting mode on OC3A

    set(TCCR3A, WGM31);// Set WGM31 (Waveform Generation Mode) for Fast PWM
    set(TCCR3B, WGM32);// Set WGM32 for Fast PWM mode with ICR3 as the TOP value
    set(TCCR3B, WGM33);// Set WGM33 for Fast PWM mode (along with WGM31, WGM32)

    set(TCCR3B, CS30);// Set Timer 3 prescaler to 64 by setting CS30 and CS31 (clock select bits)
    set(TCCR3B, CS31);// This sets the clock prescaler to 64, allowing a slower PWM frequency

    ICR3 = TOPvalue;  // Set TOP value for Timer 3

    // Infinite loop to keep pulsing the LED
    while (1) {
        heartbeat(TOPvalue);
        _delay_ms(2000);
    }

    return 0;  // End of main
}

// Function to control the LED pulse with a duty cycle increase and decrease. I call this pulse function in the heartbeat which allows me to fade in/out the pulse
void pulsefunction(int top, float max_intensity, float increase, float decrease) {
    float i;

    // Gradually increase brightness
    for (i = 0; i <= 100; i++) {
        OCR3A = (int)(i / 100.0 * top);// Set duty cycle
        _delay_ms(increase / 100);// Delay for smooth brightness increase
    }

    // Gradually decrease brightness
    for (i = 100; i >= 0; i--) {
        OCR3A = (int)(i / 100.0 * top);// Set duty cycle
        _delay_ms(decrease / 100);// Delay for smooth brightness decrease
    }
}
    //function for heartbeat pattern
    void heartbeat(int top) {
        pulsefunction(top, 100, 100, 400);// fade in 100% in 100ms, fade out in 400ms
        _delay_ms(100); //delay 100ms before second pulse
        pulsefunction(top, 50, 100, 400);//fade in 50% in 100ms, fade out in 400ms
        _delay_ms(2000); // wait 2000 ms for next signal
        

    }

