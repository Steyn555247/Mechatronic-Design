/* Name: main.c
 * Author: Steyn Knollema
 * Description: 3.1.2
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



	//this is for debugging
	//clear(DDRB, PB3); // set PB3 as input
	//set(DDRB, PB1); // set PB0 as output
	//clear(PORTB, PB1); // led is off in beginning

	while (1)
	{
		adc_setup(0); // ADC port setup, can be used for any ADC port. For debugging purposes I made it print clearly to register and therefore different variables are used for 0 and 1
		int adcstatus = adc_read(); //read the ADC value 
		m_usb_tx_string("ADC1 value is: ");
		m_usb_tx_int(adcstatus); //print the specific ADC status
		m_usb_tx_string("    "); // adding blank for readabillity

		int degree = adc_degrees(adcstatus);
		m_usb_tx_string("Degree is: ");
		m_usb_tx_int(degree); //print degree value of ADC.
		m_usb_tx_string("\n");//break line

		// following is code to try to get audio detection to work, Didn't work uptill now. Still working on it.
		//if (bit_is_set(PINB, PB3)) {
			//m_usb_tx_string("Sound is detected \n");

			// turn on LED
			//set(PORTB, PB0);
			//_delay_ms(200);

			//clear(PORTB, PB1);
		//}
		m_usb_tx_string("\n");// break line
		*/
		_delay_ms(500); // delay for readibillity and so my computer won't lift off
	}

}

void adc_setup(int ADCchannel) {


	if (ADCchannel == 2 || ADCchannel == 3 || ADCchannel > 13) { // can't use ADC values of 2, 3 and over 13
		ADCchannel = 0;// default to 0 if invalid channels are being used
		m_usb_tx_string("Invalid ADC channel was selected, defaulting to ADC0"); // clearly printing to user
		m_usb_tx_string("\n");

	}
	
	//Set DIDR
	if (ADCchannel <= 7) { // if value is below & set bit in DIDR0
		set(DIDR0, ADCchannel); // disable digitla in for adc0 till 7

	}
	else if (ADCchannel >= 8 && ADCchannel <= 13) { // if above 8 and under 14 set channel in DIDr2 register
		set(DIDR2, (ADCchannel - 8)); // disable digitla input for adc8 till 13 using set function.
	}
	/* for the following part used ChatGPT to optimize the code.
I started with all the cases and using the set & clear function accordingly however I thought it was wildly inefficient
Prompt:" Optimize this code to the most optimal state. It needs to be as fast and short as possible"
Resulting are the following two lines for using the Bit register (See Ed-discussion, mark allowed this
"The main thing is to make it clear, If you are aiming for short, then state that and don't use the set and clear"

For this ADC setup i was aiming for short (cause it was better readable than the switch case and therefore used the bit manipulation
*/
	ADMUX &= 0xF0;// clear 3 selction bits (Mux 1, 2 3 and 0) upfront so after this specific bits can be set again

	// if the channel is an 8 or higher, Adjust the Mux5 as well
	if (ADCchannel <= 7) {
		ADMUX |= ADCchannel; //turn on right bit in ADMUX with bit altering
		clear(ADCSRB, MUX5);// set mux5 bit if the ADC channel is higher than 7 (so 8 or more)
	}
	else {
		ADMUX |= (ADCchannel - 8); // set the ADMUX in the case of higher than 7
		set(ADCSRB, MUX5);//otherwhise clear this bit but now turn on mux5 since above 7
	}

	set(ADCSRA, ADSC); //start the adc conversion (set the flag)
}

int adc_read() {
	while (bit_is_set(ADCSRA, ADSC)); //if the conversion of ADC is finished (the flag is set)
	return ADC; //return the results of the ADC to read function in adc_read
}

int adc_degrees(int adc_read) {
	return (int)((long)adc_read * 180 / 1023);// turn adc value to a degree value based on the 0-1023 maps and 0-180 degrees. I used this 180 degrees (instead of 270 of potentiometers since it iwll be easier to use with servo's) 
	//(long is added since i first had overflow) I fixed it with chatGPT stating :" this is my calculation, this is what is going wrong how is this happening"
}