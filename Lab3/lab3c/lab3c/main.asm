/*
*  Lab 3 Part 3
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Debouncing a tacticle switch
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ RED = ~(0x10)
.equ GREEN = ~(0x20)
.equ BLUE = ~(0x40)
.equ BLACK = ~(0x00)

.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program	

.org PORTF_INT0_vect
	jmp EXT_ISR

.org TCC0_OVF_vect		;timer overflow vector
	rjmp TCC0_ISR

.org 0x200				;where program begins

MAIN:
	rcall IO_INIT
	rcall PORTF_INT0_INIT			;init ext interrupt
	rcall TIMER_INIT				;initialize timer
	sei
																

TOGGLE:
	ldi r20, BLUE
	sts PORTD_OUT, r20
	sts PORTD_OUT, r20			;Output colors to RBG LED
	ldi r16, BLACK
	sts PORTD_OUT, r16
	rjmp TOGGLE


IO_INIT:
	ldi r16, 0x70 					;Pins 4-6
	sts PORTD_DIRSET, r16 			;PORTD RBG LEDs as outputs

	ldi r16, 0xFF
	sts PORTC_DIRSET, r16			;setting LEDs as outputs

	ldi r16, 0xFF
	sts PORTC_OUT, r16

	ldi r16, 0x00
	sts PORTF_DIRSET, r16			;setting button as input
	ret


PORTF_INT0_INIT:
	push r16					;pushing r16 into stack

	ldi r16, 0x01
	sts PORTF_INTCTRL, r16		;interrupt 0 low level

	ldi r16, 0x04				;Pin 2 (S1)
	sts PORTF_INT0MASK, r16		;source of interrupt

	ldi r16, 0x00				;data direction of interrupt source
	sts PORTF_DIRSET, r16		;S1 as input

	ldi r16, 0x02				;sense configuration
	sts PORTF_PIN2CTRL, r16		;sense falling edge

	ldi r16, 0x01
	sts PMIC_CTRL, r16			;low level interrupts

	pop r16						;popping r16 from stack
	ret


TIMER_INIT:
	push r16

	ldi r16, 0x02					;bit 1 
	sts OSC_CTRL, r16				;32 Mhz internal oscillator enable

	rcall READY

	ldi r16, 0xD8 					;IOREG 
	sts CPU_CCP, r16 				;sets configuration charge protection

	ldi r16, 0x01 					;bit 0 is 32 MHz internal oscillator
	sts CLK_CTRL, r16 				;select system clock

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
	ret 						;return to main function once ready


EXT_ISR:
	push r16				
	lds	r16, CPU_SREG		
	push r16

	ldi r16, 0xFF					;setting TOP value to 15
	sts TCC0_PER, r16 				;PER Low
	ldi r16, 0x10
	sts TCC0_PER+1, r16				;PER High

	ldi r16, 0x07 					;Clk/1024 prescaler
	sts TCC0_CTRLA, r16 			;Timer 0 in Port C enable

	ldi r16, 0x00 					;Normal mode
	sts TCC0_CTRLB, r16 			

	ldi r16, 0x01 					;low level overflow interrupt
	sts TCC0_INTCTRLA, r16 			;interrupt enable

	ldi r16, 0x01
	sts PMIC_CTRL, r16			;low level interrupts

	ldi	r16, 0x01					;renabling timer interrupts
	sts	TCC0_INTFLAGS, r16			;cleaing timer flag

	pop	r16					
	sts	CPU_SREG, R16		
	pop r16
	reti


TCC0_ISR:	
	push r16				
	lds	r16, CPU_SREG		
	push r16				
	
	lds r16, PORTF_IN				;load button input
	SBRS r16, 2						;do not increment if PF2 is high
	inc r19							;store number of interrupts
	com r19							;not count for active low LEDs
	sts PORTC_OUT, r19				;set LED as output
	com r19							;recomplement count

	ldi r16, 0x01					;enable ext interrupt
	sts PORTF_INTFLAGS, r16			;clear PORTF int0 flag

	ldi r16, 0x00 					;Clk/1024 prescaler
	sts TCC0_CTRLA, r16 			;Timer 0 in Port C enable	

	pop	r16					
	sts	CPU_SREG, R16		
	pop r16
	reti





