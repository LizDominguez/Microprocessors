/*
	Lab 5 Part 3
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Using the ADC to create a voltometer
*/

#include <avr/io.h>


#define F_CPU 2000000  
#define BSel  135		//57.6k Baud
#define BScale  -2     


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

void USART_init() {
	
		USARTD0.CTRLB = 0x18; //RXEN/TEXEN
		USARTD0.CTRLC = 0x3B; //asynchronous/odd parity/1 stop bit/8 data
		USARTD0.BAUDCTRLA = (BSel & 0xFF); //sets BAUDCTRLA to lower 8 bits of BSel
		USARTD0.BAUDCTRLB = ((BScale << 4) & 0xF0) | ((BSel >> 8) & 0x0F);
	
}

void IO_init() {
		
		PORTD.DIRSET = 0x08;	//set TxD as output	
		PORTD.OUTSET = 0x08;	//set TxD to high
		PORTD.DIRCLR = 0x04;	//set RxD pin as input
		
									//PortQ bits 1 and 3 enable and select
		PORTQ.DIRSET = 0x0A;	//PortD bits 2 and 3 serial pins
		PORTQ.OUTCLR = 0x0A;   //connected to the USB lines
		
}


uint16_t ADC_READ() {
	
	uint16_t adcVal = 0;
	
	ADCA.CTRLA = 0x01;				//start conversion
	while (ADCA.INTFLAGS == 0);		//wait until conversion is complete
	
	adcVal = ADCA.CH0.RES;			//reading from ADC
	
	ADCA.INTFLAGS = ADCA.INTFLAGS;	//clear flag
	
	return adcVal;
	
}

void DELAY(void)
{
	volatile uint32_t ticks;            //Volatile prevents compiler optimization
	for(ticks=0;ticks<=F_CPU;ticks++);	//increment 2e6 times -> ~ 1 sec
}


int main(void)
{
   

    ADC_init();	        
	USART_init();
	IO_init();
	
	uint16_t adcVal = 0;

	
    while(1){

		adcVal = ADC_READ();
		DELAY();
		
	
	}
	
}