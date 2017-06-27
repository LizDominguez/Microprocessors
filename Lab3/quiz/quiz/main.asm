/*
*  Lab 3 Quiz
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Quiz
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ BLACK = ~(0x00)
.equ WHITE = ~(0xFF)

.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program	

.org PORTA_INT0_vect
	jmp EXT_ISR

.org TCC0_OVF_vect		;timer overflow vector
	rjmp TCC0_ISR

.org 0x200				;where program begins

MAIN:
	rcall IO_INIT
	rcall PORTA_INT0_INIT		;init ext interrupt
	rcall CLK_INIT				;initialize timer
	rcall PWM_INIT
	ldi r19, 0x01
	sei													
	
ON:	
	rcall COLOR
	rjmp ON


IO_INIT:
	ldi r16, 0x70 					;Pins 4-6
	sts PORTD_DIRSET, r16 			;PORTD RBG LEDs as outputs

	ldi r16, BLACK
	sts PORTD_OUT, r16

	ldi r16, 0xFF
	sts PORTC_DIRSET, r16			;setting LEDs as outputs

	ldi r16, BLACK					;turning LEDs off
	sts PORTC_OUT, r16

	ldi r16, 0x00
	sts PORTA_DIRSET, r16			;setting switches as input
	ret


PORTA_INT0_INIT:
	push r16					;pushing r16 into stack

	ldi r16, 0x01
	sts PORTA_INTCTRL, r16		;interrupt 0 low level

	ldi r16, 0x01				;Pin 0 
	sts PORTA_INT0MASK, r16		;source of interrupt

	ldi r16, 0x01				;sense configuration
	sts PORTA_PIN0CTRL, r16		;sense rising edge

	ldi r16, 0x01
	sts PMIC_CTRL, r16			;low level interrupts

	pop r16						;popping r16 from stack
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

TIMER_INIT:
	push r16

	ldi r16, 0x0F					;setting TOP value to 15
	sts TCC0_PER, r16 				;PER Low
	ldi r16, 0xAF
	sts TCC0_PER+1, r16				;PER High

	ldi r16, 0x07 					;Clk/1024 prescaler
	sts TCC0_CTRLA, r16 			;Timer 0 in Port C enable

	ldi r16, 0x00 					;Normal mode
	sts TCC0_CTRLB, r16 			

	pop r16
	ret

TIMER_INT_INIT:
	push r16
	ldi r16, 0x01 					;low level overflow interrupt
	sts TCC0_INTCTRLA, r16 			;interrupt enable

	ldi r16, 0x01
	sts PMIC_CTRL, r16			;low level interrupts
	pop r16
	ret

PWM_INIT:
	push r16

	ldi r16, 0xFF					;setting the period
	sts TCD0_PER, r16 				;highest resolution
	ldi r16, 0xFF
	sts TCD0_PER+1, r16				;PER High

	ldi r16, 0x01					;Clk
	sts TCD0_CTRLA, r16 			;Timer 0 in Port D enable

	ldi r16, 0x73 					;enable OC0C, 0x73 for all parts
	sts TCD0_CTRLB, r16 			;single slope PWM	

	ldi r16, 0x07					;moving PORTD timer/counter pins
	sts PORTD_REMAP, r16

	pop r16
	ret


COLOR:

	/**RED PWM **/
	ldi r16, ~0x8A 					;Duty cycle	 		
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, ~0x8A					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0x2C				
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0x00					
	sts TCD0_CCB+1, r16 
				
	/**BLUE PWM **/
	ldi r16, ~0x9A				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0x9A					
	sts TCD0_CCC+1, r16 
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
	ret 
							;return to main function once ready

EXT_ISR:
	push r16				
	lds	r16, CPU_SREG		
	push r16

	rcall TIMER_INIT
	rcall TIMER_INT_INIT

	ldi	r16, 0x01					;renabling timer interrupts
	sts	TCC0_INTFLAGS, r16			;cleaing timer flag

	pop	r16					
	sts	CPU_SREG, R16		
	pop r16
	reti

RESET:

rjmp MAIN


TCC0_ISR:	
	push r16				
	lds	r16, CPU_SREG		
	push r16	 	
	
	cpi r19, 0xFF
	BREQ RESET

	lds r16, PORTA_IN				;load switch input
	SBRS r16, 0						;do not shift if PA0 is low		
	lsl r19							;left shift

	com r19							;not count for active low LEDs
	sts PORTC_OUT, r19				;set LED as output
	com r19							;recomplement count		

	ldi r16, 0x01					;enable ext interrupt
	sts PORTA_INTFLAGS, r16			;clear PORTA int0 flag	
		
	pop	r16					
	sts	CPU_SREG, R16		
	pop r16
	reti





