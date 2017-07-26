/*
	Lab 7 Part B
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Generating sine wave using DAC
*/

#include <avr/io.h>

uint16_t sine[] = {0x800,0x8c8,0x98f,0xa52,0xb0f,0xbc5,0xc71,0xd12,0xda7,0xe2e,0xea6,0xf0d,0xf63,0xfa7,0xfd8,
	0xff5,0xfff,0xff5,0xfd8,0xfa7,0xf63,0xf0d,0xea6,0xe2e,0xda7,0xd12,0xc71,0xbc5,0xb0f,0xa52,
	0x98f,0x8c8,0x800,0x737,0x670,0x5ad,0x4f0,0x43a,0x38e,0x2ed,0x258,0x1d1,0x159,0xf2,0x9c,
	0x58,0x27,0xa,0x0,0xa,0x27,0x58,0x9c,0xf2,0x159,0x1d1,0x258,0x2ed,0x38e,0x43a,
	0x4f0,0x5ad,0x670,0x737,0x800};


void CLK_init(void) {
	
	OSC.CTRL = 0x02;							//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 							//IOREG
	CLK.CTRL = 0x01; 							//bit 0 is 32 MHz internal oscillator
	
}

void DAC_init()
{
	PORTA.DIR = 0x04; //DAC CH0
	
	DACA.CTRLA = DAC_ENABLE_bm | DAC_CH0EN_bm;
	DACA.CTRLB = 0x00;		//single channel operation on CH0
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	
}

void DAC_write(uint16_t data) {
	
	while (!(DACA.STATUS & DAC_CH0DRE_bm));	//wait for data register to empty
	DACA.CH0DATA = data;
	
}

void delay_50us(void)
{
	volatile uint32_t ticks;				//Volatile prevents compiler optimization
	for(ticks = 0; ticks <= 37; ticks++);	//increment 2e6 times -> ~ 1 sec
}

int main(void)
{
	CLK_init();
    DAC_init();	 
	
    while(1){
		
		for (int i = 0, n = sizeof(sine)/sizeof(uint16_t); i < n; i++) {
			DAC_write(sine[i]);
			delay_50us();
			
		}
	
	}
	
}