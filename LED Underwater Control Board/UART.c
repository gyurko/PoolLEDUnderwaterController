/*
 * UART.c
 *
 * Created: 9/8/2016 4:03:30 PM
 *  Author: gyurk
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <ctype.h>
#include "PWM.h"
#include "ADC.h"

// protocol is "<C XXX>" @4800 
// where C is R, G, B, or U   ---- U is send back colors and temperatures

#define START_CHAR '<'
#define END_CHAR '>'

extern volatile uint8_t current_red, current_green, current_blue;
extern volatile uint8_t command, command_data;
extern void UpdateColor(void);

typedef enum 
{
	WAIT_FOR_START,
	WAIT_FOR_COMMAND,
	WAIT_FOR_DATA
}state_t;

static state_t state = WAIT_FOR_START;
extern bool bCommandReceived;

void InitUART(void)
{
	// Set baud rate to 4800
	UBRR0H = 0;
	UBRR0L = 12;
	// 8 data bits, no parity, 1 stop bit
	UCSR0C = (3<<UCSZ00);
	// Enable receiver
	UCSR0B = (1<<RXEN0);
	// Enable transmitter
	UCSR0B |= (1<<TXEN0);
	// enable receive interrupt
	UCSR0B |= (1<<RXCIE0);
}


void ProcessReceiveChar(uint8_t c)
{
	// anytime there's a start char, reset everything
 	if (c == START_CHAR)
	{
		state = WAIT_FOR_COMMAND;
	}
	else
	{
		switch (state)
		{
			case WAIT_FOR_START:
				break;
				
			case WAIT_FOR_COMMAND:
				if ((!isdigit(c)) && (c != ' ') && (c != '>'))
				{
					command = c;
				}
				else if (c == ' ')
				{
					command_data = 0;
					state = WAIT_FOR_DATA;
				}
				else if (c == END_CHAR)
				{
					bCommandReceived = true;
					state = WAIT_FOR_START;
				}
				break;
		
			case WAIT_FOR_DATA:
				if (isdigit(c))
				{
					command_data *= 10;
					command_data += (c - 0x30);
				}
				if (c == END_CHAR)
				{
					bCommandReceived = true;
					state = WAIT_FOR_START;
				}
				break;
		}
	}
	
}

void SendString(char* pString)
{
	uint8_t i = 0;
	
	while (pString[i] != 0)
	{
		while ( !( UCSR0A & (1<<UDRE0)) )
		;
		UDR0 = pString[i];
		i++;
	}
}

ISR(USART_RX_vect)
{
	uint8_t c;
	c = UDR0;
	ProcessReceiveChar(c);
}
