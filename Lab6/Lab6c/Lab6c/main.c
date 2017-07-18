/*
	Lab 6 Part C
	Name: Elizabeth Dominguez
	Section: 7F34
	TA Name: Wesley Piard
	Description: Setting up the SPI and LSM330 read and write functions
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "LSM330.h"

volatile uint8_t accelDataReady = 0;
volatile uint8_t gyroDataReady = 0;

void CLK_init(void) {
	
	OSC.CTRL = 0x02;							//32 Mhz internal oscillator enable
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		//wait for 32 MHz oscillator ready flag
	CPU_CCP = 0xD8; 							//IOREG
	CLK.CTRL = 0x01; 							//bit 0 is 32 MHz internal oscillator
	
}

void SPI_init()
{
	PORTA.DIR = 0xFF;		//Protocol Sel, others
	PORTF.DIR = 0xFF;		//SSA, MOSI, SCK, others
	PORTF.OUT = 0x18;		//SSA & SSG
	
	
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
	//uint8_t read = 0x80;
	
}

void accWrite(uint8_t reg, uint8_t data) {
	
	uint8_t accData = 0;
	
	PORTA.OUT = 0x00;		//Protocol sel
	PORTF.OUT = 0x04;		//SSA, Sensor sel acc
	
	//RW, MS, AD[5:0], D[7:0]
	accData = spiWrite(reg);
	accData = spiWrite(data);
	
	PORTF.OUT = 0x08;		//SSA end transmission, disable SCK
	

}

void gyroWrite(uint8_t reg, uint8_t data) {
	
	uint8_t gyroData = 0;
	
	PORTA.OUT = 0x00;		//Protocol sel SPI
	PORTF.OUT = 0x00;		//SSG, sensor select gyro
	
	//RW, MS, AD[5:0], D[7:0]
	gyroData = spiWrite(reg);
	gyroData = spiWrite(data);

	PORTF.OUT = 0x10;		//SSG end transmission, disable SCK

}

void accel_init() {
	
	/*
	Enable falling edge interrupt on PORTC pin 7
	Use CTRL_REG4_A to route DRDY signal to INT1_A and enable INT1 with rising edge
	Use CTRL_REG5_A to configure highest output rate and enable XYZ
	*/
		
	PORTC.INTCTRL = 0x01;		//interrupt 0 low level
	PORTC.INT0MASK = 0x80;		//Pin 7 interrupt
	PORTC.DIRSET = 0x00;		//data direction
	PORTC.PIN7CTRL = 0x02;		//sense falling edge

	PMIC.CTRL = 0x01;			//low level interrupts

	accWrite(CTRL_REG4_A, CTRL_REG4_A_STRT);		//reset accelerometer
	
	accWrite(CTRL_REG4_A, (CTRL_REG4_A_DR_EN |		//route DRDY to INT1_A and enable INT1 rising edge
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
	PORTA.DIRSET &= ~0x02;			//data direction
	PORTA.PIN7CTRL = 0x01;			//sense rising edge
	
	gyroWrite(CTRL_REG1_G, 0x01);			//reset gyroscope
	
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

ISR(PORTC_INT0_vect){
	accelDataReady = 0x01;
	gyroDataReady = 0x00;
}

ISR(PORTA_INT0_vect) {
	
	gyroDataReady = 0x01;
	accelDataReady = 0x00;
	
}

int main(void)
{
	CLK_init();
    SPI_init();	 
	accel_init();
	gyro_init();
	
	volatile uint16_t ACCEL_X_H, ACCEL_X_L, ACCEL_Y_H, ACCEL_Y_L, ACCEL_Z_H, ACCEL_Z_L;
	volatile uint16_t GYRO_X_H, GYRO_X_L, GYRO_Y_H, GYRO_Y_L, GYRO_Z_H, GYRO_Z_L;
	
    while(1){
		
		if(accelDataReady == 0x01){
			ACCEL_X_H = OUT_X_H_A;
			ACCEL_X_L = OUT_X_L_A;
			ACCEL_Y_H = OUT_Y_H_A;
			ACCEL_Y_L = OUT_Y_L_A;
			ACCEL_Z_H = OUT_Z_H_A;
			ACCEL_Z_L = OUT_Z_L_A;
		}
		
		else if(gyroDataReady == 0x01){
			GYRO_X_H = OUT_X_H_G;
			GYRO_X_L = OUT_X_L_G;
			GYRO_Y_H = OUT_Y_H_G;
			GYRO_Y_L = OUT_Y_L_G;
			GYRO_Z_H = OUT_Z_H_G;
			GYRO_Z_L = OUT_Z_L_G;
		}
	
	
	}
	
}