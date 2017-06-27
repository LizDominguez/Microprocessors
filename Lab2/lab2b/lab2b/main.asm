/*
*  Lab 2 Part B
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Counts to 255 using Port C Timer 0 
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.org 0x0000					;places code at address 0x0000
	rjmp MAIN				;jump to start of program

.org 0x200					;where program begins

MAIN:
ldi r16, 0xFF 				;Pins 0-7 as outputs
sts PORTC_DIRSET, r16 		;PORTC as output
ldi r16, 0xFF				;setting TOP value to 255
sts TCC0_PER, r16 			;PER Low
ldi r16, 0x00
sts TCC0_PER+1, r16			;PER High
ldi r16, 0x07 				;Clk/1024 prescaler
sts TCC0_CTRLA, r16 		;Timer 0 in Port C enable

COUNT:
lds r16, TCC0_CNT 			;loading count value into register
sts PORTC_OUT, r16 			;outputing count value to PortC
rjmp COUNT					;infinite loop






