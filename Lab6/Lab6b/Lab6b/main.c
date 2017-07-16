/*
	Lab 6 Part B
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Setting up the SPI
*/

#include <avr/io.h>



void SPI_init()
{
	PORTF.DIR = 0xB0; //SS, MOSI, SCK

	SPIF.CTRL = SPI_ENABLE_bm |				//enable
				SPI_DORD_bm	  |				//data order lsb
				SPI_MASTER_bm |				//Master select
				SPI_MODE0_bm  |				//mode 0 rising leading edge
				SPI_PRESCALER_DIV16_gc;		//prescaler of 16
	

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
    SPI_init();	  
	      
	volatile uint8_t writeRes, readRes;
	
    while(1){
		
		writeRes = spiWrite(0x53);
		readRes = spiRead();
	
	
	}
	
}