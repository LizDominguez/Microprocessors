/*
	Lab 6 Part B
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Setting up the SPI
*/

#include <avr/io.h>


void CLK_init(void) {
	
	OSC.CTRL = 0x02;							//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 							//IOREG
	CLK.CTRL = 0x01; 							//bit 0 is 32 MHz internal oscillator
	
}

void SPI_init()
{
	PORTF.DIR = 0xB0; //SS, MOSI, SCK
	
	SPIF.CTRL = SPI_ENABLE_bm |				//enable
				SPI_DORD_bm	  |				//data order lsb
				SPI_MASTER_bm |				//Master select
				SPI_MODE0_bm  |				//mode 0 rising leading edge
				SPI_PRESCALER_DIV128_gc;	//prescaler of 128
	

}


uint8_t spiWrite(uint8_t data) {
				
		SPIF.DATA = data;					//enable SPI
		while(!(SPIF.STATUS & 0x80));		//wait for serial transfer
		
		return SPIF.DATA;
	
}

uint8_t spiRead() {
	
	return spiWrite(0xFF);
	
}


int main(void)
{
	CLK_init();
    SPI_init();	 
	      
	volatile uint8_t writeRes, readRes;
	
    while(1){
		
		writeRes = spiWrite(0x53);
		readRes = spiRead();
	
	
	}
	
}