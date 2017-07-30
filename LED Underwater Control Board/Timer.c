/*
 * Timer.c
 *
 * Created: 9/7/2016 4:02:31 PM
 *  Author: gyurk
 */ 
#include <avr/io.h>
#include "PWM.h"
#include "Timer.h"

static uint32_t tick_counter = 0;

uint32_t ElapsedTime(uint32_t startTime)
{
	return (tick_counter - startTime);
}

void InitTimer(void)
{
	// setup timer 1 as a periodic (1ms) interrupt
	//TCCR1A |= (1<<WGM01);			// CTC mode
	TCCR1B |= (1<<WGM12);			// CTC mode
	OCR1A = 1000;					// 1000 with at 1Mhz clock = 1ms
	TCCR1B |= 1;					// prescaler of 1 (1uS counter)
	TIMSK1 |= (1<<OCIE1A);			// interrupt on timer1 output compare
}

ISR(TIMER1_COMPA_vect)
{
	tick_counter++;
}