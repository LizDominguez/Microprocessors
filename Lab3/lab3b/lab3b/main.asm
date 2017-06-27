/*
*  Lab 3 Part 2
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Counting the number of external interrupts and displaying it on LEDs
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ BLUE = ~(0x40)
.equ BLACK = ~(0x00)
.equ ZERO = ~(0x00)

.org 0x0000							;places code at address 0x0000
	rjmp MAIN						;jump to start of program

.org PORTF_INT0_vect
	jmp ISR_LED_COUNT

.org 0x200							;where program begins

MAIN:

	ldi r16, 0x70 					;Pins 4-6
	sts PORTD_DIRSET, r16 			;PORTD RBG LEDs as outputs

	rcall PORTF_INT0_init			;initializing interrupt

	ldi r16, 0xFF
	sts PORTC_DIRSET, r16			;setting LEDs as outputs
	sts PORTC_OUT, r16

	clr r19							;clear the register we use to count interrupts
	
COLOR:							;Here we check which LEDs will be on
	ldi r20, BLACK				;RED
	ldi r17, BLACK				;GREEN
	ldi r18, BLUE				;BLUE
	AND r20, r17				;r16 or r17 -> r16
	AND r20, r18				;AND colors to get final output

TOGGLE:
	sts PORTD_OUT, r20			;Output colors to RBG LED
	ldi r16, BLACK
	sts PORTD_OUT, r16
	rjmp TOGGLE

PORTF_INT0_init:
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

	sei
	ret

ISR_LED_COUNT:
	push r16

	inc r19							;store number of interrupts
	com r19							;not count for active low LEDs
	sts PORTC_OUT, r19				;set LED as output
	com r19							;recomplement count

	ldi r16, 0x01
	sts PORTF_INTFLAGS, r16			;clear PORTF int0 flag

	pop r16
	reti









