#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include "PWM.h"
#include "Timer.h"
#include "UART.h"
#include "EEPROM.h"
#include "ADC.h"

volatile uint8_t current_red, current_green, current_blue;
volatile uint8_t command, command_data;
volatile bool bCommandReceived;
void MainLoop(void);

#define ONE_SECOND 1000

typedef enum
{
	NORMAL,
	OVERTEMP_LED_ON,
	OVERTEMP_LED_OFF
}state_t;

static state_t temperatureState = NORMAL;

void SetLEDColor(uint8_t red, uint8_t green, uint8_t blue)
{
	if (red == 0)
	{
		DisablePWMChannel(CHAN_RED);
	}
	else
	{
		EnablePWMChannel(CHAN_RED);
		SetPWMChannelDutyCycle(CHAN_RED, red);
	}

	if (green == 0)
	{
		DisablePWMChannel(CHAN_GREEN);
	}
	else
	{
		EnablePWMChannel(CHAN_GREEN);
		SetPWMChannelDutyCycle(CHAN_GREEN, green);
	}

	if (blue == 0)
	{
		DisablePWMChannel(CHAN_BLUE);
	}
	else
	{
		EnablePWMChannel(CHAN_BLUE);
		SetPWMChannelDutyCycle(CHAN_BLUE, blue);
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
	//ChangeColor(0,0,0);
	Delay(DIGIT_DELAY);
	
	// do the red count
	digit = temperature / 100;
	for (i = 0; i < digit; i++)
	{
		//ChangeColor(0x7f, 0, 0);
		Delay(FLASH_ON_TIME);
		//ChangeColor(0,0,0);
		Delay(FLASH_OFF_TIME);
	}
	
	Delay(DIGIT_DELAY);

	// do the green count
	digit = (temperature - ((temperature / 100) * 100));
	digit /= 10;
	for (i = 0; i < digit; i++)
	{
		//ChangeColor(0, 0x7F, 0);
		Delay(FLASH_ON_TIME);
		//ChangeColor(0,0,0);
		Delay(FLASH_OFF_TIME);
	}

	Delay(DIGIT_DELAY);

	digit = temperature - ((temperature / 10) * 10);
	// do the blue count
	for (i = 0; i < digit; i++)
	{
		//ChangeColor(0, 0, 0x7F);
		Delay(FLASH_ON_TIME);
		//ChangeColor(0,0,0);
		Delay(FLASH_OFF_TIME);
	}
	//ChangeColor(current_red, current_green, current_blue);
}


#define OVERTEMP_LED_ON_TIME 500
#define OVERTEMP_LED_OFF_TIME 2000
#define OVERTEMP_RED_COLOR 0x30
#define OVERTEMP_GREEN_COLOR 0x10
#define OVERTEMP_BLUE_COLOR 0x10

void ProcessOvertemperature( void )
{
	static uint32_t t;
	
	switch (temperatureState)
	{
		case NORMAL:
			if (ReadTemperatue(LED_TEMP_CHANNEL) > 100)
			{
				SetLEDColor(OVERTEMP_RED_COLOR, OVERTEMP_GREEN_COLOR, OVERTEMP_BLUE_COLOR);
				t = ElapsedTime(0);
				temperatureState = OVERTEMP_LED_ON;
			}
			break;
			
		case OVERTEMP_LED_OFF:
			if (ReadTemperatue(LED_TEMP_CHANNEL) < 80)
			{
				SetLEDColor(current_red, current_green, current_blue);
				temperatureState = NORMAL;
			}
			else if (ElapsedTime(t) > OVERTEMP_LED_OFF_TIME)
			{
				SetLEDColor(OVERTEMP_RED_COLOR, OVERTEMP_GREEN_COLOR, OVERTEMP_BLUE_COLOR);
				t = ElapsedTime(0);
				temperatureState = OVERTEMP_LED_ON;
			}
			break;
			
		case OVERTEMP_LED_ON:
			if (ReadTemperatue(LED_TEMP_CHANNEL) < 80)
			{
				SetLEDColor(current_red, current_green, current_blue);
				temperatureState = NORMAL;
			}
			else if (ElapsedTime(t) > OVERTEMP_LED_ON_TIME)
			{
				SetLEDColor(0,0,0);
				t = ElapsedTime(0);
				temperatureState = OVERTEMP_LED_OFF;
			}
			break;
	}
}

void ProcessCommand(void)
{
	char returnBuf[25];
	
	switch (command)
	{
		case 'r':
		case 'R':
			current_red = command_data;
			SetLEDColor(current_red, current_green, current_blue);
			break;
		
		case 'g':
		case 'G':
			current_green = command_data;
			SetLEDColor(current_red, current_green, current_blue);
			break;
		
		case 'b':
		case 'B':
			current_blue = command_data;
			SetLEDColor(current_red, current_green, current_blue);
			break;

		case 'u':
		case 'U':
			sprintf(returnBuf, "[%03d %03d %03d %03d %03d]", current_red, current_green, current_blue, ReadTemperatue(CPU_TEMP_CHANNEL), ReadTemperatue(LED_TEMP_CHANNEL));
			SendString(returnBuf);
			break;
	}
}

bool bOvertemp = false;

void MainLoop(void)
{
	bCommandReceived = false;
	
	while (1)
	{
		if (bCommandReceived == true)
		{
			ProcessCommand();
			
			// write the values to EEPROM
			EEPROM_write(RED_EEPROM_ADDR, current_red);
			EEPROM_write(GREEN_EEPROM_ADDR, current_green);
			EEPROM_write(BLUE_EEPROM_ADDR, current_blue);
			
			bCommandReceived = false;			
		}
		ProcessOvertemperature();
	}
}

int main (void)
{
	InitTimer();
	InitPWM();
	InitUART();
	ADCInit();
	sei();
	
	// set the colors from last time
	current_red = EEPROM_read(RED_EEPROM_ADDR);
	current_green = EEPROM_read(GREEN_EEPROM_ADDR);
	current_blue = EEPROM_read(BLUE_EEPROM_ADDR);
	
	SetLEDColor(current_red, current_green, current_blue);
	MainLoop();
}



