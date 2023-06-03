[org 0x7c00]		; Assembly command
					; Let NASM compiler know starting address of memory
					; BIOS reads 1st sector and copied it on memory address 0x7c00
[bits 16] 			; Assembly command
					; Let NASM compiler know that this code consists of 16its

[SECTION .text] 	; Text section

START:				; Boot loader(1st sector) starts
    cli				; Clear interrupt
    xor ax, ax		; Initialize ax register
    mov ds, ax		; Base address of data segment

;----------------------Write your code here------------------------
	mov ax, 0xB800
    mov es, ax                                  ; memory address of printing on screen
	
    mov al, byte [MSG_test]
    mov byte [es : 80*2*0+2*0], al
    mov byte [es : 80*2*0+2*0+1], 0x02
	mov al, byte [MSG_test+1]
    mov byte [es : 80*2*0+2*1], al
    mov byte [es : 80*2*0+2*1+1], 0x02
	mov al, byte [MSG_test+2]
    mov byte [es : 80*2*0+2*2], al
    mov byte [es : 80*2*0+2*2+1], 0x02
	mov al, byte [MSG_test+3]
    mov byte [es : 80*2*0+2*3], al
    mov byte [es : 80*2*0+2*3+1], 0x02
;------------------------------------------------------------------
	sti                                         ; set interrupt
    call load_sectors 		                    ; load rest sectors
    jmp sector_2                                ; jump to sector_2

load_sectors:                                   ; read and copy the rest sectors of disk
	push es                                     ; save es register
    xor ax, ax                                  ; ax = 0x0000
    mov es, ax									; es = 0x0000
	mov bx, sector_2 							; es:bx, Buffer Address Pointer
    mov ah,2 									; Read Sector Mode
    mov al,(sector_end - sector_2)/512 + 1  	; Sectors to Read Count
    mov ch,0 									; Cylinder Number=0
    mov cl,2 									; Sector Number=2
    mov dh,0 									; Head=0
    mov dl,0 									; Drive=0, A:drive
	int 0x13 									; BIOS interrupt
												; Services depend on ah value
    pop es                                      ; restore es register
    ret                                         ; return to caller
	
MSG_test: db'test',0                            ; define string test

times   510-($-$$) db 0                         ; $ : current address, $$ : start address of SECTION
												; $-$$ means the size of source
dw      0xAA55 					                ; signature bytes
												; End of Master Boot Record(1st Sector)
				
sector_2:                                       ; Program Starts
	cli                                         ; clear interrupt
	
	lgdt [gdt_ptr]                              ; Load the address of GDT to GDTR
	
	mov eax, cr0                                ; Control Register 0(CR0) setting
	or eax, 0x01                                ; Set the first bit of CR0 to 1
	mov cr0, eax                                ; Save CR0
	
	mov bx, 0x10                                ; Put the value of Data Segment Selector to bx
	mov ds, bx                                  ; Put the value of bx to Data Segment Selector
	mov es, bx                                  ; Put the value of bx to Extra Segment Selector
	mov fs, bx                                  ; Put the value of bx to FS Segment Selector
	mov gs, bx                                  ; Put the value of bx to GS Segment Selector
	mov ss, bx                                  ; Put the value of bx to Stack Segment Selector
	
	jmp SYS_CODE_SEL_1:Protected_START          ; jump Protected_START
												; Remove prefetch queue
;---------------------------------------------------------------		
Protected_START:                                ; Protected mode starts
[bits 32]			                            ; Assembly command
												; Let NASM compiler know that this code consists of 32its

	;jmp $							            ; jump to current address,
												; infinite loop

;----------------------Write your code here------------------------
	mov ax, VIDEO_SEL                           ; Store the value of Selector which indicates the domain
	mov es, ax                            		; of Video Memory on ES Register
;------------------------------------------------------------------
	call print_protected	
	call print_cs_Protected
	;jmp $							            ; for first problem
												; after finishing making GDT and loading it,
												; remove the remark ';'
;-------------------------write your code here---------------------
	mov ax, LDTR                                ; Put base address of ldt to ldtr descriptor
	lldt ax                                     ; Load ldt
;------------------------------------------------------------------	
	jmp 0x04:LDT0_Start                         ; for second problem
												; after finishing making LDT and loading it,
												; remove the remark ';'

LDT0_Start:
	call print_cs_LDT0_Start	
	jmp $							            ; for second problem
												; after finishing making LDT and loading it,
												; remove the remark ';'	
	
;----------------------------------------------------------------------------	
MSG_Protected_MODE_Test: db'Protectd Mode',0
CS_LDT0_Start: db'CS register of LDT0_Start : ',0
CS_Protected_Start: db'CS register of Protected_Start : ',0
temp: dd 0

printf_s:
	mov cl, byte [ds:eax]
	mov byte [es: edi], cl
	inc edi
	mov byte [es: edi], bl
	inc edi

	inc eax								
	mov cl, byte [ds:eax]
	mov ch, 0
	cmp cl, ch							
	je printf_end						
	jmp printf_s	

printf_end:
	ret
	
printf_n:
	inc eax
	inc eax
	inc eax
	mov bh, 0x01
	jmp printf2
printf2:
	mov cl, byte [ds:eax]
	mov dl, cl
	shr dl, 4
	cmp dl, 0x09
	ja a1
	jmp a2
printf3:
	mov byte [es: edi], dl
	inc edi
	mov byte [es: edi], bl
	inc edi
	mov dl, cl
	and dl, 0x0f
	cmp dl, 0x09
	ja a3
	jmp a4
printf4:
	mov byte [es: edi], dl
	inc edi
	mov byte [es: edi], bl
	inc edi
	
	cmp bh, 0x04
	je printf_end1
	jmp a5

a1 :
	add dl, 0x37
	jmp printf3	
a2 :
	add dl, 0x30
	jmp printf3
a3 :
	add dl, 0x37
	jmp printf4
a4 :
	add dl, 0x30
	jmp printf4
a5 :
	add bh, 0x01
	dec eax
	jmp printf2
printf_end1:
	ret
	
print_protected:
	pushad
	mov eax, MSG_Protected_MODE_Test
	mov edi, 80*2*1+0
	mov bl, 0x02
	call printf_s
	popad
	ret
print_cs_LDT0_Start:
	pushad
	mov eax, CS_LDT0_Start
	mov edi, 80*2*5+0
	mov bl, 0x02
	call printf_s
	mov edi, 80*2*5+2*28					
	mov bl, 0x04
	mov [temp], cs
	mov eax, temp
	call printf_n
	popad
	ret
print_cs_Protected:
	pushad
	mov eax, CS_Protected_Start
	mov edi, 80*2*4+0
	mov bl, 0x02
	call printf_s
	mov edi, 80*2*4+2*33					
	mov bl, 0x04
	mov [temp], cs
	mov eax, temp
	call printf_n
	popad
	ret	
;----------------------------------------------------------------------------		
	
;----------------------Global Description Table-----------------------
gdt:                            ; idx : 0(null descriptor), ti = 0(GDT), rpl = 0(Highest Priv)
	dw 0                        ; limit[15:0]
	dw 0                        ; base[15:0]
	db 0                        ; base[23:16] 
	db 0                        ; P, DPL[1:0], S, type[3:0]
	db 0                        ; G, D/B, L, AVL, limit[19:16] 
	db 0                        ; base [31:24]
	
SYS_CODE_SEL_0 equ 08h          ; idx = 1(Code SD0), ti = 0(GDT), rpl = 0(Highest Priv) 
	dw 0xFFFF                   ; limit[15:0]
	dw 0x0000                   ; base[15:0]
	db 0x00                     ; base[23:16] 
	db 0x9A                     ; P, DPL[1:0], S, type[3:0]
	db 0xCF                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]

SYS_DATA_SEL equ 10h            ; idx = 2(Data), ti = 0(GDT), rpl = 0(Highest Priv)
	dw 0xFFFF                   ; limit[15:0]
	dw 0x0000                   ; base[15:0]
	db 0x00                     ; base[23:16] 
	db 0x92                     ; P, DPL[1:0], S, type[3:0]
	db 0xCF                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]

VIDEO_SEL equ 18h               ; idx = 3(Video), ti = 0(GDT), rpl = 0(Highest Priv)
	dw 0xFFFF                   ; limit[15:0]
	dw 0x8000                   ; base[15:0]
	db 0x0B                     ; base[23:16] 
	db 0x92                     ; P, DPL[1:0], S, type[3:0]
	db 0x40                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]

LDTR equ 20h                    ; idx = 4(LDT), ti = 0(GDT), rpl = 0(Highest Priv)
	dw 0xFFFF                   ; limit[15:0]
	dw ldt                      ; base[15:0]
	db 0x00                     ; base[23:16] 
	db 0x82                     ; P, DPL[1:0], S, type[3:0]
	db 0x40                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]

SYS_CODE_SEL_1 equ 28h          ; idx = 5(Code SD1), ti = 0(GDT), rpl = 0(Highest Priv)
	dw 0xFFFF                   ; limit[15:0]
	dw 0x0000                   ; base[15:0]
	db 0x00                     ; base[23:16] 
	db 0x9A                     ; P, DPL[1:0], S, type[3:0]
	db 0xCF                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]
;------------------------------------------------------------------

gdt_end:

;----------------------Write your code here------------------------
gdt_ptr:
	dw gdt_end - gdt - 1        ; top ot GDT(get_end - 1) - very bottom(gdt) / 16-bit GDT table limit
	dd gdt                      ; store base address of GDT
;------------------------------------------------------------------



;---------------------------Local Description Table-----------------------

;-------------------------write your code here---------------------
ldt:                            ; Code Segment Descriptor
LDT_CODE_SEL_0 equ 00h           ; idx = 1(Code SD0), ti = 0(GDT), rpl = 0(Highest Priv)                            
	dw 0xFFFF                   ; limit[15:0]
	dw 0x0000                   ; base[15:0]
	db 0x00                     ; base[23:16] 
	db 0x9A                     ; P, DPL[1:0], S, type[3:0]
	db 0xCF                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]

LDT_DATA_SEL_0 equ 08h          ; idx = 1(Code SD0), ti = 0(GDT), rpl = 0(Highest Priv) 
	dw 0xFFFF                   ; limit[15:0]
	dw 0x0000                   ; base[15:0]
	db 0x00                     ; base[23:16] 
	db 0x92                     ; P, DPL[1:0], S, type[3:0]
	db 0xCF                     ; G, D/B, L, AVL, limit[19:16] 
	db 0x00                     ; base [31:24]
;------------------------------------------------------------------

ldt_end:

sector_end:

