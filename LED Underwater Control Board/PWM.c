/*
 * PWM.c
 *
 * Created: 9/7/2016 4:03:29 PM
 *  Author: gyurk
 */ 
#include <avr/io.h>
#include "PWM.h"

void InitPWM() {
	
	// Set mode of operation to FastPWM
	TCCR0A |= (1<<WGM00 | 1<<WGM01);
	TCCR2A |= (1<<WGM20 | 1<<WGM21);
	
	// Set clock source (prescaler)
	TCCR0B |= (1<<CS01);
	TCCR2B |= (1<<CS21);
	
	// Set to 50% duty cycle
	OCR0A = 0x80;
	OCR0B = 0x80;
	OCR2B = 0x80;
	
	// 4 PWM channel outputs
	DDRD |= 1<<PD3; // OC2B
	DDRD |= 1<<PD5; // OC0B
	DDRD |= 1<<PD6; // OC0A
}

// Enable PWM channels
void EnablePWMChannel(enum PWM_CHANNEL channel) {
	if (channel & CHAN_OC0A) TCCR0A |= 1<<COM0A1;
	if (channel & CHAN_OC0B) TCCR0A |= 1<<COM0B1;
	if (channel & CHAN_OC2B) TCCR2A |= 1<<COM2B1;
}

// Disable PWM channels
void DisablePWMChannel(enum PWM_CHANNEL channel) {
	if (channel & CHAN_OC0A) TCCR0A &= ~(1<<COM0A1);
	if (channel & CHAN_OC0B) TCCR0A &= ~(1<<COM0B1);
	if (channel & CHAN_OC2B) TCCR2A &= ~(1<<COM2B1);
}

// Sets the channel brightness
// dutycycle value is 255 for full on
void SetPWMChannelDutyCycle(enum PWM_CHANNEL channel, uint8_t dutycycle) {
	if (channel & CHAN_OC0A) OCR0A = dutycycle;
	if (channel & CHAN_OC0B) OCR0B = dutycycle;
	if (channel & CHAN_OC2B) OCR2B = dutycycle;
}