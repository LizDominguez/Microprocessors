/*
	Lab 5 Part 1
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Setting up the EBI and writing to addresses 
*/

#include <avr/io.h>
#include "ebi_driver.h"


#define F_CPU 2000000       
#define CS0_Start 0x8000
#define CS0_End 0x9FFF


void EBI_init()
{
    PORTH.DIR = 0x17;       // Enable RE, WE, ALE1, CS0
    PORTH.OUT = 0x13;
    PORTK.DIR = 0xFF;       // Enable Address 7:0 (outputs)
  
    
    EBI.CTRL = EBI_SRMODE_ALE1_gc | EBI_IFMODE_3PORT_gc;            // ALE1 multiplexing, 3 port configuration

    EBI.CS0.BASEADDRH = (uint8_t) (CS0_Start>>16) & 0xFF;
    EBI.CS0.BASEADDRL = (uint8_t) (CS0_Start>>8) & 0xFF;            // Set CS0 range to 0x008000 - 0x009FFF
    EBI.CS0.CTRLA = EBI_CS_MODE_SRAM_gc | EBI_CS_ASPACE_8KB_gc;	    // SRAM mode, 8k address space	

}


int main(void)
{
    uint8_t volatile *ptr_8 = (uint16_t)0x8500;        // memory location 0x8500
    uint16_t volatile *ptr_16 = (uint16_t)0x8501;    // memory location 0x8501
	
	
    volatile uint8_t write_37 = 0x37;
    volatile uint8_t write_73 = 0x73;

    EBI_init();	        //Call init EBI function

	
    while(1){
		
	*ptr_8 = write_37;    //write 0x37 into 0x8500
	*ptr_16 = write_73;  //write 0x73 into 0x8501
	
	}
	
}