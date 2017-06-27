/*
*  Lab 3 Part 4
*  Name: Elizabeth Dominguez
*  Section: 7F34
*  TA Name: Wesley Piard
*  Description: Switch triggered LED configurations
*/

.nolist	
.include "ATxmega128A1Udef.inc"
.list

.equ BLACK = ~(0x00)
.equ WHITE = ~(0xFF)

.org 0x0000				;places code at address 0x0000
	rjmp MAIN				;jump to start of program	

.org PORTF_INT0_vect
	jmp EXT_ISR

.org TCC0_OVF_vect		;timer overflow vector
	rjmp TCC0_ISR

.org 0x200				;where program begins

MAIN:
	rcall IO_INIT
	rcall CLK_INIT				;initialize timer
	rcall PWM_INIT
	rcall PORTF_INT0_INIT		;init ext interrupt
	clr r19	
	sei													

DETERMINE_RBG_SUB:
	cpi r20, 0x00
	BREQ OFF

	cpi r20, 0x01
	BREQ UF_RBG

	cpi r20, 0x02
	BREQ JOLY_RBG

	cpi r20, 0x03
	BREQ HULK_RBG

	SBRC r20, 2
	clr r20				;reset count of RBGs

	rjmp DETERMINE_RBG_SUB

OFF:
	rcall OFF_PWM
	rjmp DETERMINE_RBG_SUB
	
UF_RBG:	

	rcall UF_ORANGE_PWM

	;rcall UF_BLUE_PWM

	rjmp DETERMINE_RBG_SUB

JOLY_RBG:
	
	;rcall JOLY_RED_PWM			

	rcall JOLY_GREEN_PWM			

	rjmp DETERMINE_RBG_SUB

HULK_RBG:
	
	rcall HULK_PURPLE_PWM			

	;rcall HULK_GREEN_PWM			

	rjmp DETERMINE_RBG_SUB


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
	sts PORTF_DIRSET, r16			;setting button as input
	ret


PORTF_INT0_INIT:
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
	ldi r16, 0x0F
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

OFF_PWM:

	/**RED PWM **/
	ldi r16, ~0x00			
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, ~0x00					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0x00			
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0x00					
	sts TCD0_CCB+1, r16 
				
	/**BLUE PWM **/
	ldi r16, ~0x00				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0x00					
	sts TCD0_CCC+1, r16
	ret

UF_ORANGE_PWM:

	/**RED PWM **/
	ldi r16, ~0xFA			
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, ~0xFA					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0x46			
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0x00					
	sts TCD0_CCB+1, r16 
				
	/**BLUE PWM **/
	ldi r16, ~0x16				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0x00				
	sts TCD0_CCC+1, r16 
	ret

UF_BLUE_PWM:

	/**RED PWM **/
	ldi r16, ~0x00				
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, ~0x00					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0x21			
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0x00					
	sts TCD0_CCB+1, r16
	 			
	/**BLUE PWM **/
	ldi r16, ~0xA5				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0xA5					
	sts TCD0_CCC+1, r16 			
	ret

JOLY_RED_PWM:

	/**RED PWM **/
	ldi r16, ~0xC2 			;Duty cycle			
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, ~0xC2					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0x1F				
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0x00					
	sts TCD0_CCB+1, r16 
				
	/**BLUE PWM **/
	ldi r16, ~0x1F				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0x00					
	sts TCD0_CCC+1, r16 
	ret

JOLY_GREEN_PWM:

		/**RED PWM **/
	ldi r16, ~0x3C			
	sts TCD0_CCA, r16 				;RED PWM
	ldi r16, ~0x00					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0x8D				
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0x8D					
	sts TCD0_CCB+1, r16 	
			
	/**BLUE PWM **/
	ldi r16, ~0x0D				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0x00					
	sts TCD0_CCC+1, r16 
	ret

HULK_PURPLE_PWM:

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

HULK_GREEN_PWM:

	/**RED PWM **/
	ldi r16, ~0x49			
	sts TCD0_CCA, r16 				;RED PWM 
	ldi r16, ~0x00					
	sts TCD0_CCA+1, r16 			

	/**GREEN PWM **/
	ldi r16, ~0xFF				
	sts TCD0_CCB, r16 				;GREEN PWM
	ldi r16, ~0xFF					
	sts TCD0_CCB+1, r16 	
			
	/**BLUE PWM **/
	ldi r16, ~0x07				;BLUE PWM
	sts TCD0_CCC, r16 				
	ldi r16, ~0x00					
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

DELAY_500ms:
	push r16
	push r17
	push r18

	inc r16					;incrementing counter1
	cpi r16, 0xFF 			;compare counter1 to 255
	BRNE DELAY_500ms 		;loop back if counter1 is not full

	inc r17 				;increment the second counter
	cpi r17, 0xFF 			;compare counter2 to 255
	BRNE DELAY_500ms 		;loop back if not full

	inc r18 				;increment the second counter
	cpi r18, 0xFF 			;compare counter2 to 255
	BRNE DELAY_500ms 		;loop back if not full

	ldi r16, 0x00 			;reset counters
	ldi r17, 0x00
	ldi r18, 0x00

	pop r18
	pop r17
	pop r17
	ret


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


TCC0_ISR:	
	push r16				
	lds	r16, CPU_SREG		
	push r16	 	
			
	lds r16, PORTF_IN				;load button input
	SBRS r16, 2						;do not increment if PF2 is high
	inc r19							;store number of interrupts
	com r19							;not count for active low LEDs
	sts PORTC_OUT, r19				;set LED as output
	com r19							;recomplement count

	SBRS r16, 2	
	inc r20	

	ldi r16, 0x00					;disabling timer
	sts TCC0_CTRLA, r16				

	ldi r16, 0x01					;enable ext interrupt
	sts PORTF_INTFLAGS, r16			;clear PORTF int0 flag	
		
	pop	r16					
	sts	CPU_SREG, R16		
	pop r16
	reti





