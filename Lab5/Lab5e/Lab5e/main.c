/*
	Lab 5 Part 5
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: A menu with several operations using ADC, EBI, Timers, and interrupts
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ebi_driver.h"


#define F_CPU 2000000  
#define BSel  137		//28.8k Baud
#define BScale  -1
#define CS0_Start 0x8000
#define CS0_End 0x9FFF


char* Menu[] = {"A: Start Conversion of photo resistor and display on terminal", 
				"B: Start conversion on DAD board and display on terminal", 
				"C: Turn on Timer and start conversion every second", 
				"D: Turn off Timer/Counter", 
				"E: Start conversion and write this value to external SRAM at $8000", 
				"F: Read the external memory location at $8000"};     


void CLK_init() {
	
		OSC.CTRL = 0x02;					//32 Mhz internal oscillator enable
		while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
		CPU_CCP = 0xD8; 					//IOREG
		CLK.CTRL = 0x01; 					//bit 0 is 32 MHz internal oscillator
	
}



void ADC_init()
{
		PORTA.DIR = 0x00;			//Port A as input
		ADCA.CTRLA = 0x09;			//enable
		ADCA.CTRLB = 0x1C;			// 8 bit right adjusted, conversion mode
		ADCA.REFCTRL = 0x30;		//AREFB
		ADCA.EVCTRL = 0x40;
	
		ADCA.CH0.CTRL = 0x83;		//Dif with gain of 1  00011
		ADCA.CH0.MUXCTRL = 0x0A;	//Pin 1 and 6  0001010

			
		ADCA.CH1.CTRL = 0x83;		//Dif with gain of 1
		ADCA.CH1.MUXCTRL = 0x21;	//Pin 4 and 5  0100010
	
	
}



void USART_init() {
	
		USARTD0.BAUDCTRLA = (BSel & 0xFF);			//sets BAUDCTRLA to lower 8 bits of BSel
		USARTD0.BAUDCTRLB = ((BScale << 4) & 0xF0) | ((BSel >> 8) & 0x0F);
		USARTD0.CTRLB = 0x18;						//RXEN/TEXEN
		USARTD0.CTRLC = 0x0B;						//asynchronous/no parity/1 stop bit/8 data	
}



void IO_init() {
		
		PORTD.DIRSET = 0xF8;	//set TxD and RBG LED as output	
		PORTD.OUTSET = 0xF8;	//set TxD to high, LED to off
		PORTD.DIRCLR = 0x04;	//set RxD pin as input
		
								//PortQ bits 1 and 3 enable and select
		PORTQ.DIRSET = 0x0A;	//PortD bits 2 and 3 serial pins
		PORTQ.OUTCLR = 0x0A;	//connected to the USB lines
		
		
}



void EBI_init()
{
	PORTH.DIR = 0x17;       // Enable RE, WE, ALE1, CS0
	PORTH.OUT = 0x13;
	PORTK.DIR = 0xFF;       // Enable Address 7:0 (outputs)
	
	
	EBI.CTRL = EBI_SRMODE_ALE1_gc | EBI_IFMODE_3PORT_gc;            // ALE1 multiplexing, 3 port configuration

	EBI.CS0.BASEADDRH = (uint8_t) (CS0_Start >> 16) & 0xFF;
	EBI.CS0.BASEADDRL = (uint8_t) (CS0_Start >> 8) & 0xFF;            // Set CS0 range to 0x008000 - 0x009FFF
	EBI.CS0.CTRLA = EBI_CS_MODE_SRAM_gc | EBI_CS_ASPACE_8KB_gc;	    // SRAM mode, 8k address space

}



uint16_t read_photoresistor() {
	
	ADCA.CH0.CTRL |= (1 << ADC_CH_START_bp);			//start conversion
	while(!(ADCA.CH0.INTFLAGS & ADC_CH_CHIF_bm));		//wait until conversion is complete
	ADCA.INTFLAGS = 0x01;								//clear flag
	
	
	return ADCA.CH0.RES;
	
	
}



uint16_t read_DAD() {
	
	
	ADCA.CH1.CTRL |= (1 << ADC_CH_START_bp);			//start conversion
	while(!(ADCA.CH1.INTFLAGS & ADC_CH_CHIF_bm));		//wait until conversion is complete
	ADCA.INTFLAGS = 0x02;								//clear flag
	
	
	return ADCA.CH1.RES;
	
	
}



void send_char(char c)
{
	
	while( !(USARTD0.STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	
	USARTD0.DATA = c;
	
}



void send_string(char *text)
{
	while(*text)
	{
		send_char(*text++);
	}
}



char receive_char()
{
	while( !(USARTD0.STATUS & USART_RXCIF_bm) ); //Interesting DRIF didn't work.
	return USARTD0.DATA;
}





void SRAM_write(uint16_t CdS) {
	
	uint8_t volatile *ptr_8 = (uint16_t)0x8000;        // memory location 0x8000	
	*ptr_8 = CdS;    //write 0x37 into 0x8000	
	
}



void SRAM_read() {
	
	uint16_t CdS = 0;
	
	uint8_t volatile *ptr_8 = (uint16_t)0x8000;        // memory location 0x8000		
	CdS = *ptr_8;    //read 8-bit value into read_8
	
	send_char(CdS);
	
}


void delay_3s(void)
{
	volatile uint32_t ticks;				//Volatile prevents compiler optimization
	for(ticks = 0; ticks <= 200000; ticks++);	//increment 2e6 times -> ~ 1 sec
}



void delay_50us(void)
{
	volatile uint32_t ticks;				//Volatile prevents compiler optimization
	for(ticks = 0; ticks <= 200; ticks++);	//increment 2e6 times -> ~ 1 sec
}


void send_Values(int16_t adcVal) {
	
	volatile int intVal = 0;
	volatile float voltVal = 0;


	//voltVal = (1/50)*adcVal + .0098
	voltVal = (int8_t)(adcVal << 4);
	voltVal /= 50;
	voltVal += .0098;

	if (voltVal >= 0) {
		send_char((char)0x2B);				//+/-
	}
	
	else  {
		send_char((char)0x2D);
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




void determine_function(char op) {
	
	uint16_t adcVal = 0x00;
	
	switch(op) {

		case 'a':
		adcVal = read_photoresistor();
		send_Values(adcVal);
		break; 
		
		case 'b':
		adcVal = read_DAD();
		send_Values(adcVal);
		break; 
		
		case 'c':
		//timer_on();
		break;
		
		case 'd':
		//timer_off();
		break;
		
		case 'e':
		adcVal = read_photoresistor();
		SRAM_write(adcVal);
		break;
		
		case 'f':
		SRAM_read();
		break;
		
	}
	
	
}


int main(void)
{
   
	CLK_init();
    ADC_init();	 
	EBI_init();       
	USART_init();
	IO_init();
	
	sei();						//global interrupt enable

	char option = 0;

	
    while(1){
		
		//display menu
		for(volatile int i = 0; i < sizeof(Menu); i++){
			send_string(Menu[i]);
			send_string("\n");
			delay_50us();
		}
		
		send_string("\n");
		
		option = receive_char();
		determine_function(option);	
	
	}
	
}