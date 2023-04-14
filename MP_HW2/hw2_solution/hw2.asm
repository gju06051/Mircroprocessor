
	.cdecls C, LIST, "Compiler.h"
;------------------------------------------------
;		.data
;------------------------------------------------
GPIO_BASE			.equ	0x40000000
NVIC_BASE			.equ	0xe0000000
RCGCGPIO			.equ	0x608
GPIOHBCTL			.equ	0x06C
GPIODIR				.equ	0x400
GPIOAFSEL			.equ	0x420
GPIOPUR				.equ	0x510
GPIODEN				.equ	0x51C
GPIOAMSEL			.equ	0x528
GPIOPCTL			.equ	0x52C
GPIOLOCK			.equ	0x520
GPIOCR				.equ	0x524
RCGC2				.equ	0x108
GPIODR8R			.equ	0x508

GPIODATA			.equ	0x000
EN3					.equ	0x10C
GPIOIM				.equ	0x410
GPIOICR				.equ	0x41C

SW_ON				.equ	0x1E ;11110
SW_OFF				.equ	0x1D ;11101
SW_SLOW				.equ	0x1B ;11011
SW_FAST				.equ	0x17 ;10111
;--------------------------------------------------
             .text                           ; Program Start
             .global RESET                   ; Define entry point
             .align	4
			 .sect ".text"

             .global Switch_Init
             .global Switch_Input
			 .global num_1
			 .global num_3

			 .global LED_Init
			 .global LED_On
			 .global LED_Off

			 .global Blink_slow
			 .global Blink_fast

;------------------------------------------------
;			switch initializition
;------------------------------------------------

LED_Init:
		mov r0, #GPIO_BASE	;RCGC2
		mov r1, #0xFE000
		add r1, r1, r0
		mov r0, #RCGC2
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x40  ; enable read port G pin2
		str r0, [r1]

		mov r0, #GPIO_BASE		;GPIODIR
		mov r1, #0x26000
		add r1, r1, r0
		mov r2, #GPIODIR
		add r1, r1, r2

		ldr r0, [r1]
		orr r0, r0, #0x04	;Port G pin2 set as output
		str r0, [r1]

		mov r0, #GPIO_BASE		;GPIOAFSEL GPIO
		mov r1, #0x26000
		add r1, r1, r0
		mov r2, #GPIOAFSEL
		add r1, r1, r2

		ldr r0, [r1]
		mov r0, #0				;associated pin controlled by GPIO register
		str r0, [r1]

		mov r0, #GPIO_BASE		;GPIODR8R
		mov r1, #0x26000
		add r1, r1, r0
		mov r2, #GPIODR8R
		add r1, r1, r2

		ldr r0, [r1]
		orr r0, r0, #0x04		;port G pin2 controlled by 8mA Drive
		str r0, [r1]

		mov r0, #GPIO_BASE		;GPIODEN
		mov r1, #0x26000
		add r1, r1, r0
		mov r2, #GPIODEN
		add r1, r1, r2

		ldr r0, [r1]
		orr r0, r0, #0x04		;digital functions for the port G pin2 enable
		str r0, [r1]


Switch_Init:
		mov r0, #GPIO_BASE	;RCGC : General-Purpose Input/Output Run Mode Clock Gating Control
		mov r1, #0xFE000
		add r1, r1, r0
		mov r0, #RCGCGPIO
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x800 ;Enable GPIO M port
		str r0, [r1]
		nop
		nop

		mov r0, #GPIO_BASE	;HBCTL : High-Performance Bus Control
		mov r1, #0xFE000
		add r1, r1, r0
		mov r0, #GPIOHBCTL
		add r1, r1, r0

		mov r0, #0x800
		str r0, [r1]
		nop
		nop

		mov r0, #GPIO_BASE	;DIR
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIODIR
		add r1, r1, r0

		ldr r0, [r1]
		bic r0, r0, #0x1f
		str r0, [r1]

		mov r0, #GPIO_BASE	;AFSEL : Alternate Function Select
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIOAFSEL
		add r1, r1, r0

		ldr r0, [r1]
		bic r0, r0, #0x1f
		str r0, [r1]

		mov r0, #GPIO_BASE	;PUR
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIOPUR
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f
		str r0, [r1]

		mov r0, #GPIO_BASE	;DEN
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIODEN
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f
		str r0, [r1]

		mov r0, #GPIO_BASE	;AMSEL : Analog Mode Select
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIOAMSEL
		add r1, r1, r0

		mov r0, #0
		str r0, [r1]

		mov r0, #GPIO_BASE	;PCTL : Port Control
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIOPCTL
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f;#0x000f000f
		str r0, [r1]

		mov r0, #GPIO_BASE	;LOCK
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIOLOCK
		add r1, r1, r0

		mov r0, #GPIO_BASE
		mov r2, #0xc400000
		add r2, r2, r0
		mov r0, #0xf4000
		add r2, r2, r0
		mov r0, #0x34b
		add r2, r2, r0

		ldr r0, [r1]
		orr r0, r0, r2
		str r0, [r1]

		mov r0, #GPIO_BASE	;CR
		mov r1, #0x63000
		add r1, r1, r0
		mov r0, #GPIOCR
		add r1, r1, r0

		ldr r0, [r1]
		mov r2, #0x00000000
		bic r0, r0, r2
		str r0, [r1]

Switch_Input:

		mov r5, #GPIO_BASE
		mov r1, #0x63000
		add r1, r1, r5
		mov r5, #0x7c
		add r1, r1, r5

		ldr r5, [r1]

DELAY:	MOVW r3,#0xffff

_DELAY_LOOP:
		CBZ r3,_DELAY_EXIT		;Compare and Branch on Zero
		sub r3,r3,#1
		B _DELAY_LOOP
_DELAY_EXIT:

		cmp r5, #SW_ON
		BEQ _on

		cmp r5, #SW_OFF
		BEQ _off

		cmp r5, #SW_SLOW
		BEQ _slow

		cmp r5, #SW_FAST
		BEQ _fast

		mov r1, #'A'
		b _EXIT			;If nothing was pressed


_on:
		mov r1, #'B'
		b _EXIT

_off:
		mov r1, #'C'
		b _EXIT
_slow:
		mov r1, #'D'
		b _EXIT

_fast:
		mov r1, #'E'
		b _EXIT

num_1:
		mov r0, r1
		bx lr		;go back to C code


LED_On:
		mov r0, #GPIO_BASE
		mov r1, #0x26000
		add r1, r1, r0
		mov r0, #0x10  ;masking for Port G pin2
		add r1, r1, r0
		mov r5, #0x04 ;write tp port G to turn on LED
		str r5, [r1]

		b _EXIT

LED_Off:
		mov r0, #GPIO_BASE
		mov r1, #0x26000
		add r1, r1, r0
		mov r0, #0x10 	;masking for Port G pin2 addr[9:2]
		add r1, r1, r0
		mov r5, #0x00 ; wrote to port G to turn off LED
		str r5, [r1]

		b _EXIT

;////////////blink slowly///////////////////////////////

Blink_slow:
		mov r0, #GPIO_BASE
		mov r1, #0x26000
		add r1, r1, r0		; to LED's address
		mov r0, #0x10
		add r1, r1, r0

		mov r6, #5			;blink 5 times

s_on:
		mov r5, #0x04 		;write tp port G to turn on LED
		str r5, [r1]

s_Delay_i:
		MOVW r3,#0xffff; set register value large number
		mov r3, r3, LSL#6	;long delay blink slowly
s_Delay_Loop_i:
		sub r3,r3,#1			; subtract 1
		CBZ r3, s_off	;Compare and Branch on Zero (if r3 becomes 0)
		b s_Delay_Loop_i			;else branch

s_off:
		mov r5, #0x00 ; wrote to port G to turn off LED
		str r5, [r1];Turn off LED

		sub r6, #1
		CBZ r6, _EXIT		;when it completes blinking 5 times, exit
s_Delay_i_:
		MOVW r3,#0xffff; set register value large number
		mov r3, r3, LSL#6

s_Delay_Loop_i_:
		sub r3, r3, #1			; subtract 1
		CMP r3, #0
		BEQ s_on	;Compare and Branch on Zero (if r3 becomes 0)
		b s_Delay_Loop_i_			;else branch
;///////////////////////////////////////////////////////////////////////////
;////////////Blink fast/////////////////////////////////
Blink_fast:
		mov r0, #GPIO_BASE
		mov r1, #0x26000
		add r1, r1, r0		; to LED's address
		mov r0, #0x10
		add r1, r1, r0

		mov r6, #5			;blink 5 times

f_on:
		mov r5, #0x04 		;write tp port G to turn on LED
		str r5, [r1]

f_Delay_i:
		MOVW r3,#0xffff  ; set register value large number
		mov r3, r3, LSL#4  ;short delay blink fast

f_Delay_Loop_i:
		sub r3,r3,#1			; subtract 1
		CBZ r3, f_off	;Compare and Branch on Zero (if r3 becomes 0)
		b f_Delay_Loop_i			;else branch

f_off:
		mov r5, #0x00 ; wrote to port G to turn off LED
		str r5, [r1];Turn off LED

		sub r6, #1
		CBZ r6, _EXIT		;when it completes blinking 5 times, exit

f_Delay_i_:
		MOVW r3,#0xffff; set register value large number
		mov r3, r3, LSL#4

f_Delay_Loop_i_:
		sub r3,r3,#1			; subtract 1
		CMP r3, #0
		BEQ f_on	;Compare and Branch on Zero (if r3 becomes 0)
		b f_Delay_Loop_i_			;else branch
;///////////////////////////////////////////////

_EXIT:
		bx lr		;go back to C code


			.retain
			.retainrefs
