/*
	Lab 5 Part 4
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Using the ADC to sample a DAD board
*/

#include <avr/io.h>

#define F_CPU 2000000
#define BSel  137		//28.8k Baud
#define BScale  -1

void CLK_init(void) {
	
	OSC.CTRL = 0x02;					//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 					//IOREG
	CLK.CTRL = 0x01; 					//bit 0 is 32 MHz internal oscillator
	
}

void USART_init(void) {
	
	USARTD0.BAUDCTRLA = (BSel & 0xFF);			//sets BAUDCTRLA to lower 8 bits of BSel
	USARTD0.BAUDCTRLB = ((BScale << 4) & 0xF0) | ((BSel >> 8) & 0x0F);
	USARTD0.CTRLB = 0x18;						//RXEN/TEXEN
	USARTD0.CTRLC = 0x0B;						//asynchronous/no parity/1 stop bit/8 data
}



void IO_init(void) {
	
	PORTD.DIRSET = 0xF8;	//set TxD and RBG LED as output
	PORTD.OUTSET = 0xF8;	//set TxD to high, LED to off
	PORTD.DIRCLR = 0x04;	//set RxD pin as input
	
	//PortQ bits 1 and 3 enable and select
	PORTQ.DIRSET = 0x0A;	//PortD bits 2 and 3 serial pins
	PORTQ.OUTCLR = 0x0A;	//connected to the USB lines
	
	
}

void send_char(char c)
{
	
	while( !(USARTD0.STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	
	USARTD0.DATA = c;
	
}



void ADC_init(void)
{
	
	PORTA.DIR = 0x00;			//Port A as input
	ADCA.CTRLA = 0x09;			//enable
	ADCA.CTRLB = 0x1C;			// 8 bit right adjusted, conversion mode
	ADCA.REFCTRL = 0x30;		//AREFB
	ADCA.EVCTRL = 0x40;
	
	ADCA.CH1.CTRL = 0x83;		//Dif with gain of 1  
	ADCA.CH1.MUXCTRL = 0x21;	//Pin 4 and 5  0100010

	
	ADCA.INTFLAGS = 0x01;		// clearing flag
	
	//IN+ PortA pin 4 
	//IN- PortA pin 5 
	
}


uint16_t ADC_read() {
	
	
	ADCA.CH1.CTRL |= (1 << ADC_CH_START_bp);			//start conversion
	while(!(ADCA.CH1.INTFLAGS & ADC_CH_CHIF_bm));		//wait until conversion is complete
	ADCA.INTFLAGS = 0x01;								//clear flag
	
	
	return ADCA.CH1.RES;
	
	
}


void delay_3s(void)
{
	volatile uint32_t ticks;				//Volatile prevents compiler optimization
	for(ticks = 0; ticks <= 2000000; ticks++);	//increment 2e6 times -> ~ 1 sec
}

void send_Values() {
	
	volatile int8_t adcVal = 0;
	volatile int intVal = 0;
	volatile float voltVal = 0;
	
	adcVal = (ADC_read() << 2);

	//voltVal = (1/50)*adcVal + .0098
	voltVal = adcVal;
	voltVal /= 50;
	voltVal += .0098;

	if (voltVal >= 0) {
		send_char((char)0x2B);				//+/-
	}
	
	else  {
		send_char((char)0x2D);
		voltVal *= -1;
	}
	
	intVal = (int)voltVal;					//1st iteration
	send_char((char)(intVal + '0'));
	send_char((char)0x2E);					//.
	
	voltVal = 10*(voltVal - intVal);		//2nd iteration
	intVal = (int)voltVal;
	send_char((char)(intVal + '0'));
	
	voltVal = 10*(voltVal - intVal);		//3rd iteration
	intVal = (int)voltVal;
	send_char((char)(intVal + '0'));
	send_char((char)0x20);					//space
	
	//hex Values
	volatile int8_t hexVal = 0;
	hexVal = (adcVal >> 1);
	
	send_char((char)0x28);							//(
	send_char((char)0x30);							//0
	send_char((char)0x78);							//x;
	send_char((char)(hexVal + '0'));				//firs byte
	send_char((char)((adcVal & 0x0F) + '0'));		//second byte
	send_char((char)0x29);							//)
	send_char((char)0x20);							//space
	
	
}


int main(void)
{
   
	CLK_init();
	ADC_init();
	USART_init();
	IO_init();	     
	
	
    while(1){
		
		send_Values();
		delay_3s();
	
	
	}
	

	
}