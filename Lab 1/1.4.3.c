/* Name: main.c
 * Author: Steyn Knollema
 * Description: Heartbeat that slowly fades out
 * Copyright: I dont care if you copy it
 */

#include "MEAM_general.h"  // General utility macros

 //Constants that are being used throughout the code, allows for centralized change of code
#define TOPvalue 10000// Set the TOP value for the 3rd Timer PMW
#define STEPS 100// Set 100 steps for slowly fading the Heartbeat
#define HEARTBEAT_PERIOD 3000// Set the total period of the heartbeat in MS (3000ms -> 3 seconds)
#define NUM_BEATS 20// fade out Heartrate in 20 beats.
#define INITIAL_INTENSITY 100 // Starting intensity percentage (0-100%) and slowly decrease this intesnity

// set the functions
void pulsefunc(int max_intensity, int increase_ms, int decrease_ms);
void heartbeat(int max_intensity);
void fade_intensity(void);

int main(void)
{
    //Set clock and PC6
    _clockdivide(0);// Set the clock speed to 16 MHz
    set(DDRC, PC6);// Set PC6 to an output since the LED is connected to PC6

    // Setup the Timer/control register for the PWM on Timer 3 (which is on PC6)
    set(TCCR3A, COM3A1);// Non-inverting mode for OC3A so the output starts high
    clear(TCCR3A, COM3A0);// Ensure COM3A0 is cleared

    set(TCCR3A, WGM31);// set the Waveform Geneartion mode on fast PWM mode
    set(TCCR3B, WGM32);// set the waveform generation mode on fast PWM  
    set(TCCR3B, WGM33);// set waveform generation mode on fast PWM with ICR3 as top

    // Set prescaler to 1 for maximum PWM frequency to prevent flicker (I had this problem with my earlier code that it would flicker) (see 1.4.2) but changed that for this final code
    set(TCCR3B, CS30); // turns on bit CS30
    clear(TCCR3B, CS31); // Turns of bit CS31
    clear(TCCR3B, CS32); // Turns off CS32 thus making sure the timer runs at full clock speed


    ICR3 = TOPvalue;// Set TOP value for Timer 3 PWM

    int max_intensity = INITIAL_INTENSITY;//Start with making max_intensity the initial intensity. through this i can operate it centralized
    int intensity_step = INITIAL_INTENSITY / NUM_BEATS;// Intensity decrement per beat (in this case max/20)
    int beats_remaining = NUM_BEATS;// Beats counter

    // Main loop to produce the heartbeat effect
    while (beats_remaining > 0) { // keep looping as long as the remaining beats are more than 0
        heartbeat(max_intensity);// Execute the heartbeat function with current max intensity

        max_intensity -= intensity_step; //lower the intensity for the next beat
        beats_remaining--; //decrease beats counter
        // Delay to maintain constant heartbeat period
        _delay_ms(HEARTBEAT_PERIOD - 1000);// the entire heartbeat function duration is 1000 ms
    }

    // After 20 beats, ensure the LED is turned off
    OCR3A = 0;// Set duty cycle to 0 (LED off) to make sure the LED is really off

    return 0;  // End of main, will reach this after 20 beats
}

// Function to control the LED pulse with variable intensities and durations
void pulsefunc(int max_intensity, int increase_ms, int decrease_ms) {
    int i; // set variable i that will be used in for loop
    int duty_cycle; // set the inensity level of the LED as it pulses
    int steps = STEPS; // the maount of steps being used to gradually decrease the LED
    int delay_increase = increase_ms / steps; // the wait time to wait between every step when increasing the LED intensity. set delay increase as a result of the increase time / steps (slowly growing smaller as steps increase)
    int delay_decrease = decrease_ms / steps;// the wait time to wait between every stap when decreasing this intensity. Set delay increase as a result of decrease / steps (slowly growing smaller as steps increase). added this since I didn't like my first heartbeat (looked unnatural)

    // Gradually increase brightness
    for (i = 0; i <= steps; i++) {
        // Calculate the current intensity
        int current_intensity = (i * max_intensity) / steps;

        // Convert intensity percentage to duty cycle value
        duty_cycle = (current_intensity * TOPvalue) / 100;
        OCR3A = duty_cycle;  // Set duty cycle

        _delay_ms(delay_increase);  // Delay for smooth brightness increase makes the heartbeat look more natural, first I didn't have this and I didn't like the heartbeat pattern.
    }

    // Gradually decrease brightness
    for (i = steps; i >= 0; i--) { // for loop to decrease the brightness of the LED
        // Calculate the current intensity
        int current_intensity = (i * max_intensity) / steps;// current_intensity is used to create a linear scale to scale the LED

        // Convert intensity percentage to duty cycle value
        duty_cycle = (current_intensity * TOPvalue) / 100; // use Curent cycle and top value to create Dutycycle. for decresaing LED
        OCR3A = duty_cycle;// Set duty cycle

        _delay_ms(delay_decrease);// Delay for smooth brightness decrease see delay_increase comment
    }
}

// Function for heartbeat pattern with variable maximum intensity
void heartbeat(int max_intensity) {
    // First pulse: Increase to max_intensity over 100 ms, then decrease to 0% over 400 ms
    pulsefunc(max_intensity, 100, 400);//First hard pulse
    // Short pause between pulses
    _delay_ms(100);// Delay of 100 ms
    // Second pulse: Increase to half of max_intensity over 100 ms, then decrease to 0% over 400 ms
    pulsefunc(max_intensity / 2, 100, 400);  // Second "dub" pulse
}
