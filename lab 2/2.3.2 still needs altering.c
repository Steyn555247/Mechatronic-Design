/* Name: main.c
 * Author: Steyn Knollema
 * Description: Heartbeat that slowly fades out
 * Copyright: I don't care if you copy it
 * Used ChatGPT as a helper
 */

#include "MEAM_general.h"
#include "m_usb.h" // Initialize USB print
#include "avr/interrupt.h" // Add for allowing interrupt
#include "avr/io.h"

int lastTime = 0;
int timeDiff = 0;
int newTime = 0;
int lastState = 0;
int currentState = 0;

int main(void) {

    // Initialize USB communication
    m_usb_init();
    m_usb_tx_string("USB initialized!\r\n");

    // Configure PD1 as input with pull-up resistor
    clear(DDRD, PD1); // Set PD1 as input pin
    set(PORTD, PD1);  // Enable pullup resistor on PD1

    // Configure LEDs (PB1 = Red LED, PF7 = Green LED)
    set(DDRB, PB1);   // Set PB1 as output
    set(DDRB, PB3);   // Set PF6 as output

    // Ensure LEDs are off at start
    clear(PORTB, PB1); // Red LED off
    clear(PORTB, PB3); // Green LED off

    // Set up timer with 16MHz clock, Timer1 increments are 4 microseconds
    set(TCCR1B, CS11); // Set Timer1 with prescaler of 64
    set(TCCR1B, CS10);
    clear(TCCR1B, CS12); // Just to be sure

    // Check the initial state of PD1
    lastState = bit_is_set(PIND, PD1);
    if (lastState) {
        m_usb_tx_string("Initial state of PD1: HIGH\r\n");
    }
    else {
        m_usb_tx_string("Initial state of PD1: LOW\r\n");
    }

    while (1) {
        set(PORTB, B2);
        _delay_ms(500);
        // Read the current state of PD1
        currentState = bit_is_set(PIND, PD1);

        // Detect rising edge
        if (currentState && !lastState) {
            int currentTime = TCNT1; // Get current timer value

            // Calculate time difference
            if (currentTime >= lastTime) {
                timeDiff = currentTime - lastTime;
            }
            else {
                timeDiff = (65535 - lastTime) + currentTime + 1; // Timer overflow
            }

            lastTime = currentTime;

            // Check if timeDiff is within the range of 25 Hz signal
            if (timeDiff >= 9500 && timeDiff <= 10500) {
                set(PORTB, PB1); // Turn on Red LED (25 Hz detected)
                m_usb_tx_string("Detected 25Hz signal, Red LED is on\r\n");
            }
            else {
                clear(PORTB, PB1); // Turn off Red LED
            }

            // Check if timeDiff is within the range of 600-700 Hz signal
            if (timeDiff >= 357 && timeDiff <= 417) {
                set(PORTB, PB3);
                m_usb_tx_string("Detected 600-700Hz signal, Green LED is on\r\n");
            }


            // Print timeDiff for debugging
            m_usb_tx_string("Time Difference (Ticks): ");
            m_usb_tx_uint(timeDiff);
            m_usb_tx_string("\r\n");

        }

        lastState = currentState; // Update lastState for next iteration

        _delay_us(10); // Small delay to prevent excessive CPU usage
    }

    return 0;
}
