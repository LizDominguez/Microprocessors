/*
	Lab 7 Part A
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Setting up the DAC to output 1.3V
*/

#include <avr/io.h>

void DAC_init()
{
	PORTA.DIR = 0x04; //DAC CH0
	
	DACA.CTRLA = DAC_ENABLE_bm | DAC_CH0EN_bm;
	DACA.CTRLB = 0x00;		//single channel operation on CH0
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	
}

void DAC_write(uint16_t data) {
	
	while ( !(DACA.STATUS & DAC_CH0DRE_bm));	//wait for data register to empty
	DACA.CH0DATA = data;
	
}



int main(void)
{
    DAC_init();	 
	      
	//Vdac = CHdata/0xFFF * Vref
	
    while(1){
		
		DAC_write(0x890);
	
	}
	
}