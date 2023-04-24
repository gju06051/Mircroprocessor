
	.cdecls C, LIST, "Compiler.h"
;------------------------------------------------
;		.data
;------------------------------------------------
GPIO_BASE			.equ	0x40000000
NVIC_BASE			.equ	0xE000E000

SC_BASE             .equ    0xFE000
GPIO_PORT_M         .equ    0x63000
GPIO_PORT_G         .equ 	0x26000

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
GPIOIS              .equ    0x404
GPIOIBE             .equ    0x408
GPIOIEV             .equ    0x40C

SW_UP				.equ	0x1E    ;11110
SW_DOWN				.equ	0x1D    ;11101
SW_LEFT             .equ    0x1B    ;11011
;--------------------------------------------------
            .text           ; Program Start
            .global RESET   ; Define entry point
            .align	4
			.sect ".text"

            .global Switch_Init
            .global LED_Init
            .global Interrupt_Init
            .global Switch_Input
			.global num_1
			.global num_3
			.global stack
_stack:
;--------------------------------------------------
; Interrupt Vectors
;--------------------------------------------------
	.sect	".intvecs"
	.align	4
	.field	IntDefaultHandler,32 ; g_pfnVectors[0] @ 0
	.field	IntDefaultHandler,32 ; g_pfnVectors[1] @ 32
	.field	IntDefaultHandler,32 ; g_pfnVectors[2] @ 64
	.field	IntDefaultHandler,32 ; g_pfnVectors[3] @ 96
	.field	IntDefaultHandler,32 ; g_pfnVectors[4] @ 128
	.field	IntDefaultHandler,32 ; g_pfnVectors[5] @ 160
	.field	IntDefaultHandler,32 ; g_pfnVectors[6] @ 192
	.field	0,32                 ; g_pfnVectors[7] @ 224
	.field	0,32                 ; g_pfnVectors[8] @ 256
	.field	0,32                 ; g_pfnVectors[9] @ 288
	.field	0,32                 ; g_pfnVectors[10] @ 320
	.field	IntDefaultHandler,32 ; g_pfnVectors[11] @ 352
	.field	IntDefaultHandler,32 ; g_pfnVectors[12] @ 384
	.field	0,32			     ; g_pfnVectors[13] @ 416
	.field	IntDefaultHandler,32 ; g_pfnVectors[14] @ 448
	.field	IntDefaultHandler,32 ; g_pfnVectors[15] @ 480
	.field	IntDefaultHandler,32 ; g_pfnVectors[16] @ 512
	.field	IntDefaultHandler,32 ; g_pfnVectors[17] @ 544
	.field	IntDefaultHandler,32 ; g_pfnVectors[18] @ 576
	.field	IntDefaultHandler,32 ; g_pfnVectors[19] @ 608
	.field	IntDefaultHandler,32 ; g_pfnVectors[20] @ 640
	.field	IntDefaultHandler,32 ; g_pfnVectors[21] @ 672
	.field	IntDefaultHandler,32 ; g_pfnVectors[22] @ 704
	.field	IntDefaultHandler,32 ; g_pfnVectors[23] @ 736
	.field	IntDefaultHandler,32 ; g_pfnVectors[24] @ 768
	.field	IntDefaultHandler,32 ; g_pfnVectors[25] @ 800
	.field	IntDefaultHandler,32 ; g_pfnVectors[26] @ 832
	.field	IntDefaultHandler,32 ; g_pfnVectors[27] @ 864
	.field	IntDefaultHandler,32 ; g_pfnVectors[28] @ 896
	.field	IntDefaultHandler,32 ; g_pfnVectors[29] @ 928
	.field	IntDefaultHandler,32 ; g_pfnVectors[30] @ 960
	.field	IntDefaultHandler,32 ; g_pfnVectors[31] @ 992
	.field	IntDefaultHandler,32 ; g_pfnVectors[32] @ 1024
	.field	IntDefaultHandler,32 ; g_pfnVectors[33] @ 1056
	.field	IntDefaultHandler,32 ; g_pfnVectors[34] @ 1088
	.field	IntDefaultHandler,32 ; g_pfnVectors[35] @ 1120
	.field	IntDefaultHandler,32 ; g_pfnVectors[36] @ 1152
	.field	IntDefaultHandler,32 ; g_pfnVectors[37] @ 1184
	.field	IntDefaultHandler,32 ; g_pfnVectors[38] @ 1216
	.field	IntDefaultHandler,32 ; g_pfnVectors[39] @ 1248
	.field	IntDefaultHandler,32 ; g_pfnVectors[40] @ 1280
	.field	IntDefaultHandler,32 ; g_pfnVectors[41] @ 1312
	.field	IntDefaultHandler,32 ; g_pfnVectors[42] @ 1344
	.field	IntDefaultHandler,32 ; g_pfnVectors[43] @ 1376
	.field	IntDefaultHandler,32 ; g_pfnVectors[44] @ 1408
	.field	IntDefaultHandler,32 ; g_pfnVectors[45] @ 1440
	.field	IntDefaultHandler,32 ; g_pfnVectors[46] @ 1472
	.field	IntDefaultHandler,32 ; g_pfnVectors[47] @ 1504
	.field	IntDefaultHandler,32 ; g_pfnVectors[48] @ 1536
	.field	IntDefaultHandler,32 ; g_pfnVectors[49] @ 1568
	.field	IntDefaultHandler,32 ; g_pfnVectors[50] @ 1600
	.field	IntDefaultHandler,32 ; g_pfnVectors[51] @ 1632
	.field	IntDefaultHandler,32 ; g_pfnVectors[52] @ 1664
	.field	IntDefaultHandler,32 ; g_pfnVectors[53] @ 1696
	.field	IntDefaultHandler,32 ; g_pfnVectors[54] @ 1728
	.field	IntDefaultHandler,32 ; g_pfnVectors[55] @ 1760
	.field	IntDefaultHandler,32 ; g_pfnVectors[56] @ 1792
	.field	IntDefaultHandler,32 ; g_pfnVectors[57] @ 1824
	.field	0,32			     ; g_pfnVectors[58] @ 1856
	.field	IntDefaultHandler,32 ; g_pfnVectors[59] @ 1888
	.field	IntDefaultHandler,32 ; g_pfnVectors[60] @ 1920
	.field	IntDefaultHandler,32 ; g_pfnVectors[61] @ 1952
	.field	IntDefaultHandler,32 ; g_pfnVectors[62] @ 1984
	.field	IntDefaultHandler,32 ; g_pfnVectors[63] @ 2016
	.field	IntDefaultHandler,32 ; g_pfnVectors[64] @ 2048
	.field	IntDefaultHandler,32 ; g_pfnVectors[65] @ 2080
	.field	IntDefaultHandler,32 ; g_pfnVectors[66] @ 2112
	.field	IntDefaultHandler,32 ; g_pfnVectors[67] @ 2144
	.field	0,32                 ; g_pfnVectors[68] @ 2176
	.field	0,32                 ; g_pfnVectors[69] @ 2208
	.field	IntDefaultHandler,32 ; g_pfnVectors[70] @ 2240
	.field	IntDefaultHandler,32 ; g_pfnVectors[71] @ 2272
	.field	IntDefaultHandler,32 ; g_pfnVectors[72] @ 2304
	.field	IntDefaultHandler,32 ; g_pfnVectors[73] @ 2336
	.field	IntDefaultHandler,32 ; g_pfnVectors[74] @ 2368
	.field	IntDefaultHandler,32 ; g_pfnVectors[75] @ 2400
	.field	IntDefaultHandler,32 ; g_pfnVectors[76] @ 2432
	.field	IntDefaultHandler,32 ; g_pfnVectors[77] @ 2464
	.field	IntDefaultHandler,32 ; g_pfnVectors[78] @ 2496
	.field	IntDefaultHandler,32 ; g_pfnVectors[79] @ 2528
	.field	0,32                 ; g_pfnVectors[80] @ 2560
	.field	0,32                 ; g_pfnVectors[81] @ 2592
	.field	0,32                 ; g_pfnVectors[82] @ 2624
	.field	0,32                 ; g_pfnVectors[83] @ 2656
	.field	IntDefaultHandler,32 ; g_pfnVectors[84] @ 2688
	.field	IntDefaultHandler,32 ; g_pfnVectors[85] @ 2720
	.field	IntDefaultHandler,32 ; g_pfnVectors[86] @ 2752
	.field	IntDefaultHandler,32 ; g_pfnVectors[87] @ 2784
	.field	0,32                 ; g_pfnVectors[88] @ 2816
	.field	0,32                 ; g_pfnVectors[89] @ 2848
	.field	0,32                 ; g_pfnVectors[90] @ 2880
	.field	0,32                 ; g_pfnVectors[91] @ 2912
	.field	0,32                 ; g_pfnVectors[92] @ 2944
	.field	0,32                 ; g_pfnVectors[93] @ 2976
	.field	0,32                 ; g_pfnVectors[94] @ 3008
	.field	0,32                 ; g_pfnVectors[95] @ 3040
	.field	0,32                 ; g_pfnVectors[96] @ 3072
	.field	0,32                 ; g_pfnVectors[97] @ 3104
	.field	0,32                 ; g_pfnVectors[98] @ 3136
	.field	0,32                 ; g_pfnVectors[99] @ 3168
	.field	0,32                 ; g_pfnVectors[100] @ 3200
	.field	0,32                 ; g_pfnVectors[101] @ 3232
	.field	0,32                 ; g_pfnVectors[102] @ 3264
	.field	0,32                 ; g_pfnVectors[103] @ 3296
	.field	0,32                 ; g_pfnVectors[104] @ 3328
	.field	0,32                 ; g_pfnVectors[105] @ 3360
	.field	0,32                 ; g_pfnVectors[106] @ 3392
	.field	0,32                 ; g_pfnVectors[107] @ 3424
	.field	IntDefaultHandler,32 ; g_pfnVectors[108] @ 3456
	.field	IntDefaultHandler,32 ; g_pfnVectors[109] @ 3488
	.field	IntDefaultHandler,32 ; g_pfnVectors[110] @ 3520
	.field	IntDefaultHandler,32 ; g_pfnVectors[111] @ 3552
	.field	IntDefaultHandler,32 ; g_pfnVectors[112] @ 3584
	.field	IntDefaultHandler,32 ; g_pfnVectors[113] @ 3616
	.field	IntDefaultHandler,32 ; g_pfnVectors[114] @ 3648
	.field	IntDefaultHandler,32 ; g_pfnVectors[115] @ 3680
	.field	IntDefaultHandler,32 ; g_pfnVectors[116] @ 3712
	.field	IntDefaultHandler,32 ; g_pfnVectors[117] @ 3744
	.field	IntDefaultHandler,32 ; g_pfnVectors[118] @ 3776
	.field	IntDefaultHandler,32 ; g_pfnVectors[119] @ 3808
	.field	IntDefaultHandler,32 ; g_pfnVectors[120] @ 3840
	.field	IntDefaultHandler,32 ; g_pfnVectors[121] @ 3872
	.field	IntDefaultHandler,32 ; g_pfnVectors[122] @ 3904
	.field	0,32                 ; g_pfnVectors[123] @ 3936
	.field	0,32                 ; g_pfnVectors[124] @ 3968
	.field	IntDefaultHandler,32 ; g_pfnVectors[125] @ 4000
	.field	IntDefaultHandler,32 ; g_pfnVectors[126] @ 4032
	.field	IntGPIOm,32          ; g_pfnVectors[127] @ 4064
	.field	IntDefaultHandler,32 ; g_pfnVectors[128] @ 4096
	.field	IntDefaultHandler,32 ; g_pfnVectors[129] @ 4128
	.field	0,32                 ; g_pfnVectors[130] @ 4160
	.field	0,32                 ; g_pfnVectors[131] @ 4192
	.field	IntDefaultHandler,32 ; g_pfnVectors[132] @ 4224
	.field	IntDefaultHandler,32 ; g_pfnVectors[133] @ 4256
	.field	IntDefaultHandler,32 ; g_pfnVectors[134] @ 4288
	.field	IntDefaultHandler,32 ; g_pfnVectors[135] @ 4320
	.field	IntDefaultHandler,32 ; g_pfnVectors[136] @ 4352
	.field	IntDefaultHandler,32 ; g_pfnVectors[137] @ 4384
	.field	IntDefaultHandler,32 ; g_pfnVectors[138] @ 4416
	.field	IntDefaultHandler,32 ; g_pfnVectors[139] @ 4448
	.field	IntDefaultHandler,32 ; g_pfnVectors[140] @ 4480
	.field	IntDefaultHandler,32 ; g_pfnVectors[141] @ 4512
	.field	IntDefaultHandler,32 ; g_pfnVectors[142] @ 4544
	.field	IntDefaultHandler,32 ; g_pfnVectors[143] @ 4576
	.field	IntDefaultHandler,32 ; g_pfnVectors[144] @ 4608
	.field	IntDefaultHandler,32 ; g_pfnVectors[145] @ 4640
	.field	IntDefaultHandler,32 ; g_pfnVectors[146] @ 4672
	.field	IntDefaultHandler,32 ; g_pfnVectors[147] @ 4704
	.field	IntDefaultHandler,32 ; g_pfnVectors[148] @ 4736
	.field	IntDefaultHandler,32 ; g_pfnVectors[149] @ 4768
	.field	IntDefaultHandler,32 ; g_pfnVectors[150] @ 4800
	.field	IntDefaultHandler,32 ; g_pfnVectors[151] @ 4832
	.field	IntDefaultHandler,32 ; g_pfnVectors[152] @ 4864
	.field	IntDefaultHandler,32 ; g_pfnVectors[153] @ 4896
	.field	IntDefaultHandler,32 ; g_pfnVectors[154] @ 4928 


;------------------------------------------------
;			switch initializition
;------------------------------------------------
Switch_Init:
		mov r0, #GPIO_BASE	;RCGC(General Purpose IO Clock)
		mov r1, #SC_BASE    ;SC(System Clock)
		add r1, r1, r0
		mov r0, #RCGCGPIO
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x800  ;GPIO M Port
		str r0, [r1]
		nop
		nop

		mov r0, #GPIO_BASE	;HBCTL(High Bus Control)
		mov r1, #SC_BASE
		add r1, r1, r0
		mov r0, #GPIOHBCTL
		add r1, r1, r0

		mov r0, #0x800
		str r0, [r1]
		nop
		nop

		mov r0, #GPIO_BASE	;DIR(Port Direction)
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r0, #GPIODIR
		add r1, r1, r0

		ldr r0, [r1]
		bic r0, r0, #0x1f   ;0x1f = 0001_1111
		str r0, [r1]

		mov r0, #GPIO_BASE	;DEN(Digital Enable)
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r0, #GPIODEN
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f   ;0x1f = 0001_1111
		str r0, [r1]

		mov r0, #GPIO_BASE	;PUR(Pull-Up Select)
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r0, #GPIOPUR
		add r1, r1, r0

		ldr r0, [r1]
		orr r0, r0, #0x1f   ;0x1f = 0001_1111
		str r0, [r1]

;------------------------------------------------
;			LED initializition
;------------------------------------------------
LED_Init:
        mov r0, #GPIO_BASE  ;RCGC2 : GPIO RCGC2
        mov r1, #SC_BASE
        add r1, r1, r0
        mov r0, #RCGC2
        add r1, r1, r0

        ldr r0, [r1]
        orr r0, r0, #0x40   ;enable read port G pin2
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIODIR
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r2, #GPIODIR
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;port G pin2 set as output
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIOAFSEL GPIO
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r2, #GPIOAFSEL
        add r1, r1, r2

        ldr r0, [r1]
        mov r0, #0          ;associated pin by GPIO register
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIODR8R
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r2, #GPIODR8R
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;port G pin2 controlled by 8mA
        str r0, [r1]

        mov r0, #GPIO_BASE  ;GPIODEN
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r2, #GPIODEN
        add r1, r1, r2

        ldr r0, [r1]
        orr r0, r0, #0x04   ;digital for port G pin2 enable
        str r0, [r1]
        bx lr

;------------------------------------------------
;			Interrupt initializition
;------------------------------------------------
Interrupt_Init:

INEDGEHI:
		mov r0, #GPIO_BASE  ;GPIO Interrupt sense
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r2, #GPIOIS
		add r1, r1, r2
		
		ldr r0, [r1]
		orr r0, r0, #0xFF   ;level-trigger
		;orr r0, r0, #0x00   ;edge-trigger
		str r0, [r1]

		mov r0, #GPIO_BASE  ;GPIO both edges 
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r2, #GPIOIBE    ;both edges
		add r1, r1, r2
		
		ldr r0, [r1]
		orr r0, r0, #0x04   ;sw2 only both edge trigger 
		str r0, [r1]

		mov r0, #GPIO_BASE  ;GPIO interrupt event
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r2, #GPIOIEV    ;rising edge
		add r1, r1, r2
		
		ldr r0, [r1]
		orr r0, r0, #0xFF   ;rising-edge 
		str r0, [r1]

INTEN:
		mov r0, #NVIC_BASE  ;NVIC Interrupt Set-Enable 
        mov r1, #EN3        ;IRQ 96-127
        add r1, r1, r0
        
        ldr r0, [r1]
        orr r0, r0, #0x8000 ;IRQ111(EN3[15]) enable
        str r0, [r1]
        
UNMSK:
		mov r0, #GPIO_BASE  ;Unmask interrupt for Port M
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r2, #GPIOIM
		add r1, r1, r2
		
		ldr r0, [r1]
		orr r0, r0, #0xFF   ;pin 0~7 interrupt enable 
		str r0, [r1]
		bx lr
	
;------------------------------------------------
;			Other Function
;------------------------------------------------
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
		cmp r5, #SW_UP
		BEQ _up
		cmp r5, #SW_DOWN
		BEQ _down
		mov r1, #'A'
		b _EXIT
_up:
		mov r1, #'B'
		b _EXIT
_down:
		mov r1, #'C'
		b _EXIT
_EXIT:
		bx lr
num_1:
		mov r0, r1
		bx lr

IntDefaultHandler:
		bx lr

IntGPIOm:

ICL:
		mov r0, #GPIO_BASE      ;interrupt clear
		mov r1, #GPIO_PORT_M
		add r1, r1, r0
		mov r2, #GPIOICR
		add r1, r1, r2
		
		ldr r0, [r1]
		orr r0, r0, #0xFF        
		str r0, [r1]
		
LED_Behavior:
		;LED behave
		;read switch value
Switch_Check:
		mov r5, #GPIO_BASE
		mov r1, #GPIO_PORT_M
		add r1, r1, r5
		mov r5, #0x7c       ;0x7c = 0111_1100(SW0~4)
		add r1, r1, r5

		ldr r5, [r1]

		cmp r5, #SW_UP      ;SW1(LED ON)
		BEQ LED_ON

		cmp r5, #SW_DOWN    ;SW2(LED OFF)
		BEQ LED_OFF

		cmp r5, #SW_LEFT    ;SW3(LED ON/OFF)
		BEQ LED_ONOFF

		mov r1, #'A'
		b _EXIT
		
LED_ON:
		mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r0, #0x10 	;masking for Port G pin2
        add r1, r1, r0

        mov r5, #0x04 	;write to Port G to turn on LED
        str r5, [r1]
		bx lr
		
LED_OFF:
		mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r0, #0x10 	;masking for Port G pin2
        add r1, r1, r0

        mov r5, #0x00 	;write to Port G to turn off LED
        str r5, [r1]
		bx lr		
		
LED_ONOFF:
		mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r0, #0x10 	;masking for Port G pin2
        add r1, r1, r0
		
		mov r5, #0x04
		str r5, [r1]        
        
Switch_LOOP:
		mov r5, #GPIO_BASE
		mov r1, #GPIO_PORT_M
		add r1, r1, r5
		mov r5, #0x7c       ;0x7c = 0111_1100(SW0~4)
		add r1, r1, r5

		ldr r5, [r1]

		sub r5, #SW_LEFT    ;SW3(LED ON/OFF)

		mov r0, #GPIO_BASE
        mov r1, #GPIO_PORT_G
        add r1, r1, r0
        mov r0, #0x10 	;masking for Port G pin2
        add r1, r1, r0

        mov r5, #0x00 	;write to Port G to turn off LED
        str r5, [r1]
        
        bx lr

			.retain
			.retainrefs
