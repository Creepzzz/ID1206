;ASSEMBLER NOTATION
;operation destination, source



;PROGRAM STARTS
bits 16

start:
	mov ax, 0x07C0		; 0x07c00 is were we are
	add ax, 0x20		; add 0x20 (when shifted 512)
	mov ss, ax		; set the stack segment
	mov sp, 0x1000		; set the stack pointer
	
	mov ax, 0x07C0		; set data segment... 
	mov ds, ax		; more about this later
	
	mov si, msg		; pointer to the message in reg. SI
	mov ah, 0x0E		; print char BIOS procedure
	
				; -------
				; SI = Source Index
				; -------
				
;LOOP THROUGH THE MSG VARIABLE
.next:
	lodsb			; load next byte to reg. AL, increment SI
	cmp al, 0		; if the byte is zero
	je .done		; 	jump to done
	int 0x10		; invoke the BIOS system call
	jmp .next		; loop

;EXITING LOOPING THROUGH MSG VARIABLE
.done:
	jmp $			; loop forever

;DEFINE MSG VARIABLE CONTEXT
msg:	db 'Hello', 0		; variable (string) to print

;COMPOSE MASTER BOOT RECORD (512 bits)
	times 510-($-$$) db 0	; fill up to 510 bytes
	dw 0xAA55		; master boot record signature
	
	




