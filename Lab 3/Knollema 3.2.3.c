/* Name: main.c
 * Author: Steyn Knollema
 * Description: 3.2.3 with Three Servos and Corrected ADC Reading (ADC0 and ADC1 switched)
 * Adjusted mapping to ensure that when the potentiometers turn 90 degrees, the servos turn 90 degrees.
 * Reversed the angle for Servo 1 to correct rotation direction.
 *
 * used ChatGPT as debugger using the prompt "Debugg this : *insert fully copied code)" mostly using o1 preview/mini models
 * Used chatGPT to quickly structure code using prompt "Structure this code in a nice format so that it's properly readable. Do not change/add/delete any of the codes/comments"
 */

#include "MEAM_general.h"
#include "m_usb.h"          // Initialize USB communication
#include <avr/interrupt.h>
#include <avr/io.h>

 // Function prototypes
void adc_setup(int ADCchannel);                    // Setup ADC channel based on input (copied from 3.1
unsigned int adc_read(void);                       // Read the value from the ADC copied from 3.1
unsigned int adc_degrees(unsigned int adc_value);  // Convert ADC value to degrees copied from 3.1
void servointiliaze(void);                         // Initialize the PWM servo in subroutine to improve readability
void servo1angleset(unsigned int angle);           // Set angle for Servo 1 and make sure it aligns with potentiometer (but doesn't go past 180 degrees)
void servo2angleset(unsigned int angle);           // set angel for servo 2 ""

int main(void) {
    m_usb_init(); // Initialize USB communication

    set(ADMUX, REFS0);         // Set reference voltage to Vcc
    clear(ADMUX, REFS1);       // Set reference voltage to Vcc

    // ADC Prescaler =set prescaler to 128 for 125Khz
    set(ADCSRA, ADPS2);        // Set ADC prescaler bits
    set(ADCSRA, ADPS1);        //""
    set(ADCSRA, ADPS0);        //""

    set(ADCSRA, ADEN);         // EnabI wle ADC

    // Disable digital input on ADC0, ADC1 in DIDR
    set(DIDR0, ADC0D);         // ADC0
    set(DIDR0, ADC1D);         // ADC1

    // Servo Initialization
    servointiliaze();              // Initialize servos with subroutine for readabillity

    while (1) {
        // Read from ADC channel 1 for Servo 1
        adc_setup(1);                                      // Setup ADC on ADC 1 using subroutine
        unsigned int servo1_ADCvalue = adc_read();         // Read ADC value for Servo 1

        m_usb_tx_string("ADC1 raw value is: ");            // ttransmit string over USB for debugging purposes
        m_usb_tx_uint(servo1_ADCvalue);                    // transmit ADC value
        m_usb_tx_string("\n");                             // break line

        unsigned int servo1degrees = adc_degrees(servo1_ADCvalue);  // ADC value to degree converter

        // reverse the angle for Servo 1 to correct rotation direction since I messed it up with the setup
        servo1degrees = 180 - servo1degrees;

        m_usb_tx_string("Servo 1 Degree is: ");            // transmit the degrees of the servo to usb for debugging
        m_usb_tx_uint(servo1degrees);                      // Transmit degree value
        m_usb_tx_string("\n");                             // break

        // Set Servo 1 to the calculated angle using subroutine
        servo1angleset(servo1degrees);

        // Read from ADC channel 0 for Servo 2
        adc_setup(0);                                      // Setup adc on channel 0
        unsigned int servo2_ADCvalue = adc_read();         // reead adc value for Servo 2

        m_usb_tx_string("ADC0 raw value is: ");            // Transmit string over USB for debugging
        m_usb_tx_uint(servo2_ADCvalue);                    // Transmit ADC value
        m_usb_tx_string("\n");                             // break

        unsigned int servo2degrees = adc_degrees(servo2_ADCvalue);// Convert adc value to degrees

        m_usb_tx_string("Servo 2 Degree is: ");            // Transmit string over USB for debugging
        m_usb_tx_uint(servo2degrees);                      // Transmit degree value
        m_usb_tx_string("\n");                             // breaak line

        // Set Servo 2 to the calculated angle using subroutine
        servo2angleset(servo2degrees);
    }
}

void adc_setup(int ADCchannel) { // code from 3.1
    // Clear previous channel selection while preserving REFS bits
    ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);  // Set desired channel using bit operater

    if (ADCchannel > 7) {
        set(ADCSRB, MUX5);          // Set MUX5 for channels above 7
    }
    else {
        clear(ADCSRB, MUX5);        // Clear MUX5 for channels 0–7
    }

    // Do a dummy read after changing the channel to allow settling
    set(ADCSRA, ADSC);                   // Start ADC conversion
    while (bit_is_set(ADCSRA, ADSC));    // Wait for conversion to finish
}

unsigned int adc_read() {
    set(ADCSRA, ADSC);                   // Start ADC conversion
    while (bit_is_set(ADCSRA, ADSC));    // Wait for conversion to finish
    return ADC;                          // Return ADC value (0–1023) to read on USB
}

unsigned int adc_degrees(unsigned int adc_value) {
    // Map ADC value (0–682) to angle (0–180°)
    unsigned long angle = ((unsigned long)adc_value * 180UL) / 682UL; // scale 180 degrees over the range of 0 to 682 of the potentiometer to make sure that 90 degrees turn of pot is 90 degrees turn of servo
    if (angle > 180) angle = 180; // if for some reason angle is above 180 it turns to 180
    return (unsigned int)angle; // return angle to code
}

// for this quickly ask chatgpt to setup the PWM for the Servos: Prompt "give me an example of a PWM setup for servo using timer 2 for the ATMEGA32u4. Used the o1 preview model.
void servointiliaze(void) {
    //using timer 1 for both PB5/PB6 so to use 1 timer for all
    // Set PB5 (OC1A) and PB6 (OC1B) as outputs
    set(DDRB, PB5);// Servo 1 on PB5
    set(DDRB, PB6);// Servo 2 on Pb6

    // Configure Timer1 for Fast PWM with ICR1 as TOP
    set(TCCR1B, WGM13);  // Set Waveform Generation Mode bits
    set(TCCR1B, WGM12); //""
    set(TCCR1A, WGM11); //""
    clear(TCCR1A, WGM10); //""

    // setup noninverting output compare mode for both timers
    set(TCCR1A, COM1A1);  // Set Compare Output Mode for channel A (non-inverting)
    clear(TCCR1A, COM1A0); //""

    set(TCCR1A, COM1B1);  // Set Compare Output Mode for channel B (non-inverting)
    clear(TCCR1A, COM1B0); //""

    // Set prescaler to 8 for Timer1
    clear(TCCR1B, CS12);  // Set Clock Select bits for prescaler = 8
    set(TCCR1B, CS11);
    clear(TCCR1B, CS10);

    // set the top value for 20 ms on the timer by using shown formula
    ICR1 = 39999;  // 16 MHz / (8 * (ICR1 + 1)) = 50 Hz ==20ms
}

void servo1angleset(unsigned int angle) {
    if (angle > 180) angle = 180; // limit angle to 180 (i know i did it before for the pot angle but just so i odn't wanna breka anything i kept it in)

    // Map angle to pulse width (1000 to 5000 counts)
    unsigned int pulse_min = 1000;// 0.5 ms pulse width meaning 0 degrees for servo
    unsigned int pulse_max = 5000;// 2.5 ms pulse width meaning 180 degrees on servo

    // Calculate pulse width for the angels in between max and min
    unsigned long pulse_width = pulse_min + ((unsigned long)(pulse_max - pulse_min) * angle) / 180UL;

    // Set compare value for OCR1A (Servo 1) to the pulswidth that was just calculated
    OCR1A = pulse_width; // for port PB5
}

void servo2angleset(unsigned int angle) {
    if (angle > 180) angle = 180; // limit angle to 180 (i know i did it before for the pot angle but just so i odn't wanna breka anything i kept it in)

    // Map angle to pulse width (1000 to 5000 counts)
    unsigned int pulse_min = 1000;   // 0.5 ms pulse width meaning 0 degrees for servo
    unsigned int pulse_max = 5000;   // 2.5 ms pulse width meaning 180 degrees for servo

    // Calculate pulse width for the given angle in between max and min
    unsigned long pulse_width = pulse_min + ((unsigned long)(pulse_max - pulse_min) * angle) / 180UL;
    OCR1B = pulse_width; //for PB6 set OCR1b to previously calculated pulse width.
}
