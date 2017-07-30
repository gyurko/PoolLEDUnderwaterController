/*
 * PWM.h
 *
 * Created: 9/7/2016 4:05:15 PM
 *  Author: gyurk
 */ 

#include <avr/io.h>

#ifndef PWM_H_
#define PWM_H_

enum PWM_CHANNEL {
	
	// Name by color
	CHAN_GREEN		= 1,
	CHAN_BLUE		= 1<<1,
	CHAN_RED		= 1<<3,
	
	// Name by index
	CHAN_1			= 1,
	CHAN_2			= 1<<1,
	CHAN_4			= 1<<3,
	
	// Name by port
	CHAN_PD6		= 1,
	CHAN_PD5		= 1<<1,
	CHAN_PD3		= 1<<3,
	
	// Name by register
	CHAN_OC0A		= 1,
	CHAN_OC0B		= 1<<1,
	CHAN_OC2B		= 1<<3
	
};


void InitPWM(void);
void EnablePWMChannel(enum PWM_CHANNEL channel);
void DisablePWMChannel(enum PWM_CHANNEL channel);
void SetPWMChannelDutyCycle(enum PWM_CHANNEL channel, uint8_t dutycycle);

#endif /* PWM_H_ */