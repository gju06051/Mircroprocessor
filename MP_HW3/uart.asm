
	.cdecls C, LIST, "Compiler.h"
;------------------------------------------------
;		.data
;------------------------------------------------
GPIO_BASE			.equ	0x40000000
NVIC_BASE			.equ	0xe0000000
SC_BASE             .equ    0xFE000

RCGCGPIO			.equ	0x608
RCGCUART            .equ    0x618
RCGC2               .equ    0x108

GPIOHBCTL			.equ	0x06C
GPIODIR				.equ	0x400
GPIOAFSEL			.equ	0x420
GPIOPUR				.equ	0x510
GPIODEN				.equ	0x51C
GPIOAMSEL			.equ	0x528
GPIOPCTL			.equ	0x52C
GPIOLOCK			.equ	0x520
GPIOCR				.equ	0x524
GPIODR8R            .equ    0x508
GPIODATA			.equ	0x000
GPIOIM				.equ	0x410
GPIOICR				.equ	0x41C

EN3					.equ	0x10C

GPIO_PORT_M_OFFSET  .equ    0x63000
GPIO_PORT_G_OFFSET 	.equ 	0x26000
GPIO_PORT_A_APB_OFFSET  .equ    0x4000

UART0_OFFSET        .equ    0xC000
UARTCTL             .equ    0x030
UARTIBRD            .equ    0x024
UARTFBRD            .equ    0x028
UARTLCRH            .equ    0x02C
UARTFR              .equ    0x018
UARTDR              .equ    0x000

SW_ON				.equ	0x1E ;11110
SW_OFF				.equ	0x1D ;11101
SW_SLOW				.equ	0x1B ;11011
SW_FAST             .equ	0x17 ;10111
SW_INIT				.equ	0x0F ;01111

LED_DELAY_SLOW      .equ    0x200000
LED_DELAY_FAST      .equ    0x70000
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
            .global UART_Init
            .global PRINT_LEDMODE
;------------------------------------------------
;			switch initializition
;------------------------------------------------

Switch_Init:
        ;RCGC GPIO Port M Enable
		mov r0, #GPIO_BASE	;RCGC : General-Purpose Input/Output Run Mode Clock Gating Control
		mov r1, #SC_BASE    ;System Clock Register Offset
		add r1, r1, r0
		mov r0, #RCGCGPIO
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x800  ;Enable GPIO M port
		str r0, [r1]
		nop
		nop

        ;HBCTL : High-Performance Bus Control
		mov r0, #GPIO_BASE
		mov r1, #SC_BASE
		add r1, r1, r0
		mov r0, #GPIOHBCTL
		add r1, r1, r0

		mov r0, #0x800
		str r0, [r1]
		nop
		nop

        ;GPIODIR : GPIO Direction
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
		add r1, r1, r0
		mov r0, #GPIODIR
		add r1, r1, r0

		ldr r0, [r1]
		bic r0, r0, #0x1f
		str r0, [r1]

        ;AFSEL : Alternate Function Select
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
		add r1, r1, r0
		mov r0, #GPIOAFSEL
		add r1, r1, r0

		ldr r0, [r1]
		bic r0, r0, #0x1f
		str r0, [r1]

        ;PUR : Pull-Up Select
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
		add r1, r1, r0
		mov r0, #GPIOPUR
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f
		str r0, [r1]

        ;DEN : Digital Enable
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
		add r1, r1, r0
		mov r0, #GPIODEN
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f
		str r0, [r1]

        ;AMSEL : Analog Mode Select
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
		add r1, r1, r0
		mov r0, #GPIOAMSEL
		add r1, r1, r0

		mov r0, #0
		str r0, [r1]

        ;PCTL : Port Control
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
		add r1, r1, r0
		mov r0, #GPIOPCTL
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f   ;#0x000f000f
		str r0, [r1]

        ;LOCK : GPIO Lock
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
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

        ;CR : Commit
		mov r0, #GPIO_BASE	;CR
		mov r1, #SC_BASE
		add r1, r1, r0
		mov r0, #GPIOCR
		add r1, r1, r0

		ldr r0, [r1]
		mov r2, #0x00000000
		bic r0, r0, r2
		str r0, [r1]
		bx lr

LED_Init:
        ;RCGC2 : GPIO RCGC2
        mov r0, #GPIO_BASE
        mov r1, #SC_BASE
        add r1, r1, r0
        mov r0, #RCGC2
        add r1, r1, r0

        ldr r0, [r1]
        orr r0, r0, #0x40   ;enable read port G pin2
        str r0, [r1]

        ;GPIODIR
        mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIODIR
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;port G pin2 set as output
        str r0, [r1]

        ;GPIOAFSEL GPIO
        mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIOAFSEL
        add r1, r1, r2


        ldr r0, [r1]
        mov r0, #0          ;associated pin controlled by GPIO register
        str r0, [r1]

        ;GPIODR8R
        mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIODR8R
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;port G pin2 controlled by 8mA Drive
        str r0, [r1]

        ;GPIODEN
        mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r2, #GPIODEN
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;digital functions for the port G pin2 enable
        str r0, [r1]
        bx lr

UART_Init:
        ;System Control
        mov r0, #GPIO_BASE  ;RCGCUART -> Enable UART module0
		mov r1, #SC_BASE
		add r1, r1, r0
		mov r0, #RCGCUART
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x01   ;Enable UART0 module
		str r0, [r1]

        ;RCGCGPIO -> Enable Port A
        mov r0, #GPIO_BASE
		mov r1, #SC_BASE
		add r1, r1, r0
		mov r0, #RCGCGPIO
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x01   ;Enable GPIO A port
		str r0, [r1]

        ;UARTCTL -> Disable UART
        mov r0, #GPIO_BASE
		mov r1, #UART0_OFFSET
		add r1, r1, r0
		mov r0, #UARTCTL
		add r1, r1, r0

		ldr r0, [r1]
		bic r0, r0, #0x01   ;Disable UART0
		str r0, [r1]

        ;UARTIBRD -> Set Baud Rate Integer
        mov r0, #GPIO_BASE
		mov r1, #UART0_OFFSET
		add r1, r1, r0
		mov r0, #UARTIBRD
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x08   ;Set Baud Rate
		str r0, [r1]

        ;UARTFBRD -> Set Baud Rate Float
        mov r0, #GPIO_BASE
		mov r1, #UART0_OFFSET
		add r1, r1, r0
		mov r0, #UARTFBRD
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x2C
		str r0, [r1]

        ;UARTLCRH -> Set Word Length, Stop Bit, Parity
        mov r0, #GPIO_BASE
		mov r1, #UART0_OFFSET
		add r1, r1, r0
		mov r0, #UARTLCRH
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x00000060
		str r0, [r1]

        ;UARTCTL -> Enable UART
        mov r0, #GPIO_BASE
		mov r1, #UART0_OFFSET
		add r1, r1, r0
		mov r0, #UARTCTL
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x01      ;Enable UART0
		str r0, [r1]

        ;GPIOAFSEL -> Enable Alternate Function(Peripheral)
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_A_APB_OFFSET
		add r1, r1, r0
		mov r0, #GPIOAFSEL
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x03   ;port A pin0,1 set as alternate function
		str r0, [r1]

        ;GPIOCTL -> Set UART Function (Use U0Rx, U0Tx)
        mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_A_APB_OFFSET
		add r1, r1, r0
		mov r0, #GPIOPCTL
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x11   ;port A pin0,1 set as UART function
		str r0, [r1]


        ;GPIODEN -> Enable Digital Function
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_A_APB_OFFSET
		add r1, r1, r0
		mov r0, #GPIODEN
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x03    ;port A pin0,1 set as digital function
		str r0, [r1]

        ;GIPOAMSEL -> Disable Analog Function
		mov r0, #GPIO_BASE
		mov r1, #GPIO_PORT_A_APB_OFFSET
		add r1, r1, r0
		mov r0, #GPIOAMSEL
		add r1, r1, r0

		mov r0, #0
		str r0, [r1]
        bx lr

PRINT_LEDMODE:

_Check_UART_TXFF_L:
        mov r1, #GPIO_BASE
        mov r2, #UART0_OFFSET
        add r2, r1
        mov r1, #UARTFR
        add r2, r1

        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20

        BEQ _Check_UART_TXFF_L

        mov r3, #GPIO_BASE
        mov r4, #UART0_OFFSET
        add r3, r4
        mov r4, #UARTDR
        add r3, r4

        mov r4, #'L'
        str r4, [r3]

_Check_UART_TXFF_E1:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_E1
        mov r4, #'E'
        str r4, [r3]

_Check_UART_TXFF_D:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_D
        mov r4, #'D'
        str r4, [r3]

_Check_UART_TXFF_B:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_B
        mov r4, #' '
        str r4, [r3]

_Check_UART_TXFF_M1:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_M1
        mov r4, #'M'
        str r4, [r3]

_Check_UART_TXFF_O:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_O
        mov r4, #'O'
        str r4, [r3]

_Check_UART_TXFF_D2:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_D2
        mov r4, #'D'
        str r4, [r3]

_Check_UART_TXFF_E2:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_E2
        mov r4, #'E'
        str r4, [r3]

_Check_UART_TXFF_BACKSPACE:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_BACKSPACE
        mov r4, #0x0A
        str r4, [r3]

_Check_UART_TXFF_CL:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ _Check_UART_TXFF_CL
        mov r4, #0x0D
        str r4, [r3]

        bx lr


Switch_Input:
		mov r5, #GPIO_BASE
		mov r1, #GPIO_PORT_M_OFFSET
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


LED_On:

UART_SW_A_D:
        mov r1, #GPIO_BASE
        mov r2, #UART0_OFFSET
        add r2, r1
        mov r1, #UARTFR
        add r2, r1

        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20

        BEQ UART_SW_A_D

        mov r3, #GPIO_BASE
        mov r4, #UART0_OFFSET
        add r3, r4
        mov r4, #UARTDR
        add r3, r4
        mov r4, #'-'
        str r4, [r3]

UART_SW_A_S:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_A_S
        mov r4, #'S'
        str r4, [r3]

UART_SW_A_W:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_A_W
        mov r4, #'W'
        str r4, [r3]

UART_SW_A_B:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_A_B
        mov r4, #' '
        str r4, [r3]

UART_SW_A_A:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_A_A
        mov r4, #'A'
        str r4, [r3]

UART_SW_A_BACKSPACE:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_A_BACKSPACE
        mov r4, #0x0A
        str r4, [r3]

UART_SW_A_CL:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_A_CL
        mov r4, #0x0D
        str r4, [r3]

LED_Turn_On:
        mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r0, #0x10 	;masking for Port G pin2
        add r1, r1, r0

        mov r5, #0x04 	;write to Port G to turn on LED
        str r5, [r1]

        bx lr

LED_Off:

UART_SW_B_D:
        mov r1, #GPIO_BASE
        mov r2, #UART0_OFFSET
        add r2, r1
        mov r1, #UARTFR
        add r2, r1

        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20

        BEQ UART_SW_B_D

        mov r3, #GPIO_BASE
        mov r4, #UART0_OFFSET
        add r3, r4
        mov r4, #UARTDR
        add r3, r4

        mov r4, #'-'
        str r4, [r3]

UART_SW_B_S:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_B_S
        mov r4, #'S'
        str r4, [r3]

UART_SW_B_W:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_B_W
        mov r4, #'W'
        str r4, [r3]

UART_SW_B_B1:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_B_B1
        mov r4, #' '
        str r4, [r3]

UART_SW_B_B2:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_B_B2
        mov r4, #'B'
        str r4, [r3]

UART_SW_B_BACKSPACE:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_B_BACKSPACE
        mov r4, #0x0A
        str r4, [r3]

UART_SW_B_CL:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_B_CL
        mov r4, #0x0D
        str r4, [r3]

LED_Turn_Off:
        mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G_OFFSET
        add r1, r1, r0
        mov r0, #0x10 	;masking for Port G pin2
        add r1, r1, r0

        mov r5, #0x00 	;write to Port G to turn off LED
        str r5, [r1]

        bx lr

Blink_slow:

UART_SW_C_D:
        mov r1, #GPIO_BASE
        mov r2, #UART0_OFFSET
        add r2, r1
        mov r1, #UARTFR
        add r2, r1

        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20

        BEQ UART_SW_C_D

        mov r3, #GPIO_BASE
        mov r4, #UART0_OFFSET
        add r3, r4
        mov r4, #UARTDR
        add r3, r4

        mov r4, #'-'
        str r4, [r3]

UART_SW_C_S:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_C_S
        mov r4, #'S'
        str r4, [r3]

UART_SW_C_W:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_C_W
        mov r4, #'W'
        str r4, [r3]

UART_SW_C_B:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_C_B
        mov r4, #' '
        str r4, [r3]

UART_SW_C_C:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_C_C
        mov r4, #'C'
        str r4, [r3]

UART_SW_C_BACKSPACE:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_C_BACKSPACE
        mov r4, #0x0A
        str r4, [r3]

UART_SW_C_CL:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_C_CL
        mov r4, #0x0D
        str r4, [r3]

LED_SLOW_BLINK:
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

	mov r6, #LED_DELAY_SLOW
	bl DELAY_LOOP_S

	mov r5, #0x00	;write to Port G to turn off LED
	str r5, [r1]

	mov r6, #LED_DELAY_SLOW
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

UART_SW_D_D1:
        mov r1, #GPIO_BASE
        mov r2, #UART0_OFFSET
        add r2, r1
        mov r1, #UARTFR
        add r2, r1

        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20

        BEQ UART_SW_D_D1

        mov r3, #GPIO_BASE
        mov r4, #UART0_OFFSET
        add r3, r4
        mov r4, #UARTDR
        add r3, r4

        mov r4, #'-'
        str r4, [r3]

UART_SW_D_S:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_D_S
        mov r4, #'S'
        str r4, [r3]

UART_SW_D_W:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_D_W
        mov r4, #'W'
        str r4, [r3]

UART_SW_D_B:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_D_B
        mov r4, #' '
        str r4, [r3]

UART_SW_D_D2:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_D_D2
        mov r4, #'D'
        str r4, [r3]

UART_SW_D_BACKSPACE:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_D_BACKSPACE
        mov r4, #0x0A
        str r4, [r3]

UART_SW_D_CL:
        ldr r1, [r2]
        and r1, #0x20           ;compare if TXFF bit is 1
        cmp r1, #0x20
        BEQ UART_SW_D_CL
        mov r4, #0x0D
        str r4, [r3]

LED_FAST_BLINK:
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

	mov r6, #LED_DELAY_FAST
	bl DELAY_LOOP_F

	mov r5, #0x00	;write to Port G to turn off LED
	str r5, [r1]

	mov r6, #LED_DELAY_FAST
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


			.retain
			.retainrefs
