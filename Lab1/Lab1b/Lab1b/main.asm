/*
*  Lab 1 Part B
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Displays the values of 8 switches on LEDs denpending on if S1 or S2 was pressed
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list 

.equ DISPLAY = 0x5000			;program memory location of subrouting
.equ RETRIEVE = 0x6000

.org 0x0000						;places code at address 0x0000
	rjmp MAIN					;jump to start of program

.dseg 
.org 0x3744
Switches: .byte 1

.cseg
.org 0x200

MAIN:
	ldi r16, 0x00 				;PA0 - PA7
	sts PORTA_DIRSET, r16 		;sets DIP switches as inputs 
	ldi r16, 0x00				;F0 - F7
	sts PORTF_DIRSET, r16 		;sets S1 and S2 as inputs
	ldi r16, 0xFF 				;PC0 - PC7
	sts PORTC_DIRSET, r16 		;sets all LEDs as outputs
	ldi YL, low(Switches)		;Initialize Y register
	ldi YH, high(Switches)

BUTTONS: 
	lds r16, PORTF_IN			;loads input values from switches
	SBRS r16, 2					;checks in S1 is pressed
	jmp CALL_RETRIEVE			;jumps to RETRIEVE if bit is set
	SBRS r16, 3					;checks if S2 is pressed
	jmp CALL_DISPLAY					;jumps to DISPLAY
	rjmp BUTTONS				;infinite loop

CALL_RETRIEVE:
	call RETRIEVE
	rjmp BUTTONS

CALL_DISPLAY:
	call DISPLAY
	rjmp BUTTONS

.org DISPLAY 
	ld r16, Y 					;loads switch value from SRAM
	sts PORTC_OUT, r16 			;outputs switch values
	ret							;return to where DISPLAY was called

.org RETRIEVE 
	push r16					;push r16 into the stack
	lds r16, PORTA_IN 			;loads switch value 
	st Y, r16					;stores into SRAM
	pop r16						;pop Y from the stack
	ret

DONE:
	rjmp DONE					;infinite loop 