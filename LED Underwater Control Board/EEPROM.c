/*
 * EEPROM.c
 *
 * Created: 9/9/2016 11:14:25 AM
 *  Author: gyurk
 */ 
#include "EEPROM.h"
#include <avr/io.h>


void EEPROM_write(uint16_t uiAddress, uint8_t ucData)
{
	// Wait for completion of previous write
	while(EECR & (1 << EEPE));
	
	// Set up address and data registers
	EEAR = uiAddress;
	EEDR = ucData;
	
	// Write logical one to EEMWE
	EECR |= (1 << EEMPE);
	
	// Start eeprom write by setting EEPE
	EECR |= (1 << EEPE);
}

uint8_t EEPROM_read(uint16_t uiAddress)
{
	// Wait for completion of previous write
	while(EECR & (1 << EEPE));
	
	// Set up address register
	EEAR = uiAddress;
	
	// Start eeprom read by writing EERE
	EECR |= (1 << EERE);
	
	// Return data from data register
	return EEDR;
}