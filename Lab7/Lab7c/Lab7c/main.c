/*
	Lab 7 Part C
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Generating 300 Hz sine wave using DAC and DMA
*/

#include <avr/io.h>
#include <avr/interrupt.h>

uint16_t sine[] = {0x800,0x880,0x900,0x97f,0x9fd,0xa78,0xaf1,0xb67,0xbda,0xc49,0xcb3,0xd19,0xd79,0xdd4,0xe29,
	0xe78,0xec0,0xf02,0xf3c,0xf6f,0xf9b,0xfbf,0xfdb,0xfef,0xffb,0xfff,0xffb,0xfef,0xfdb,0xfbf,
	0xf9b,0xf6f,0xf3c,0xf02,0xec0,0xe78,0xe29,0xdd4,0xd79,0xd19,0xcb3,0xc49,0xbda,0xb67,0xaf1,
	0xa78,0x9fd,0x97f,0x900,0x880,0x800,0x77f,0x6ff,0x680,0x602,0x587,0x50e,0x498,0x425,0x3b6,
	0x34c,0x2e6,0x286,0x22b,0x1d6,0x187,0x13f,0xfd,0xc3,0x90,0x64,0x40,0x24,0x10,0x4,
	0x0,0x4,0x10,0x24,0x40,0x64,0x90,0xc3,0xfd,0x13f,0x187,0x1d6,0x22b,0x286,0x2e6,
	0x34c,0x3b6,0x425,0x498,0x50e,0x587,0x602,0x680,0x6ff,0x77f,0x800};
	

volatile uint16_t i;


void CLK_init(void) {
	
	TCE0.PER = 67;					//300 Hz frequency
	TCE0.CTRLA = 0x01; 
	TCE0.CNT = 0x00;				//reset counter
	TCE0.INTCTRLA = 0x02;			//enable interrupt
	
}

void DMA_init() {
	
	DMA.CTRL = DMA_ENABLE_bm;							//enable, single buffer, round robin

	DMA.CH0.CTRLA = DMA_CH_ENABLE_bm |
					DMA_CH_SINGLE_bm |					//enable, repeat, single 2 byte burst
					DMA_CH_REPEAT_bm | 
					DMA_CH_BURSTLEN_2BYTE_gc; 
					
	DMA.CH0.ADDRCTRL = DMA_CH_SRCRELOAD_BLOCK_gc |		//reload transaction, increment source addr
						DMA_CH_SRCDIR_INC_gc |
						DMA_CH_DESTRELOAD_BURST_gc |
						DMA_CH_DESTDIR_INC_gc; 
						
	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_TCE0_OVF_gc;		//DACA CH0 as trigger source
	DMA.CH0.TRFCNT = 200;								//number of samples
	
	DMA.CH0.SRCADDR0 = ((uint16_t)(&sine[0]) >> 0) & 0xFF;
	DMA.CH0.SRCADDR1 = ((uint16_t)(&sine[0]) >>  8) & 0xFF;
	DMA.CH0.SRCADDR2 = 0;
	DMA.CH0.DESTADDR0 = ((uint16_t)(&DACA.CH0DATA) >> 0) & 0xFF;
	DMA.CH0.DESTADDR1 = ((uint16_t)(&DACA.CH0DATA) >> 8) & 0xFF;
	DMA.CH0.DESTADDR2 = 0;
	
}

void DAC_init()
{
	PORTA.DIR = 0x04;								//DAC CH0
	
	DACA.CTRLA = DAC_ENABLE_bm | DAC_CH0EN_bm;
	DACA.CTRLB = 0x00;								//single channel operation on CH0
	DACA.CTRLC = DAC_REFSEL_AREFB_gc;
	
}

void DAC_write(uint16_t data) {
	
	while (!(DACA.STATUS & DAC_CH0DRE_bm));			//wait for data register to empty
	DACA.CH0DATA = data;
	
}

ISR(TCE0_OVF_vect){	
	i++;	
}


int main(void)
{
	CLK_init();
    DAC_init();	
	DMA_init();
	
	PMIC_CTRL = 0x02;
	sei();
	
    while(1);
	
	
}