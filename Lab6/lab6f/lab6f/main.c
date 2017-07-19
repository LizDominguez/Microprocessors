/*
	Lab 6 Part F
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Setting up the SPI and LSM330 to output XYZ via USART
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "LSM330.h"

#define BSel  1		//1Mbps
#define BScale  0


volatile uint8_t accelDataReady = 0x00;
volatile uint8_t gyroDataReady = 0x00;


void CLK_init(void) {
	
	OSC.CTRL = 0x02;							//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 							//IOREG
	CLK.CTRL = 0x01; 							//bit 0 is 32 MHz internal oscillator
	
}


void SPI_init()
{
	PORTA.DIR = 0xF8;		//Protocol Sel, others
	PORTA.OUT = 0x00;		//Protocol sel SPI
	PORTF.DIR = 0xBF;		//SSA, MOSI, SCK, MISO as input
	PORTF.OUT = 0x18;		//SSA & SSG
	PORTC.DIR = 0x00;		//data direction
	
	SPIF.CTRL = SPI_ENABLE_bm |				//enable, data order msb
				SPI_MASTER_bm |				//Master select
				SPI_MODE_0_gc;				//mode 3 rising leading edge		
				
}

void USART_init(void) {
	
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

uint8_t spiWrite(uint8_t data) {
			
		SPIF.DATA = data;					//enable SPI
		while(!(SPIF.STATUS & 0x80));		//wait for serial transfer
		
		return SPIF.DATA;
			
}

uint8_t spiRead() {
	
	return spiWrite(0xFF);
	
}

uint8_t accRead(uint8_t reg){
	
	volatile uint8_t read;
	
	PORTF.OUTCLR = 0x08;		//SSA clear
	PORTF.OUTSET = 0x04;		//Sensor sel acc
	
	//RW, MS, AD[5:0], D[7:0]
	spiWrite(lsm330_spiRead_strobe | (reg & 0x3F));
	read = spiRead();
	
	PORTF.OUTSET = 0x08;		//SSA end transmission
	
	return read;
	
}


void accWrite(uint8_t reg, uint8_t data) {
	
	PORTF.OUTCLR = 0x08;		//SSA clear
	PORTF.OUTSET = 0x04;		//Sensor sel acc
	
	//RW, MS, AD[5:0], D[7:0]
	spiWrite(lsm330_spiWrite_strobe | (reg & 0x3F));
	spiWrite(data);
	
	PORTF.OUTSET = 0x08;		//SSA end transmission
	

}

uint8_t gyroRead(uint8_t reg) {
	
	volatile uint8_t read;
	
	PORTF.OUTCLR = 0x10;		//SSG
	PORTF.OUTCLR = 0x04;		//Sensor sel gyro
	
	
	//RW, MS, AD[5:0], D[7:0]
	spiWrite(lsm330_spiRead_strobe | (reg & 0x3F));
	read = spiRead();
	
	PORTF.OUTSET = 0x10;		//SSG end transmission
	
	return read;

}

void gyroWrite(uint8_t reg, uint8_t data) {
	
	PORTF.OUTCLR = 0x10;		//SSG
	PORTF.OUTCLR = 0x04;		//Sensor sel gyro
	
	//RW, MS, AD[5:0], D[7:0]
	spiWrite(lsm330_spiWrite_strobe | (reg & 0x3F));
	spiWrite(data);
	
	PORTF.OUTSET = 0x10;		//SSG end transmission

}

void accel_init() {
	
	/*
	Enable falling edge interrupt on PORTC pin 7
	Use CTRL_REG4_A to route DRDY signal to INT1_A and enable INT1 with rising edge
	Use CTRL_REG5_A to configure highest output rate and enable XYZ
	*/
	
	PORTC.INTCTRL = 0x01;		//interrupt 0 low level
	PORTC.INT0MASK = 0x80;		//Pin 7 interrupt
	PORTC.PIN7CTRL = 0x02;		//sense falling edge
	
	
	accWrite(CTRL_REG4_A, (CTRL_REG4_A_DR_EN |		//route DRDY to INT1_A and enable INT1 rising edge
							CTRL_REG4_A_IEA |		
							CTRL_REG4_A_INT1_EN));	
													
			
	accWrite(CTRL_REG5_A, (CTRL_REG5_A_ODR3 |		//configure 1600Hz output rate and enable XYZ
							CTRL_REG5_A_ODR0 |
							CTRL_REG5_A_ZEN |
							CTRL_REG5_A_YEN |
							CTRL_REG5_A_XEN));		

}

void gyro_init() {
	
	/*
	Set GYRO_ENABLE bit on P0RTA3 and enable rising edge interrupt for INT2G bit PA1
	Use CTRL_REG1_G to configure highest output rate and enable XYZ
	Leave BW[1:0] bit 00 and set PD bit for normal mode
	Set I2_DRDY bit in CTRL_REG3_G (signals when gyro is ready using INT2_G pin PA1)
	Choose 2000 dps for full scale selection FS[1:0] in CTRL_REG4_G
	*/
	
	PORTA.OUT = 0x08;				//gyro enable
	PORTA.INTCTRL = 0x01;			//interrupt 0 low level
	PORTA.INT0MASK = 0x02;			//Pin 1 interrupt
	PORTA.PIN7CTRL = 0x01;			//sense rising edge

	
	gyroWrite(CTRL_REG1_G, (CTRL_REG1_G_DR1 |		//highest 100Hz, enable XYZ, normal mode
							CTRL_REG1_G_DR0 |
							CTRL_REG1_G_PD	|
							CTRL_REG1_G_ZEN |
							CTRL_REG1_G_YEN |
							CTRL_REG1_G_XEN));	
													
	
	gyroWrite(CTRL_REG3_G, CTRL_REG3_G_I2_DRDY);	//set I2_DRDY
	
	
	gyroWrite(CTRL_REG4_G, (CTRL_REG4_G_FS1 |
							CTRL_REG4_G_FS0));		//2000 dps
	
}


void send_char(char c)
{
	
	while( !(USARTD0.STATUS & USART_DREIF_bm) ); //Wait until DATA buffer is empty
	USARTD0.DATA = c;
	
}


ISR(PORTC_INT0_vect){
	accelDataReady = 0x01;
	PORTC.INTFLAGS |= 0x01;
}

ISR(PORTA_INT0_vect) {	
	gyroDataReady = 0x01;	
	PORTA.INTFLAGS |= 0x01;
}


int main(void)
{
	PMIC.CTRL = 0x01;			//low level interrupts	
	CLK_init();
	USART_init();
    SPI_init();
	
	gyroWrite(CTRL_REG1_G, 0x00);	//reset gyro
	accWrite(CTRL_REG4_A, 0x01);	//reset accelerometer
	
	gyro_init();
	accel_init();
	 
	sei();
	
	volatile uint8_t ACCEL_X_H, ACCEL_X_L, ACCEL_Y_H, ACCEL_Y_L, ACCEL_Z_H, ACCEL_Z_L;
	volatile uint8_t GYRO_X_H, GYRO_X_L, GYRO_Y_H, GYRO_Y_L, GYRO_Z_H, GYRO_Z_L;
	
    while(1){	
		
		if(gyroDataReady == 0x01){
			GYRO_X_H = gyroRead(OUT_X_H_G);
			GYRO_X_L = gyroRead(OUT_X_L_G);
			GYRO_Y_H = gyroRead(OUT_Y_H_G);
			GYRO_Y_L = gyroRead(OUT_Y_L_G);
			GYRO_Z_H = gyroRead(OUT_Z_H_G);
			GYRO_Z_L = gyroRead(OUT_Z_L_G);
			
			gyroDataReady = 0x00;
		}
		
		else if(accelDataReady == 0x01){
			ACCEL_X_H = accRead(OUT_X_H_A);
			ACCEL_X_L = accRead(OUT_X_L_A);
			ACCEL_Y_H = accRead(OUT_Y_H_A);
			ACCEL_Y_L = accRead(OUT_Y_L_A);
			ACCEL_Z_H = accRead(OUT_Z_H_A);
			ACCEL_Z_L = accRead(OUT_Z_L_A);
			
			accelDataReady = 0x00;
			
		}
		
		
		send_char(0x03);
		send_char(ACCEL_X_L);
		send_char(ACCEL_X_H);
		send_char(ACCEL_Y_L);
		send_char(ACCEL_Y_H);
		send_char(ACCEL_Z_L);
		send_char(ACCEL_Z_H);
		send_char(GYRO_X_L);
		send_char(GYRO_X_H);
		send_char(GYRO_Y_L);
		send_char(GYRO_Y_H);
		send_char(GYRO_Z_L);
		send_char(GYRO_Z_H);
		send_char(0xFC);
	
	}
	
}