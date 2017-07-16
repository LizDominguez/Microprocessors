/*
	Quiz
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Using the ADC to sample a photoresistor
*/

#include <avr/io.h> 
#include "ebi_driver.h"

#define CS0_Start 0x128000
#define CS0_End	  0x148000


void ADC_init(void)
{
	
	PORTA.DIR = 0x00;			//Port A as input
	ADCA.CTRLA = 0x01;			//channel 0 enabled
	ADCA.CTRLB = 0x0C;			// 8 bit right adjusted, conversion mode
	ADCA.REFCTRL = 0x30;		//AREFB
	
	ADCA.CH0.CTRL = 0x81;		//single-ended with gain of 1  00001
	ADCA.CH0.MUXCTRL = 0x0A;	//Pin 1 and 6  0001010

	
	ADCA.INTFLAGS = 0x01;		// clearing flag
	
	//CDS+ PortA pin 1 ADC1
	//CDS- PortA pin 6 ADC6
	
}


void EBI_init()
{
	PORTH.DIR = 0x17;       // Enable RE, WE, ALE1, CS0
	PORTH.OUT = 0x13;
	PORTK.DIR = 0xFF;       // Enable Address 7:0 (outputs)
	
	
	EBI.CTRL = EBI_SRMODE_ALE1_gc | EBI_IFMODE_3PORT_gc;				// ALE1 multiplexing, 3 port configuration

	EBI.CS0.BASEADDR = (uint16_t) (CS0_Start >> 8) & 0xFFFF;  			// Set CS0 range to 0x128000 - 0x148000
	EBI.CS0.CTRLA = EBI_CS_MODE_SRAM_gc | EBI_CS_ASPACE_32KB_gc;	    // SRAM mode, 32k address space

}


uint16_t ADC_read() {
	
	
	ADCA.CH0.CTRL |= (1 << ADC_CH_START_bp);			//start conversion
	while(!(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm));		//wait until conversion is complete
	ADCA.INTFLAGS = 0x01;								//clear flag
	
	
	return ADCA.CH0.RES;
	
	
}



int main(void)
{

	 EBI_init();	        //Call init EBI function
	 ADC_init();
	 
	 volatile uint16_t adcVal = 0;
	 volatile uint16_t mem_value = 0;

	 
	 while(1){
		 
		 adcVal = ADC_read();
		 __far_mem_write(0x128000, adcVal);
		 mem_value = __far_mem_read(0x128000);
	 }
	

	
}