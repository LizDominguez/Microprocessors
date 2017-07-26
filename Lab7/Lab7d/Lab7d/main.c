/*
	Lab 7 Part D
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Function Generator that generates sine or triangle waves at different freq
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#define BSel  137		//28.8k Baud
#define BScale  -1

uint16_t sine[] = {0x800,0x880,0x900,0x97f,0x9fd,0xa78,0xaf1,0xb67,0xbda,0xc49,0xcb3,0xd19,0xd79,0xdd4,0xe29,
	0xe78,0xec0,0xf02,0xf3c,0xf6f,0xf9b,0xfbf,0xfdb,0xfef,0xffb,0xfff,0xffb,0xfef,0xfdb,0xfbf,
	0xf9b,0xf6f,0xf3c,0xf02,0xec0,0xe78,0xe29,0xdd4,0xd79,0xd19,0xcb3,0xc49,0xbda,0xb67,0xaf1,
	0xa78,0x9fd,0x97f,0x900,0x880,0x800,0x77f,0x6ff,0x680,0x602,0x587,0x50e,0x498,0x425,0x3b6,
	0x34c,0x2e6,0x286,0x22b,0x1d6,0x187,0x13f,0xfd,0xc3,0x90,0x64,0x40,0x24,0x10,0x4,
	0x0,0x4,0x10,0x24,0x40,0x64,0x90,0xc3,0xfd,0x13f,0x187,0x1d6,0x22b,0x286,0x2e6,
	0x34c,0x3b6,0x425,0x498,0x50e,0x587,0x602,0x680,0x6ff,0x77f,0x800};
	
uint16_t tri[] = {0x52,0xa4,0xf6,0x148,0x19a,0x1eb,0x23d,0x28f,0x2e1,0x333,0x385,0x3d7,0x429,0x47b,0x4cd,
	0x51e,0x570,0x5c2,0x614,0x666,0x6b8,0x70a,0x75c,0x7ae,0x800,0x851,0x8a3,0x8f5,0x947,0x999,
	0x9eb,0xa3d,0xa8f,0xae1,0xb33,0xb84,0xbd6,0xc28,0xc7a,0xccc,0xd1e,0xd70,0xdc2,0xe14,0xe66,
	0xeb7,0xf09,0xf5b,0xfad,0xfff,0xfad,0xf5b,0xf09,0xeb7,0xe66,0xe14,0xdc2,0xd70,0xd1e,0xccc,
	0xc7a,0xc28,0xbd6,0xb84,0xb33,0xae1,0xa8f,0xa3d,0x9eb,0x999,0x947,0x8f5,0x8a3,0x851,0x800,
	0x7ae,0x75c,0x70a,0x6b8,0x666,0x614,0x5c2,0x570,0x51e,0x4cd,0x47b,0x429,0x3d7,0x385,0x333,
0x2e1,0x28f,0x23d,0x1eb,0x19a,0x148,0xf6,0xa4,0x52,0x0,0xfae};

uint32_t Freq[] = {0, 6500, 3260, 2175, 1630, 1300, 1085, 930, 815, 724};
//0, 50, 100, 150, 200, 250, 300, 350, 400, 450

char* Menu[] = {"s: output sinusoid",
				"t: output triangle wave",
				"0-9: make output waveform keyboard input*50Hz"};

volatile uint16_t i;


void CLK_init(void) {
	
	OSC.CTRL = 0x02;							//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 							//IOREG
	CLK.CTRL = 0x01; 							//bit 0 is 32 MHz internal oscillator
	
	TCE0.PER = Freq[1];				//50 Hz frequency
	TCE0.CTRLA = 0x01;				//enable counter
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

void USART_init() {
	
	PORTD.DIRSET = 0xF8;	//set TxD and RBG LED as output
	PORTD.OUTSET = 0xF8;	//set TxD to high, LED to off
	PORTD.DIRCLR = 0x04;	//set RxD pin as input
	
	//PortQ bits 1 and 3 enable and select
	PORTQ.DIRSET = 0x0A;	//PortD bits 2 and 3 serial pins
	PORTQ.OUTCLR = 0x0A;	//connected to the USB lines
	
	USARTD0.BAUDCTRLA = (BSel & 0xFF);			//sets BAUDCTRLA to lower 8 bits of BSel
	USARTD0.BAUDCTRLB = ((BScale << 4) & 0xF0) | ((BSel >> 8) & 0x0F);
	USARTD0.CTRLB = 0x18;						//RXEN/TEXEN
	USARTD0.CTRLC = 0x0B;						//asynchronous/no parity/1 stop bit/8 data
	
}

char receive_char()
{
	while( !(USARTD0.STATUS & USART_RXCIF_bm) );		//Interesting DRIF didn't work.
	return USARTD0.DATA;
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


void output_sine() {
	while(DMA.STATUS & 0x10);
	DMA.CH0.SRCADDR0 = ((uint16_t)(&sine[0]) >> 0) & 0xFF;
	DMA.CH0.SRCADDR1 = ((uint16_t)(&sine[0]) >>  8) & 0xFF;
	DMA.CH0.SRCADDR2 = 0;
}

void output_triangle() {
	while(DMA.STATUS & 0x10);
	DMA.CH0.SRCADDR0 = ((uint16_t)(&tri[0]) >> 0) & 0xFF;
	DMA.CH0.SRCADDR1 = ((uint16_t)(&tri[0]) >>  8) & 0xFF;
	DMA.CH0.SRCADDR2 = 0;
}


void determine_function(char op) {
	
	int index = 0;
	
	switch(op) {

		case 's':
		output_sine();
		break;
		
		case 't':
		output_triangle();
		break;
		
		default:
		index = (int)op - 0x30;
		if (index == 0) {
			TCE0.CTRLA = 0x00;				//enable counter
		}
		else{
			TCE0.CTRLA = 0x01;				//enable counter
			TCE0.PER = Freq[index];
			
		}
		
		break;

	}
}

ISR(TCE0_OVF_vect){	
	i = 1;	
}


int main(void)
{
	CLK_init();
    DAC_init();	
	DMA_init();
	USART_init();
	
	PMIC_CTRL = 0x02;
	sei();
		
	volatile char option;
	
    while(1){
		
		option = receive_char();
		determine_function(option);
		
		//display menu
		for(volatile int i = 0; i < sizeof(Menu); i++){
			send_string(Menu[i]);
			send_string("\n");
		}
	
		

	}
	
	
}