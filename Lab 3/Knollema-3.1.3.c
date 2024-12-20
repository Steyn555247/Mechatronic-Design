/* Name: main.c
 * Author: Steyn Knollema
 * Description: 3.1.3
 * Copyright: I don't care if you copy it
 * Used ChatGPT as a helper
 */

#include "MEAM_general.h"
#include "m_usb.h" // Initialize USB print
#include "avr/interrupt.h" // Add for allowing interrupt not used in this code
#include "avr/io.h"

void adc_setup(int ADCchannel);//setup wanted ADC channel
int adc_read(void); //read the value from the ADC
int adc_degrees(int adcstatus);//convert adc value to degrees

int main(void) {
	m_usb_init(); // initialize USB com
	set(ADMUX, REFS0);//set Vcc for Vref
	clear(ADMUX, REFS1); //"
	set(ADCSRA, ADPS2); //set prescaler to 128 -> 100kHZ
	set(ADCSRA, ADPS1); //"
	set(ADCSRA, ADPS0); // "
	set(ADCSRA, ADEN); // this enables the ADC mode (one stupid line missing costing me 2 hours ;-(   )
	set(DIDR0, ADC0D); //disable digital input for ADC0
	set(DIDR0, ADC1D); //sidable digital input for ADC1


	//this is for debugging
	//clear(DDRB, PB3); // set PB3 as input
	//set(DDRB, PB1); // set PB0 as output
	//clear(PORTB, PB1); // led is off in beginning

	while (1)
	{
		adc_setup(1); // first setup ADC 1 since by potentiometers are connected to 0 and 1, any ADC can be used and it will work (except for the ADC's that don't exist obviously, like ADC 2). For debugging purposes I made it print clearly to register and therefore different variables are used for 0 and 1
		int adcstatus1 = adc_read(); //read the ADC value 
		m_usb_tx_string("ADC1 value is: ");
		m_usb_tx_int(adcstatus1); //print the specific ADC status
		m_usb_tx_string("    "); // adding blank for readabillity

		int degree1 = adc_degrees(adcstatus1);
		m_usb_tx_string("Degree is: ");
		m_usb_tx_int(degree1); //print degree value of ADC.
		m_usb_tx_string("\n");//break line

		adc_setup(0);
		int adcstatus0 = adc_read(); // read the ADC value 
		m_usb_tx_string("ADC0 value is: ");
		m_usb_tx_int(adcstatus0); //print the specific ADC status
		m_usb_tx_string("    ");// blank for readabillity

		int degree0 = adc_degrees(adcstatus0);
		m_usb_tx_string("Degree is: ");
		m_usb_tx_int(degree0); // print degree value of ADC.
		m_usb_tx_string("\n");// break line

		// following is code to try to get audio detection to work, Didn't work uptill now. Still working on it.
		//if (bit_is_set(PINB, PB3)) {
			//m_usb_tx_string("Sound is detected \n");

			// turn on LED
			//set(PORTB, PB0);
			//_delay_ms(200);

			//clear(PORTB, PB1);
		//}
		m_usb_tx_string("\n");// break line
		_delay_ms(500);
	}

}

void adc_setup(int ADCchannel) {
	/* for the following part used ChatGPT to optimize the code. 
	I started with all the cases and using the set & clear function accordingly however I thought it was wildly inefficient
	Prompt:" Optimize this code to the most optimal state. It needs to be as fast and short as possible"
	Resulting are the following two lines for using the Bit register (See Ed-discussion, mark allowed this 
	"The main thing is to make it clear, If you are aiming for short, then state that and don't use the set and clear"
	
	For this ADC setup i was aiming for short (cause it was better readable than the switch case and therefore used the bit manipulation 
	*/

	if (ADCchannel == 2 || ADCchannel == 3 || ADCchannel > 13) {
		ADCchannel = 0;// default to 0 if invalid channels are being used
		m_usb_tx_string("Invalid ADC channel was selected, defaulting to ADC0"); // clearly printing to user
		m_usb_tx_string("\n");

	}

	ADMUX &= 0xF0;// clear 3 selction bits (Mux 1, 2 3 and 0) upfront so after this specific bits can be set again
	ADMUX |= (ADCchannel & 0x0F); //sets lower four bits of ADMUX to set desired channel

	// if the channel is an 8 or higher, Adjust the Mux5 as well
	if (ADCchannel > 7) {
		set(ADCSRB, MUX5);// set mux5 bit if the ADC channel is higher than 7 (so 8 or more)
	}
	else {
		clear(ADCSRB, MUX5);//otherwhise clear this bit
	}

	set(ADCSRA, ADSC); //start the adc conversion
}

int adc_read() {
	while (bit_is_set(ADCSRA, ADSC)); //if the conversion of ADC is finished (the flag is set)
	return ADC; //return the results of the ADC to read function in adc_read
}

int adc_degrees(int adc_read) {
	return (int)((long)adc_read * 270 / 1023);// turn adc value to a degree value based on the 0-1023 maps and 0-270 degrees.
	//(long is added since i first had overflow) I fixed it with chatGPT stating :" this is my calculation, this is what is going wrong how is this happening"
}