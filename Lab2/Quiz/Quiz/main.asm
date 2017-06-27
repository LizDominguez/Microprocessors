/*
*  Lab 2 Part C
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Quiz
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ RED = ~(0x10)
.equ GREEN = ~(0x20)
.equ BLUE = ~(0x40)
.equ BLACK = ~(0x00)
.equ RED_20 = 0x20
.equ RED_80 = 0x80
.equ RED_FF = 0xFF
.equ GREEN_FF = 0xFF
.equ RED_GREEN_FF = 0xFF


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

	ldi r16, 0xF0					;setting TOP value to 15
	sts TCC0_PER, r16 				;PER Low
	ldi r16, 0x00
	sts TCC0_PER+1, r16				;PER High
	ldi r16, 0x04 					;Clk/8 prescaler
	sts TCC0_CTRLA, r16 			;Timer 0 in Port C enable

	ldi r16, 0x70 					;Pins 4-6
	sts PORTD_DIRSET, r16 			;PORTD RBG LEDs as outputs

	ldi r16, BLACK
	sts PORTD_OUT, r16

	ldi r16, 0x00					;setting up switches as inputs
	sts PORTA_DIRSET, r16

	ldi r16, 0x00					;setting up button as input
	sts PORTF_DIRSET, r16			;S1 PF2

	
OUTPUT:							;Here we check which LEDs will be on
	ldi r20, BLACK				;RED
	ldi r17, GREEN				;GREEN
	ldi r18, BLACK				;BLUE
	AND r20, r17				;r16 or r17 -> r16
	AND r20, r18				;AND colors to get final output
					 			;Output colors to RBG LED
																
ON:
	lds r16, PORTF_IN			;button
	SBRS r16, 2
	lds r19, PORTA_IN			;switches
	cpi r19, 0x00
	BREQ OFF 
	sts PORTD_OUT, r20
	lds r17, TCC0_CNT			;checking counter 
	cp r17, r19					;compare time to counter
	BRLT ON						;if not equal to time or less, keep LED ON


OFF:
	ldi r16, BLACK 				;Turn off RBG LEDs
	sts PORTD_OUT, r16 			
	ldi r16, 0xF0				;load 15 into register		
	lds r17, TCC0_CNT			;check if counter has reached 15
	cp r16, r17
	BRLT OFF					;if counter has not reached 15, keep off
	rjmp OUTPUT					;else, go back to main


READY:
	push r16					;pushing registers into stack
	push r17
	ldi r16, 0x03				;Bit 1 is 32 MHz oscillator ready flag
	lds r17, OSC_STATUS			;load oscillator status 
	cp r17, r16 				;check if status flag is set
	BRNE READY 					;keep waiting if flag isn't set	
	pop r17
	pop r16						;popping registers from the stack
	ret 						;return to main function once ready











