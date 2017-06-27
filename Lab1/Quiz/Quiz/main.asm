;
; Quiz.asm
;
; Created: 5/24/17 5:03:00 PM
; Author : Liz
;


.nolist	
.include "ATxmega128A1Udef.inc"
.list 


.org 0x0000						;places code at address 0x0000
	rjmp MAIN					;jump to start of program


.org 0x200						;where program begins

MAIN:
	ldi r16, 0xFF 				;PC0 - PC7 
	sts PORTC_DIRSET, r16 		;sets LEDs as outputs
	ldi r16, 0x00				;F0 - F7
	sts PORTF_DIRSET, r16 		;sets S1 and S2 as inputs
	ldi r18, 0x00 				;initializes counters
	ldi r19, 0x00

RESET:
	ldi r17, 0xFE			;PC0
	sts PORTC_OUT, r17		;sets up beggining of game

SERVE:
	lds r16, PORTF_IN		;loads input values from switches
	SBRS r16, 2				;checks if S1 is pressed
	rjmp S2					;jumps to S2
	rjmp SERVE				;infinite loop

S2:
	com r17					;complement bits so that we can shift
	lsl r17					;left shifts LED bits
	com r17
	sts PORTC_OUT, r17		;displays the new LED
	rcall DELAY				;delays
	lds r16, PORTF_IN		;reads button
	SBRS r16, 3				;checks if S2 is pressed
	rjmp WIN2				;jump to WIN2 if S2 is pressed
	rjmp S2

WIN2:
	cpi r17, 0x7F			;check if LED7 is on
	BRNE RESET				;if not, reset

S1:
	com r17					;complement bits so that we can shift
	lsr r17					;shifts bits right	
	com r17					
	sts PORTC_OUT, r17		;displays the new LED		
	rcall DELAY				;delays
	lds r16, PORTF_IN		;reads buttons
	SBRS r16, 2				;checks if S1 is pressed
	rjmp WIN1				;jump to WIN1 if S1 is pressed
	rjmp S1

WIN1:
	cpi r17, 0xFE			;check if LED0 is set
	BRNE RESET				;branch to S2 if it is
	rjmp S2					;otherwise it resets


DELAY:
	nop						;0.5us
	nop
	inc r18					;incrementing counter1
	cpi r18, 0xFF 			;compare counter1 to 250
	brne DELAY 				;loop back if counter1 is not full
	inc r19 				;increment the second counter
	cpi r19, 0xFF 			;compare counter2 to 250
	brne DELAY 		;loop back if not full
	ldi r18, 0x00 			;reset counters
	ldi r19, 0x00
	ret


DONE:
	rjmp DONE	;infinite loop