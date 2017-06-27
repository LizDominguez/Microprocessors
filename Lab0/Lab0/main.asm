/*
*  Lab 0
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Saves values less than 0x75 and if they are greater than 0x37, 0x11 is added
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list 

.org 0x0000						;places code at address 0x0000
	rjmp MAIN					;jump to start of program

.org 0xF000							;Table 1 is at address 0x7800.  This will appear
									;to be at 0x1E000 in program memory

TAB1: .db 0x3D, 0x7E, 84, 102, 0x7B, 0172, 0x20, 0x64, 0x7E, 0x3F, 060, 0x33, 0x7B, 121, 118, 0x21, 0x78, 0x77, 0x00 

.dseg				;data segment only allows variables defined by bytes
.org 0x3744			;location of output table in data memory

TAB2: .byte 20	;reserving space for 20 characters

.cseg			;redeclaration of code segment
.org 0x200		;where program begins

MAIN:

	ldi ZL, byte3(TAB1<<1) ; Initialize Z pointer
	out CPU_RAMPZ, ZL
	ldi ZH, byte2(TAB1<<1)
	ldi ZL, byte1(TAB1<<1)


	ldi YL, low(TAB2)		
	ldi YH, high(TAB2)	

LOOP:

	elpm r16, Z+		;load value from TAB1 using Z pointer and increment
	cpi r16, 0x00	;checking if we have reached NUL
	breq NUL		;branch to NUL if 0
	cpi r16, 0x76	;compares data to 0x76
	brge LOOP		;returns to LOOP if data is greater than 0x75
	cpi r16, 0x26	;compares data to 0x38
	brlt STORE		;Branches if less than 0x38
	ldi r17, 0x11	;loads 0x11 
	Add r16, r17	;adds 0x11 to the value that's greater than 0x37
	st Y+, r16		;stores indirect and post increment
	rjmp LOOP		;goes back to the beggining

STORE:	
	st Y+, r16
	rjmp LOOP

NUL:
	st Y+, r16		;copies last value

DONE:
	rjmp DONE	;infinite loop