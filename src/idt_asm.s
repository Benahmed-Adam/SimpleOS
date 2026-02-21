[bits 32]
section .text
global load_idt

load_idt:
    mov    eax, [esp + 4]
    lidt   [eax]
    ret

;; num %1
%macro isr_noerr 1
global isr%1
isr%1:
    cli
    push 0
    push %1
    jmp isr_stub
%endmacro


%macro isr_err 1
global isr%1
isr%1:
    cli
    push %1
    jmp isr_stub
%endmacro

isr_noerr 0
isr_noerr 1
isr_noerr 2
isr_noerr 3
isr_noerr 4
isr_noerr 5
isr_noerr 6
isr_noerr 7
isr_err 8
isr_noerr 9
isr_err 10
isr_err 11
isr_err 12
isr_err 13
isr_err 14
isr_noerr 15
isr_noerr 16
isr_noerr 17
isr_noerr 18
isr_noerr 19
isr_noerr 20
isr_noerr 21
isr_noerr 22
isr_noerr 23
isr_noerr 24
isr_noerr 25
isr_noerr 26
isr_noerr 27
isr_noerr 28
isr_noerr 29
isr_noerr 30
isr_noerr 31

extern isr_handler
isr_stub:
    pusha           ; Pousse EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX

    push ds         ; Sauvegarde les segments proprement
    push es
    push fs
    push gs

    mov ax, 0x10    ; Charge les segments de données du noyau
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Pousse le pointeur vers la structure registers_t
    call isr_handler
    mov esp, eax    ; IMPORTANT : eax doit pointer sur une structure registers_t valide

    pop gs          ; Restaure dans l'ordre INVERSE
    pop fs
    pop es
    pop ds

    popa
    add esp, 8      ; Nettoie le code d'erreur et le numéro d'interruption
    iretd           ; Utilise iretd pour le mode 32 bits

;; num %1 map %2
%macro irq 2
global irq%1
irq%1:
    cli
    push    0
    push    %2
    jmp     irq_stub
%endmacro

irq 0, 32
irq 1, 33
irq 2, 34
irq 3, 35
irq 4, 36
irq 5, 37
irq 6, 38
irq 7, 39
irq 8, 40
irq 9, 41
irq 10, 42
irq 11, 43
irq 12, 44
irq 13, 45
irq 14, 46
irq 15, 47

extern irq_handler
irq_stub:
    pusha

    push ds
    push es
    push fs
    push gs

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push esp
    cli
    call irq_handler
    sti
    mov esp, eax

    pop gs
    pop fs
    pop es
    pop ds

    popa
    add esp, 8
    iretd