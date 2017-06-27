/*
*  Lab 2 Part A
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Sets the clock to 8MHz
*/

.nolist	; This works, but the below file can't be removed for lss file.
.include "ATxmega128A1Udef.inc"
.list

.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program

.org 0x200				;where program begins

MAIN:
	ldi r16, 0x02					;bit 1 
	sts OSC_CTRL, r16				;32 Mhz internal oscillator enable
	call READY
	ldi r16, 0xD8 					;IOREG 
	sts CPU_CCP, r16 				;sets configuration charge protection
	ldi r16, 0x01 					;bit 0 is 32 MHz internal oscillator
	sts CLK_CTRL, r16 				;select system clock
	/*ldi r16, 0xD8 					;IOREG 
	sts CPU_CCP, r16 				;sets configuration charge protection
	ldi r16, 0b00001100 			;PSADIV prescaler division factor
	sts CLK_PSCTRL, r16 			;divide clock by 4 32/4 = 8 MHz */
	ldi r16, 0x80 					;PC7 
	sts PORTC_DIRSET, r16 			;sets clock output on PORTC
	ldi r16, 0x01					;clock output on PORTC
	sts PORTCFG_CLKEVOUT, r16 		;default output pin is 7

DONE:
	rjmp DONE					;infinite loop

READY:
	ldi r16, 0x03				;Bit 1 is 32 MHz oscillator ready flag
	lds r17, OSC_STATUS			;load oscillator status 
	cp r17, r16 				;check if status flag is set
	BRNE READY 					;keep waiting if flag isn't set		
	ret 						;return to main function once ready


