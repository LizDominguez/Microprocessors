/*
*  Lab 3 Part 1
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: PWM BLUE LED
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ RED = ~(0x10)
.equ GREEN = ~(0x20)
.equ BLUE = ~(0x40)
.equ BLACK = ~(0x00)

.equ DUTY_CYCLE = ~(0x0F)

.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program	

.org 0x200				;where program begins

MAIN:

	ldi r16, 0x70 					;Pins 4-6
	sts PORTD_DIRSET, r16 			;PORTD RBG LEDs as outputs

	rcall PWM_INIT												

ON:
	ldi r20, BLUE
	sts PORTD_OUT, r20			;Output colors to RBG LED
	rjmp ON

PWM_INIT:
	push r16

	ldi r16, 0x02					;bit 1 
	sts OSC_CTRL, r16				;32 Mhz internal oscillator enable

	rcall READY

	ldi r16, 0xD8 					;IOREG 
	sts CPU_CCP, r16 				;sets configuration charge protection

	ldi r16, 0x01 					;bit 0 is 32 MHz internal oscillator
	sts CLK_CTRL, r16 				;select system clock

	ldi r16, 0xFF					;setting the period
	sts TCD0_PER, r16 				;highest resolution
	ldi r16, 0xFF
	sts TCD0_PER+1, r16				;PER High

	ldi r16, 0x01					;Clk
	sts TCD0_CTRLA, r16 			;Timer 0 in Port D enable

	ldi r16, 0x73 					;enable OC0C, 0x73 for all parts
	sts TCD0_CTRLB, r16 			;single slope PWM	

	ldi r16, 0x07					;moving PORTD timer/counter pins
	sts PORTD_REMAP, r16

	/**RED PWM **/
	ldi r16, BLACK			
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, BLACK					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, BLACK			
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, BLACK					
	sts TCD0_CCB+1, r16 
				
	/**BLUE PWM **/
	ldi r16, DUTY_CYCLE				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, DUTY_CYCLE					
	sts TCD0_CCC+1, r16

	pop r16
	ret

READY:
	push r16					;pushing registers into stack
	push r17
	ldi r16, 0x03				;Bit 1 is 32 MHz oscillator ready flag
	lds r17, OSC_STATUS			;load oscillator status 
	cp r17, r16 				;check if status flag is set
	BRNE READY 					;keep waiting if flag isn't set	
	pop r17
	pop r16						;popping registers from the stack
	ret 
							;return to main function once ready






