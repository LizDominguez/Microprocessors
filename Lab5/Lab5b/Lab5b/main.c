/*
	Lab 5 Part 2
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Using the ADC to sample a photoresistor
*/

#include <avr/io.h> 


void CLK_init(void) {
	
	OSC.CTRL = 0x02;					//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 					//IOREG
	CLK.CTRL = 0x01; 					//bit 0 is 32 MHz internal oscillator
	
}


void ADC_init(void)
{
	
	PORTA.DIR = 0x00;			//Port A as input
	ADCA.CTRLA = 0x01;			//channel 0 enabled
	ADCA.CTRLB = 0x0C;			// 8 bit right adjusted, conversion mode
	ADCA.REFCTRL = 0x30;		//AREFB
	ADCA.CH0.CTRL = 0x83;		//Dif with gain of 1  00011
	ADCA.CH0.MUXCTRL = 0x0A;	//Pin 1 and 6  0001010

	
	ADCA.INTFLAGS = 0x01;		// clearing flag
	
	//CDS+ PortA pin 1 ADC1
	//CDS- PortA pin 6 ADC6
	
}


uint16_t ADC_read() {
	
	
	ADCA.CH0.CTRL |= (1 << ADC_CH_START_bp);			//start conversion
	while(!(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm));		//wait until conversion is complete
	ADCA.INTFLAGS = 0x01;								//clear flag
	
	
	return ADCA.CH0.RES;
	
	
}



int main(void)
{
   
	CLK_init();
	EBI_init();
    ADC_init();	     

	volatile int16_t adcVal = 0x00; 
	
    while(1){
		
		adcVal = ADC_read();								//reading from ADC
	
	}
	

	
}