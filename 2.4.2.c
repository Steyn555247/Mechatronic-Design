/* Name: main.c
 * Author: Steyn Knollema
 * Description: 2.4.2
 * Copyright: I don't care if you copy it
 * Used ChatGPT as a debugger
 * 
 * exact same code a 2.3.2 has been used
 */

#include "MEAM_general.h"
#include "m_usb.h" // Initialize USB print

int lasttime = 0;
int TimeDifference = 0;
int lastState = 0;
int currentState = 0;

int main(void) {
    m_usb_init();//initialize the USB
    clear(DDRD, PD1);// set PD1 as input pin by clearing DDRD register
    set(PORTD, PD1);// enable the internal pullup resistor on PD1

    set(DDRB, PB1);// Set PB1 as output in the DDR so that LED can turn on
    set(DDRF, PF6);//Set PF6 as output in DDR so LED can turn on
    clear(PORTB, PB1); //Make sure RED LED is off
    clear(PORTF, PF6); //Make sure tha tGreen LED is off

    set(TCCR1B, CS11); // Set Timer1 with prescaler of 64 -> increments of 4 microseconds
    set(TCCR1B, CS10); // ""
    clear(TCCR1B, CS12); // Just to be sure


    lastState = bit_is_set(PIND, PD1);// Check the initial state of the PD1

    while (1) {
        //set(PORTB, B2); //debugging purposes
        //_delay_ms(500); // ""
        currentState = bit_is_set(PIND, PD1); // set current state to if the pin is detecting anything or not

        if (currentState && !lastState)  //  if there is a change in if the pin detects anything
            int currenttime = TCNT1; //set the current time to Timer 1

        if (currenttime >= lasttime) { // if the new current time is more than the last time
            TimeDifference = currenttime - lasttime; // Calculate time difference in ticks by substracting the last time from the current time
        }
        else { // if the current time is smaller than the last time
            TimeDifference = (65535 - lasttime) + currenttime + 1; //calculate time difference by using max value of 8 bit, so that overflow is taken into account
        }

        lasttime = currenttime; // set the current to last time for the enxt time you go through the sequel


        if (TimeDifference >= 9900 && TimeDifference <= 10100) { // Check if timeDiff is within the range of 25 Hz signal with a reasonable safety window 
            set(PORTB, PB1); //if so turn on the LED connected to port B1
            m_usb_tx_string("Detected 25Hz signal, Red LED is on\r\n"); // debugging purposes
        }
        else {
            clear(PORTB, PB1);// otherwhise don't turn on PB1
        }


        if (TimeDifference >= 375 && TimeDifference <= 381)  // check if the time difference between the ticks is inside the calculated ticks needed for the 662HZ signal with a reasonable safety window
            set(PORTF, PF6); // if it is in that window. turn on the LED connected to Port F6
        m_usb_tx_string("Detected 662Hz signal, Green LED is on\r\n"); // print this to the terminal for debugging purposes
    }
            else {
                clear(PORTF, PF6)// otherwhise turn of the LED
            }
            m_usb_tx_string("Time Difference (Ticks): ");// sequel used for debugging prints the time difference to check if code is wrong or signal is wrong
            m_usb_tx_uint(TimeDifference);
            m_usb_tx_string("\r\n");

}

lastState = currentState; //update last state for the next time

_delay_us(10); //small delay to prevent that my computer went crazy
    }

    return 0;
}
