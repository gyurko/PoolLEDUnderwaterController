#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/io.h>
#include "PWM.h"
#include "Timer.h"
#include "UART.h"
#include "EEPROM.h"
#include "ADC.h"

extern volatile uint8_t current_red, current_green, current_blue, command;
extern volatile bool bColorMsgReceived;
void ChangeColor(uint8_t r, uint8_t g, uint8_t b);
void MainLoop(void);

#define ONE_SECOND 1000

void ChangeColor(uint8_t r, uint8_t g, uint8_t b)
{
	if (r == 0)
	{
		DisablePWMChannel(CHAN_RED);
	}
	else
	{
		EnablePWMChannel(CHAN_RED);
		SetPWMChannelDutyCycle(CHAN_RED, r);
	}

	if (g == 0)
	{
		DisablePWMChannel(CHAN_GREEN);
	}
	else
	{
		EnablePWMChannel(CHAN_GREEN);
		SetPWMChannelDutyCycle(CHAN_GREEN, g);
	}

	if (b == 0)
	{
		DisablePWMChannel(CHAN_BLUE);
	}
	else
	{
		EnablePWMChannel(CHAN_BLUE);
		SetPWMChannelDutyCycle(CHAN_BLUE, b);
	}
}

void Delay(uint32_t t)
{
	uint32_t timer;
	
	timer = ElapsedTime(0);
	while (ElapsedTime(timer) <= t) {};
}

uint8_t digit;

#define FLASH_ON_TIME 200
#define FLASH_OFF_TIME 500
#define DIGIT_DELAY 1000

void FlashTemperature(uint16_t temperature)
{
	uint8_t i;
	
	// start by turning off the current color
	ChangeColor(0,0,0);
	Delay(DIGIT_DELAY);
	
	// do the red count
	digit = temperature / 100;
	for (i = 0; i < digit; i++)
	{
		ChangeColor(0x7f, 0, 0);
		Delay(FLASH_ON_TIME);
		ChangeColor(0,0,0);
		Delay(FLASH_OFF_TIME);
	}
	
	Delay(DIGIT_DELAY);

	// do the green count
	digit = (temperature - ((temperature / 100) * 100));
	digit /= 10;
	for (i = 0; i < digit; i++)
	{
		ChangeColor(0, 0x7F, 0);
		Delay(FLASH_ON_TIME);
		ChangeColor(0,0,0);
		Delay(FLASH_OFF_TIME);
	}

	Delay(DIGIT_DELAY);

	digit = temperature - ((temperature / 10) * 10);
	// do the blue count
	for (i = 0; i < digit; i++)
	{
		ChangeColor(0, 0, 0x7F);
		Delay(FLASH_ON_TIME);
		ChangeColor(0,0,0);
		Delay(FLASH_OFF_TIME);
	}
	ChangeColor(current_red, current_green, current_blue);
}



void MainLoop(void)
{
	while (1)
	{
		if (bColorMsgReceived == true)
		{
			ChangeColor(current_red, current_green, current_blue);
			
			// write the values to EEPROM
			EEPROM_write(RED_EEPROM_ADDR, current_red);
			EEPROM_write(GREEN_EEPROM_ADDR, current_green);
			EEPROM_write(BLUE_EEPROM_ADDR, current_blue);
					
			if (command == 1)
			{
				FlashTemperature(123);
			}
			else if (command == 2)
			{
				FlashTemperature(345);
			}
			command = 0;
			bColorMsgReceived = false;
			
		}
	}
}

uint16_t t1,t2;
	
int main (void)
{
	InitTimer();
	InitPWM();
	InitUART();
	ADCInit();
	sei();
	
	t1 = ReadTemperatue(CPU_TEMP_CHANNEL);
	t2 = ReadTemperatue(LED_TEMP_CHANNEL);

	// set the colors from last time
	current_red = EEPROM_read(RED_EEPROM_ADDR);
	current_green = EEPROM_read(GREEN_EEPROM_ADDR);
	current_blue = EEPROM_read(BLUE_EEPROM_ADDR);
	
	ChangeColor(current_red, current_green, current_blue);
	MainLoop();
}



