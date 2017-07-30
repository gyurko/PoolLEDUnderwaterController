/*
 * UART.c
 *
 * Created: 9/8/2016 4:03:30 PM
 *  Author: gyurk
 */ 
#include <avr/io.h>
#include <stdbool.h>
#include <ctype.h>
#include "PWM.h"

// protocol is "<RRR GGG BBB>" @4800

#define START_CHAR '<'
#define END_CHAR '>'

typedef enum 
{
	WAIT_FOR_START,
	WAIT_FOR_R,
	WAIT_FOR_G,
	WAIT_FOR_B,
	WAIT_FOR_COMMAND		
}state_t;

static state_t state = WAIT_FOR_START;
uint8_t current_red, current_green, current_blue, command;
bool bColorMsgReceived = false;

void InitUART(void)
{
	// Set baud rate to 4800
	UBRR0H = 0;
	UBRR0L = 12;
	// 8 data bits, no parity, 1 stop bit
	UCSR0C = (3<<UCSZ00);
	// Enable receiver
	UCSR0B = (1<<RXEN0);
	// enable receive interrupt
	UCSR0B |= (1<<RXCIE0);
}

void ProcessReceiveChar(uint8_t c)
{
	// anytime there's a start char, reset everything
 	if (c == START_CHAR)
	{
		current_red = 0;
		state = WAIT_FOR_R;
	}
	
	switch (state)
	{
		case WAIT_FOR_START:
			break;
			
		case WAIT_FOR_R:
			if (isdigit(c))
			{
				current_red *= 10;
				current_red += (c - 0x30);
			}
			else if (c == ' ')
			{
				current_green = 0;
				state = WAIT_FOR_G;
			}
			break;
			
		case WAIT_FOR_G:
			if (isdigit(c))
			{
				current_green *= 10;
				current_green += (c - 0x30);
			}
			else if (c == ' ')
			{
				current_blue = 0;
				state = WAIT_FOR_B;
			}
			break;

		case WAIT_FOR_B:
 			if (isdigit(c))
			{
				current_blue *= 10;
				current_blue += (c - 0x30);
			}
			else if (c == ' ')
			{
				command = 0;
				state = WAIT_FOR_COMMAND;
			}
			break;
			
		case WAIT_FOR_COMMAND:
 			if (isdigit(c))
			{
				command *= 10;
				command += (c - 0x30);
			}
			else if (c == '>')
			{
				bColorMsgReceived = true;
				state = WAIT_FOR_START;
			}
		break;

	}
	
}

#ifdef x
ISR(USART_RX_vect)
{
	uint8_t c;
	c = UDR0;
	ProcessReceiveChar(c);
}
#endif