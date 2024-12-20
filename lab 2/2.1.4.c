/* Name: main.c
 * Author: Steyn Knollema
 * Description: 2.1.4
 * Copyright: I dont care if you copy it
 * Used ChatGPT as a helper
 */

#include "MEAM_general.h"  
#include "m_usb.h" // Initialize usb print

#define INPUT 1// input is defined as a 1
#define INPUT_PULLUP 2//input with pullup is now defined as 2
#define OUTPUT 3// output is defined as number 3

#define pinMode(port, bit, mode)                              
  do {                                                        
    if ((mode) == INPUT) { /* check if  given mode is INPUT*/                      
      DDR##port &= ~(1 << (bit));  /*if given mode is input, set DDR registor to input */    
      PORT##port &= ~(1 << (bit));/*pull up not need so disable pullup*/   
    } 
    else if ((mode) == INPUT_PULLUP) {    /* if mode is said to be input_pullup*/                
      DDR##port &= ~(1 << (bit));  /*if given mode is input, set DDR registor to input */    
      PORT##port |= (1 << (bit));  /* set specific port to enable pullup resistor */       
    } 
    else if ((mode) == OUTPUT) { /* if the given mode is output*/                      
      DDR##port |= (1 << (bit));   /*  set the specific pin as output in DDR */    
    }  


void checkPin(char port, int bit) {  // Function to check the pinMode to see if i did it correclty and print it to the USB
    int ddrreg = 0; // intialie variable for checking DDR
    int portreg = 0; // intialise vairable for checkin gpullup

    switch (port) { // switches between cases to create ifelse statements but more clealry
    case 'B': // if the port is port B, check the ddr and port
        ddrreg = DDRB; // check the DDRB of Port B
        portreg = PORTB; // Check the pullup of Port B
        break; // break switch to go through and not directly go to next case
    case 'C': // if port is port c, check DDR and pullup
        ddrreg = DDRC; // set variable to DDRC of Port C
        portreg = PORTC; // check the pullup of portc
        break;// break switch to go through and not directly go to next case
    case 'D': // If port is port D, check DDR and pullup
        ddrreg = DDRD; // check the DDRD 
        portreg = PORTD; // set varialbe to PORTD to check Pullup
        break;// break switch to go through and not directly go to next case
    default: // if something is none of the cases say Invalid port *port**bit* Check your port.
        m_usb_tx_string("Invalid port "); // print to usb also 4 lines below here
        m_usb_tx_char(port);
        m_usb_tx_uint(bit);
        m_usb_tx_string(" Check your port. \r\n");
        return;
    }

    if (ddrreg & (1 << bit)) { // if the in the ddr register shows that bit is set or cleared. checks if pin is output since if 1 it is ouptut
        m_usb_tx_string("Port "); // print the text "Port ... Pin ... is set as output"
        m_usb_tx_char(port);
        m_usb_tx_string(", Pin ");
        m_usb_tx_uint(bit);
        m_usb_tx_string(" is set as OUTPUT.\r\n");// make it nice and understandable by breaking the line
    }
    else { // if pin is not set as output -> must be input
        if (portreg & (1 << bit)) { // checks if pullup resistor is on by checking PORTregister, if PIN is 1 it is set as pullup
            m_usb_tx_string("Port "); // prints text "Port ... Pin .. is set as INPUT with PULL-UP
            m_usb_tx_char(port);
            m_usb_tx_string(", Pin ");
            m_usb_tx_uint(bit);
            m_usb_tx_string(" is set as INPUT with PULL-UP.\r\n"); // make it nice and understandable by breaking the line
        }
        else { // if pullup resistor pinn is not on, it must be off
            m_usb_tx_string("Port "); // print "Port ... Pin ... is set as input without pulllup"
            m_usb_tx_char(port);
            m_usb_tx_string(", Pin ");
            m_usb_tx_uint(bit);
            m_usb_tx_string(" is set as INPUT without PULL-UP.\r\n");  //make it nice and understandable by breaking the line
        }
    }
}

int main(void) { // this is main loop that actually runs
    m_usb_init(); // initialize pin
    pinMode(C, 7, INPUT_PULLUP);  // uses pinmode function to set  PC7 as input with pull-up resistor, need that for button in this program as well
    pinMode(B, 1, OUTPUT);        // uses pinmode fucntion to set PB1 as output
    pinMode(D, 2, INPUT);         // uses pinmode function to set PD2 as input without pull-up

   
    while (1) { // infinite loop
        if (bit_is_clear(PINC, 7)) { // if the input from PC7 is low, button should be pressed due to internal resistor (also a small check on if the internal resistor is actually on). gives you info on button press
            checkPin('C', 7);  // Check PC7
            checkPin('B', 1);  // Check PB1
            checkPin('D', 2);  // CheckPD2
            checkPin('A', 3);   // See if it also works with invalid ports
            _delay_ms(5000);   // Delay so i can actually read the results normally
        }
    }

    return 0;
}
