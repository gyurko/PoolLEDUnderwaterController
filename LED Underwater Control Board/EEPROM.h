/*
 * EEPROM.h
 *
 * Created: 9/9/2016 11:37:54 AM
 *  Author: gyurk
 */ 
#include <avr/io.h>


#ifndef EEPROM_H_
#define EEPROM_H_

#define RED_EEPROM_ADDR 0
#define GREEN_EEPROM_ADDR 1
#define BLUE_EEPROM_ADDR 2

void EEPROM_write(uint16_t uiAddress, uint8_t ucData);
uint8_t EEPROM_read(uint16_t uiAddress);


#endif /* EEPROM_H_ */