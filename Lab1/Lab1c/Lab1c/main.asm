/*
*  Lab 1 Part C
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: toggles LED at 50 Hz
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list 

.org 0x0000						;places code at address 0x0000
	rjmp MAIN					;jump to start of program

.org 0x200						;where program begins

MAIN:
	ldi r16, 0x01 				;PC0 
	sts PORTC_DIRSET, r16 		;sets first LED as output
	ldi r18, 0x00 				;initializes counters
	ldi r19, 0x00

BLINK:
	ldi r16, 0b11111110			
	sts PORTC_OUT, r16 			;turns on low LED0
	rcall DELAY_10ms 			;delays 10 ms
	ldi r16, 0xFF
	sts PORTC_OUT, r16 			;turns off low LED
	rcall DELAY_10ms 			;delays 10 ms
	rjmp BLINK 					;loops infinitely at 50Hz

DELAY_10ms:
	nop						;0.5us
	nop
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