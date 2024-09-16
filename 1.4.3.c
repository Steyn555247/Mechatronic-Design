/* Name: main.c
 * Author: Steyn L Knollema
 * Copyright: I got it from ChatGPT so you can get it from me. (yeah that rhymes)
 * License: 
 */
#include "MEAM_general.h"  // General utility macros

 // Function prototype for pulse
void pulse(int top, float max_intensity, float increasetime, float decreasetime);
// function for heartbeat pattern
void heartbeat(int top, float max_intensity);

int main(void)
{
    _clockdivide(0); // set the clock speed to 16 Mhz
    set(DDRC, PC6);  // Set PB5 as output
    int TOP = 10000;
    float max_intensity = 100.0;//start with high intesnity
    int beats = 20; //number of heartbeats before fully away

    // Timer/Counter control register setup for PWM on Timer 3
    set(TCCR3A, COM3A1);// Set Timer 3 to non-inverting mode for OC3A (clears output on compare match, sets at BOTTOM)
    clear(TCCR3A, COM3A0);// Ensure COM3A0 is cleared to fully enable non-inverting mode on OC3A

    set(TCCR3A, WGM31);// Set WGM31 (Waveform Generation Mode) for Fast PWM
    set(TCCR3B, WGM32);// Set WGM32 for Fast PWM mode with ICR3 as the TOP value
    set(TCCR3B, WGM33);// Set WGM33 for Fast PWM mode (along with WGM31, WGM32)

    set(TCCR3B, CS30);// Set Timer 3 prescaler to 64 by setting CS30 and CS31 (clock select bits)
    set(TCCR3B, CS31);// This sets the clock prescaler to 64, allowing a slower PWM frequency

    ICR3 = TOP;  // Set TOP value for Timer 3

    // Infinite loop to keep pulsing the LED
    while (beats > 0 && max_intensity > 0) {
        heartbeat(TOP, max_intensity);
        max_intensity -= (100.0/20.0);//gradually decrease heartbeat by 1/20th part
        beats--; // decrease counts of beats.
        _delay_ms(2000);
    }

    return 0;  // End of main
}

// Function to control the LED pulse with a duty cycle increase and decrease
void pulse(int top, float max_intensity, float increasetime, float decreasetime) {
    float i;

    // Gradually increase brightness
    for (i = 0; i <= max_intensity; i++) {
        OCR3A = (int)(i / 100.0 * top);  // Set duty cycle
        _delay_ms(increasetime / max_intensity);   // Delay for smooth brightness increase
    }

    // Gradually decrease brightness
    for (i = max_intensity; i >= 0; i--) {
        OCR3A = (int)(i / 100.0 * top);  // Set duty cycle
        _delay_ms(decreasetime / max_intensity);   // Delay for smooth brightness decrease
    }
}
    //function for heartbeat pattern
    void heartbeat(int top, float max_intensity) {
        pulse(top, max_intensity, 100, 400); // fade in max intensity in 100ms, fade out in 400ms
        _delay_ms(100); //delay 100ms before second pulse
        pulse(top, max_intensity/2, 100, 400);//fade in 50% of max intensity in 100ms, fade out in 400ms
        
    }

