
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
RCGC2               .equ    0x108
GPIODR8R            .equ    0x508

GPIODATA			.equ	0x000
EN3					.equ	0x10C
GPIOIM				.equ	0x410
GPIOICR				.equ	0x41C

GPIO_PORT_G_OFFSET 	.equ 	0x26000

SW_ON				.equ	0x1E ;11110
SW_OFF				.equ	0x1D ;11101
SW_SLOW				.equ	0x1B ;11011
SW_FAST             .equ	0x17 ;10111
SW_INIT				.equ	0x0F ;01111
;--------------------------------------------------
             .text                           ; Program Start
             .global RESET                   ; Define entry point
             .align	4
			 .sect ".text"

             .global Switch_Init
             .global Switch_Input
			 .global num_1

			 .global LED_Init
			 .global LED_On
			 .global LED_Off

			 .global Blink_slow
			 .global Blink_fast

;------------------------------------------------
;			switch initializition
;------------------------------------------------

Switch_Init:
		mov r0, #GPIO_BASE	;RCGC : General-Purpose Input/Output Run Mode Clock Gating Control
		mov r1, #0xFE000
		add r1, r1, r0
		mov r0, #RCGCGPIO
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x800  ;Enable GPIO M port
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

LED_Init:
        mov r0, #GPIO_BASE   ;RCGC2
        mov r1, #0xFE000
        add r1, r1, r0
        mov r0, #RCGC2
        add r1, r1, r0

        ldr r0, [r1]
        orr r0, r0, #0x40   ;enable read port G pin2
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIODIR
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIODIR
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;port G pin2 set as output
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIOAFSEL GPIO
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIOAFSEL
        add r1, r1, r2

        ldr r0, [r1]
        mov r0, #0          ;associated pin controlled by GPIO register
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIODR8R
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIODR8R
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;port G pin2 controlled by 8mA Drive
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIODEN
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIODEN
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;digital functions for the port G pin2 enable
        str r0, [r1]

Switch_Input:
		mov r5, #GPIO_BASE
		mov r1, #0x63000
		add r1, r1, r5
		mov r5, #0x7c
		add r1, r1, r5

		ldr r5, [r1]

DELAY:	MOV r3, #0xffff

_DELAY_LOOP:
		CBZ r3, _DELAY_EXIT		;Compare and Branch on Zero
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
		b _EXIT                 ;nothing

_EXIT:
		bx lr
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
		bx lr                   ;go back to C code

Blink_slow:
   	mov r7, lr
	mov r2, #0x05

	mov r0, #GPIO_BASE
	mov r1, #GPIO_PORT_G_OFFSET
	add r1, r1, r0
	mov r0, #0x10 	;masking for Port G pin2
	add r1, r1, r0

	bl LOOP_S

LOOP_S:
	CBZ r2, _EXIT_S
	sub r2, r2, #1

	mov r5, #0x04 	;write to Port G to turn on LED
	str r5, [r1]

	mov r6, #0x500000
	bl DELAY_LOOP_S

	mov r5, #0x00	;write to Port G to turn off LED
	str r5, [r1]

	mov r6, #0x500000
	bl DELAY_LOOP_S

	b LOOP_S

DELAY_LOOP_S:
	CBZ r6, _EXIT_D_S		;Compare and Branch on Zero
	sub r6, r6, #1
	b DELAY_LOOP_S

_EXIT_D_S:
	bx lr

_EXIT_S:
	bx r7

Blink_fast:
   	mov r7, lr
	mov r2, #0x05

	mov r0, #GPIO_BASE
	mov r1, #GPIO_PORT_G_OFFSET
	add r1, r1, r0
	mov r0, #0x10 	;masking for Port G pin2
	add r1, r1, r0

	bl LOOP_F

LOOP_F:
	CBZ r2, _EXIT_F
	sub r2, r2, #1

	mov r5, #0x04 	;write to Port G to turn on LED
	str r5, [r1]

	mov r6, #0x100000
	bl DELAY_LOOP_F

	mov r5, #0x00	;write to Port G to turn off LED
	str r5, [r1]

	mov r6, #0x100000
	bl DELAY_LOOP_F

	b LOOP_F

DELAY_LOOP_F:
	CBZ r6, _EXIT_D_F		;Compare and Branch on Zero
	sub r6, r6, #1
	b DELAY_LOOP_F

_EXIT_D_F:
	bx lr

_EXIT_F:
	bx r7


LED_On:
	mov r0, #GPIO_BASE
	mov r1, #GPIO_PORT_G_OFFSET
	add r1, r1, r0
	mov r0, #0x10 	;masking for Port G pin2
	add r1, r1, r0
	mov r5, #0x04 	;write to Port G to turn on LED
	str r5, [r1]
	bx lr

LED_Off:
	mov r0, #GPIO_BASE
	mov r1, #GPIO_PORT_G_OFFSET
	add r1, r1, r0
	mov r0, #0x10 	;masking for Port G pin2
	add r1, r1, r0
	mov r5, #0x00 	;write to Port G to turn off LED
	str r5, [r1]
	bx lr



			.retain
			.retainrefs
