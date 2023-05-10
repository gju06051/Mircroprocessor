[org 0x7c00]		; Assembly command
					; Let NASM compiler know starting address of memory
					; BIOS reads 1st sector and copied it on memory address 0x7c00
[bits 16] 			; Assembly command
					; Let NASM compiler know that this code consists of 16its

[SECTION .text] 	; Text section

START:				; Boot loader(1st sector) starts
    cli				; Clear interrupt
    xor ax, ax		; Initialize ax register
	mov ax, 0x8FF
	mov ds, ax		; Set data segment register
	mov bx, 0x00
	mov al, 0x01

;-----------Following code is for filling some values in the memory-------------;
mem:																		
	mov byte [ds:bx], al
	cmp bx, 0xFF
	je test_end
	jmp re

re:
	add al, 0x02
	add bx, 0x01
	jmp mem
	
test_end:
	cli
	xor ax, ax
	mov ds, ax
    mov ax, 0xB800
    mov es, ax 
	
;-------------------------------------------------------------------------------;

	sti						; Set interrupt
    call load_sectors 		; Load rest sectors
    jmp sector_2

load_sectors:			 	; Read and copy the rest sectors of disk

	push es
    xor ax, ax
    mov es, ax									; es=0x0000
	mov bx, sector_2 							; es:bx, Buffer Address Pointer
    mov ah, 2 									; Read Sector Mode
    mov al, (sector_end - sector_2)/512 + 1  	; Sectors to Read Count
    mov ch, 0 									; Cylinder Number=0
    mov cl, 2 									; Sector Number=2
    mov dh, 0 									; Head=0
    mov dl, 0 									; Drive=0, A:drive
	int 0x13 									; BIOS interrupt
												; Services depend on ah value
    pop es
    ret

times   510-($-$$) db 0 		; $ : current address, $$ : start address of SECTION
								; $-$$ means the size of source
dw      0xAA55 					; signature bytes
								; End of Master Boot Record(1st Sector)
								
		

sector_2:						; Program Starts
	mov ax, 0x8FF
	mov ss, ax
	mov sp, 0x10
	mov ax, 0x1234
	push ax
	mov bx, 0x8FFC
	mov dl, byte [ds:bx]
	add ah, dl
	xchg al, bh
	mov bx, 0x8FFD
	mov word[ds:bx], ax
	sub al, ah
	mov bx, 0x8FFF
	mov byte[ds:bx], al

;-------------------------Write your code here----------------------------------;	
; Print your Name in VMware screen											    ;
; Print your ID in VMware screen											    ;
; Print the value(word size) in the Stack Pointer after executing the above code;
;-------------------------------------------------------------------------------;

ID_SET:
	mov esi, ID                 ; bring ID string to esi register
	mov edi, 160                ; print position offset(print start with second line, 80 * 2 * line_num() + 2 * 0 = 160)
	mov ebx, 0                  ; string index offset +2, +4, +6
	mov edx, 12                 ; light red (8'b0000_1100)

PRINT_ID:
	mov eax, 0                  ; reset eax(=ah-background color, al-character color)
	mov al, [si+bx]             ; one byte of the string character(First Character 'I')
	mov ah, dl                  ; color setting(high 4-bit for background : 4'b0000, low 4-bit for character color : 4'b1100)
	
	mov word [es:di], ax        ; print one byte of the string with character property(ah : color, al : character) -> little endian
	
	add ebx, 1                  ; update string index offset(Second Character 'D')
	add edi, 2                  ; update print position offset
	
	cmp al, 0                   ; check if the string is end(string last value is null)
	jne PRINT_ID                ; If string not end, print next character

NAME_SET:
	mov esi, NAMEE
	mov edi, 320                ; print position offset(print start with third line, 80 * 2 * line_num(=2) + 2 * 0 = 320)
	mov ebx, 0                  ; string index offset +2, +4, +6
	mov edx, 14                 ; yello (8'b0000_1110)

PRINT_NAME:
	mov eax, 0                  ; reset eax(=ah-background color, al-character color)
	mov al, [si+bx]             ; one byte of the string character
	mov ah, dl                  ; color setting(high 4-bit for background : 4'b0000, low 4-bit for character color : 4'b1110)
	
	mov word [es:di], ax        ; print one byte of the string with character property(ah : color, al : character) -> little endian
	
	add ebx, 1                  ; update string index offset
	add edi, 2                  ; update print position offset
	
	cmp al, 0                   ; check if the string is end(string last value is null)
	jne PRINT_NAME              ; If string not end, print next character

ANSWER_SET:
	mov esi, Answer
	mov edi, 480                ; print position offset(print start with third line, 80 * 2 * line_num(=3) + 2 * 0 = 480)
	mov ebx, 0                  ; string index offset +2, +4, +6
	mov edx, 14                 ; yello (8'b0000_1110)
	mov ecx, 0                  ; reset ecx

PRINT_ANSWER:
	mov eax, 0                  ; reset eax(=ah-background color, al-character color)
	mov al, [si+bx]             ; one byte of the string character
	mov ah, dl                  ; color setting(high 4-bit for background : 4'b0000, low 4-bit for character color : 4'b1110)
	
	mov word [es:di], ax        ; print one byte of the string with character property(ah : color, al : character) -> little endian
	
	add ebx, 1                  ; update string index offset
	add edi, 2                  ; update print position offset
	
	cmp al, 0                   ; check if the string is end(string last value is null)
	jne PRINT_ANSWER            ; If string not end, print next character

	pop bx                      ; pop stack value to bx
	
STACK_VAL:
	mov ax, 0xF000              ; mask for MSB 4-bit of bx (ex. 16'b1234_5678_4321_8765)
	and ax, bx                  ; get MSB 4-bit of bx (16'b1234_0000_0000_0000)
	shr ax, 12                  ; shift right 12-bit to get MSB 4-bit of bx (16'b0000_0000_0000_1234)
	cmp al, 0xA                 ; compare with ASCII 10 (ASCII div with 10)
	jae ABOVE_TEN               ; if above or equal 10, jump to ABOVE_TEN
	add al, 0x30                ; if below 10, add 0x30 to get ASCII value

PRINT_STACK:
	mov ah, dl                  ; color setting(high 4-bit for background : 4'b0000, low 4-bit for character color : 4'b1110), 
								; al alread have data
	mov word [es:di], ax        ; print one byte of the string
	add edi, 2                  ; update print position offset
	add ecx, 1                  ; update counter (for 4 numbers print)
	
	shl bx, 4                   ; shift left 4-bit to get next 4-bit of bx
	cmp ecx, 4                  ; check if the string is end(4 things to print)
	je exit
	jmp STACK_VAL
	
ABOVE_TEN:
	add al, 0x37                ; if above or equal 10, add 0x37 to get ASCII value
	jmp PRINT_STACK             ; jump to PRINT_STACK

exit:

;---------------------- Write your Name and ID here-----------------------------;

ID  db 'ID : 2018312959',0
NAMEE db 'NAME : Choi Bo Yul',0
Answer db 'A value in Stack Pointer(word size) : ',0

;-------------------------------------------------------------------------------;

sector_end:

