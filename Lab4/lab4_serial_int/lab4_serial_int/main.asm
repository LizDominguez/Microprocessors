/*
*  Lab 4 Part 5
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Interrupt driven echo
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ BSel = 135		;57.6k Baud 
.equ BScale = -2


.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program	


.org USARTD0_RXC_vect		;USART interrupt
		jmp USART_ISR

.org 0x200				;where program begins

MAIN:

	ldi YL, low(0x3FFF)			;initializing the stack
	out CPU_SPL, YL				
	ldi YL, high(0x3FFF)
	out CPU_SPH, YL	

	rcall CLK_INIT				;initialize clock to 32MHz
	rcall USART_INIT			;initializing USARTD0
	rcall IO_INIT


BLINK:
	st X, r16
	rcall DELAY
	st X, r20
	rcall DELAY
	rjmp BLINK


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
	ldi r16, 0x10
	sts USARTD0_CTRLA, r16		;low level receiver interrupt

	ldi r16, 0x18				;RXEN/TEXEN
	sts USARTD0_CTRLB, r16		

	ldi r16, 0x3B
	sts USARTD0_CTRLC, r16		;asynchronous/odd parity/1 stop bit/8 data

	ldi r16, (BSel & 0xFF)		
	sts USARTD0_BAUDCTRLA, r16	;sets BAUDCTRLA to lower 8 bits of BSel 

	ldi r16, ((BScale << 4) & 0xF0) | ((BSel >> 8) & 0x0F)							
	sts USARTD0_BAUDCTRLB, r16	;sets BAUDCTRLB to BScale | BSel 		

	ldi r16, 0x01				;PMIC interrupt level
	sts PMIC_CTRL, r16
	sei							;global interrupt enable
							
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

USART_ISR:
	;Always (almost) do next 3 lines at the beginning of ISRs
	push r16				;Always do this at the beginning of ISRs
	lds	r16, CPU_SREG		;Save the status bits
	push r16				;  ...

	nop
	rcall IN_CHAR
	rcall OUT_CHAR

	pop	r16					;Restore the status bits
	sts	CPU_SREG, r16		;  ...
	pop r16
	reti


DELAY: 

	nop					;.5 us each operation
	nop
	nop
	nop
	inc r18					;increments first counter
	cpi r18, 0xFF				;checks if 255
	brne DELAY				;jumps to beginning of loop if not equal
	ldi r18, 0x00				;resets count
	nop
	nop
	nop
	nop
	inc r19					;increments second counter
	cpi r19, 0xFF				;checks if 255
	brne DELAY				;goes back to loop if not
	ldi r19, 0x00				;resets second counter
	ret