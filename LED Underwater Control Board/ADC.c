/* A simple ADC example that checks the analog reading on ADC0 and turns
 * an LED on if the reading is higher than a threshold value and turns if
 * off if it is under that value. */
#include <avr/io.h>
#include <stdint.h>
#include "adc.h"

/* Which analog pin we want to read from.  The pins are labeled "ADC0"
 * "ADC1" etc on the pinout in the data sheet.  In this case ADC_PIN
 * being 0 means we want to use ADC0.  On the ATmega328P this is also
 * the same as pin PC0 */
#define CPU_TEMP_PIN 0
#define LED_TEMP_PIN 0

#define _BV(bit) (1 << (bit))


void ADCInit()
{
	/* Enable the ADC */
	ADCSRA |= _BV(ADEN);
}

uint16_t ADCRead(uint8_t channel) {
	/* adcx is the analog pin we want to use.  ADMUX's first few bits are
	 * the binary representations of the numbers of the pins so we can
	 * just 'OR' the pin's number with ADMUX to select that pin.
	 * We first zero the four bits by setting ADMUX equal to its higher
	 * four bits. */
	ADMUX	&=	0xf0;
	ADMUX	|=	channel;

	/* This starts the conversion. */
	ADCSRA |= _BV(ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	while ( (ADCSRA & _BV(ADSC)) );

	/* Finally, we return the converted value to the calling function. */
	return ADC;
}

uint16_t ReadTemperatue(uint8_t channel)
{
	uint16_t counts = ADCRead(channel);
	double volts;
	double temp;
	
	volts = ((double)counts * 5) / 1024.0;
	temp = (volts - 0.4) / .0195;
	
	return (uint16_t)temp;
}