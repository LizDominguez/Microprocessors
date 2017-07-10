/*
	Lab 5 Part 2
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Using the ADC to sample a photoresistor
*/

#include <avr/io.h>


#define F_CPU 2000000       



void ADC_init()
{
	
	ADCA.CTRLA = 0x01; //channel 0 & 1 enabled 
	ADCA.CTRLB = 0x1C; // 8 bit right adjusted, conversion mode
	ADCA.REFCTRL = 0x30; //AREFB
	ADCA.CH0.CTRL = 0x83; //Dif with gain of 1  00011
	ADCA.CH0.MUXCTRL = 0x00; //ADC0 differential with gain
	ADCA.CH0.INTCTRL = 0 ; // No interrupt
	ADCA.INTFLAGS = 0x01; // clearing flag
	PORTA.DIR &= ~0x01; //CH0 as input
}

uint16_t ADC_READ() {
	
	uint16_t adcVal = 0;
	
	ADCA.CTRLA = 0x01;				//start conversion
	while (ADCA.INTFLAGS == 0);		//wait until conversion is complete
	
	adcVal = ADCA.CH0.RES;			//reading from ADC
	
	ADCA.INTFLAGS = ADCA.INTFLAGS;	//clear flag
	
	return adcVal;			
}


int main(void)
{
   

    ADC_init();	        //Call init EBI function
	uint16_t reading = 0;
	
    while(1){
		
		reading = ADC_READ();
	
	}
	
}