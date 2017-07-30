/*
 * ADC.h
 *
 * Created: 7/20/2017 1:09:53 PM
 *  Author: gyurk
 */ 


#ifndef ADC_H_
#define ADC_H_

#define CPU_TEMP_CHANNEL 0
#define LED_TEMP_CHANNEL 1

void ADCInit();
uint16_t ReadTemperatue(uint8_t channel);
uint16_t ADCRead(uint8_t channel);


#endif /* ADC_H_ */