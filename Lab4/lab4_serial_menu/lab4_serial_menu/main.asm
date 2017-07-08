/*
*  Lab 4 Part 4
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: USART interactive menu
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ BSel = 135		;57.6k Baud 
.equ BScale = -2

.equ CR = 0x0D
.equ LF = 0x0A
.equ D = 0x44


.org 0x1000
MENU: .db "Liz's favorite:", CR, LF, "1. Food", CR, LF, "2. Quote", CR, LF, "3. Movie", CR, LF, "4. UF Course", CR, LF, "5. Hobby", CR, LF, "6. Re-display menu", CR, LF, "D: Done", CR, LF, 0x00
OP1: .db "Liz's favorite food is anything Cuban", CR, LF, CR, LF, 0x00
OP2: .db "Liz's favorite quote is by John Muir", CR, LF, CR, LF, 0x00
OP3: .db "Liz's favorite movie is Howl's Moving Castle", CR, LF, CR, LF, 0x00
OP4: .db "Liz doesn't like any UF courses", CR, LF, CR, LF, 0x00
OP5: .db "Liz's favorite hobby is front-end development", CR, LF, CR, LF, 0x00
OP6: .db "Done!", CR, LF, CR, LF, 0x00


.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program	


.org 0x200				;where program begins

MAIN:

	ldi YL, low(0x3FFF)			;initializing the stack
	out CPU_SPL, YL				
	ldi YL, high(0x3FFF)
	out CPU_SPH, YL	

	rcall CLK_INIT				;initialize clock to 32MHz
	rcall USART_INIT			;initializing USARTD0
	rcall IO_INIT


READ_MENU:
	ldi ZL, low(MENU << 1)
	ldi ZH, high(MENU << 1) 
	rcall OUT_STRING


GET_CHAR:

	rcall IN_CHAR		;scans the number
	cpi r16, D			;Done
	breq EXIT

	cpi r16, 0x31		;1. Food
	breq FOOD

	cpi r16, 0x32		;2. Quote
	breq QUOTE

	cpi r16, 0x33		;3. Movie
	breq MOVIE

	cpi r16, 0x34		;4. UF Course
	breq UF_COURSE

	cpi r16, 0x35		;5. Hobby
	breq HOBBY

	cpi r16, 0x36		;6. Menu
	breq READ_MENU

	rjmp GET_CHAR


EXIT:
	ldi ZL, low(OP6 << 1)		;makes Z pointer point to menu
	ldi ZH, high(OP6 << 1)
	call OUT_STRING				;outputs the menu


DONE:
	rjmp DONE


FOOD:
	ldi ZL, low(OP1 << 1)
	ldi ZH, high(OP1 << 1)
	rcall OUT_STRING
	rjmp READ_MENU


QUOTE:
	ldi ZL, low(OP2 << 1)
	ldi ZH, high(OP2 << 1)
	rcall OUT_STRING
	rjmp READ_MENU


MOVIE:
	ldi ZL, low(OP3 << 1)
	ldi ZH, high(OP3 << 1)
	rcall OUT_STRING
	rjmp READ_MENU


UF_COURSE:
	ldi ZL, low(OP4 << 1)
	ldi ZH, high(OP4 << 1)
	rcall OUT_STRING
	rjmp READ_MENU


HOBBY:
	ldi ZL, low(OP5 << 1)
	ldi ZH, high(OP5 << 1)
	rcall OUT_STRING
	rjmp READ_MENU


OUT_STRING:
	push r16


NEXT_CHAR:
	lpm r16, Z+				
	cpi r16, 0x00
	breq RETURN_OUT_STRING
	rcall OUT_CHAR
	rjmp NEXT_CHAR		


RETURN_OUT_STRING:
	pop r16
	ret


IN_CHAR:
	push r17


RX_POLL:
	lds r16, USARTD0_STATUS		;loads status register
	sbrs r16, 7					;reads a char if flag set	
						
	rjmp RX_POLL				;continues polling
	lds r16, USARTD0_DATA		;reads char into r16
	pop r17
	ret

OUT_CHAR:
	push r17


TX_POLL:
	lds r17, USARTD0_STATUS		;loads status register
	sbrs r17, 5					;writes out char if DREIF flag set							
	rjmp TX_POLL				
	sts USARTD0_DATA, r16		;sends char to USART
	pop r17
	ret


USART_INIT:
	ldi r16, 0x18				;RXEN/TEXEN
	sts USARTD0_CTRLB, r16		

	ldi r16, 0x3B
	sts USARTD0_CTRLC, r16		;asynchronous/odd parity/1 stop bit/8 data

	ldi r16, (BSel & 0xFF)		
	sts USARTD0_BAUDCTRLA, r16	;sets BAUDCTRLA to lower 8 bits of BSel 

	ldi r16, ((BScale << 4) & 0xF0) | ((BSel >> 8) & 0x0F)							
	sts USARTD0_BAUDCTRLB, r16	;sets BAUDCTRLB to BScale | BSel 								
	ret

IO_INIT:
	ldi r16, 0x08	
	sts PortD_DIRSET, r16	;set TxD as output	
					
	sts PortD_OUTSET, r16	;set TxD to high
					
	ldi r16, 0x04
	sts PortD_DIRCLR, r16	;set RxD pin as input

	ldi r16, 0x0A			;PortQ bits 1 and 3 enable and select
	sts PORTQ_DIRSET, r16	;PortD bits 2 and 3 serial pins 
	sts PORTQ_OUTCLR, r16   ;connected to the USB lines
	ret

CLK_INIT:
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

