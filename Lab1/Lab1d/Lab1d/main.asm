/*
*  Lab 1 Part D
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: KITT LED pattern at 50ms rate, blinking at 50Hz
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list 


.org 0x0000						;places code at address 0x0000
	rjmp MAIN					;jump to start of program


.org 0x100							;0x4000 in window

Pattern: .db 0xFE, 0xFC, 0xF9, 0xF3, 0xEF, 0xCF, 0x9F, 0x3F, 0x80, 0x00


.org 0x200						;where program begins

MAIN:

	ldi ZL, low(Pattern << 1)
	ldi ZH, high(Pattern << 1)

	ldi r16, 0xFF 				;PC0 - PC7 
	sts PORTC_DIRSET, r16 		;sets LEDs as outputs
	ldi r18, 0x00 				;initializes counters
	ldi r19, 0x00
	ldi r20, 0x00
	ldi r17, 0x00

KITT:
	lpm r16, Z+ 				;load pattern in increments Z pointer
	cpi r16, 0x00 				;compare register to last pattern
	BREQ MAIN 					;return to main function if equal
	ldi r17, 0x00

BLINK:
	sts PORTC_OUT, r16 			;turns on LEDs
	rcall DELAY_10ms 			;delays 10 ms
	inc r17 					;keeping track of each 10ms
	cpi r17, 0x05 				;check if 50ms has passed
	BREQ KITT 					;go to next pattern if equal
	ldi r20, 0xFF 				;loads FF into register
	sts PORTC_OUT, r20 			;outputs to LOW LED
	rcall DELAY_10ms 			;delays 10 ms
	inc r17
	cpi r17, 0x04 				;check if 50ms has passed
	BREQ KITT 					;go to next pattern if equal
	rjmp BLINK 					;loops infinitely at 50Hz

DELAY_10ms:
	nop						;0.5us
	inc r18					;incrementing counter1
	cpi r18, 0xFF 			;compare counter1 to 250
	brne DELAY_10ms 		;loop back if counter1 is not full
	inc r19 				;increment the second counter
	cpi r19, 0x0D 			;compare counter2 to 250
	brne DELAY_10ms 		;loop back if not full
	ldi r18, 0x00 			;reset counters
	ldi r19, 0x00
	ret


DONE:
	rjmp DONE	;infinite loop