section .data
    msg_input    db 'Введите трехзначное число: ', 0
    len_input    equ $ - msg_input
    msg_yes      db 'Одинаковые цифра '
    digit        db '0', 0
    len_yes      equ $ - msg_yes
    msg_no       db 'Нету одинаковых', 0
    len_no       equ $ - msg_no
    newline      db 10

section .bss
    buffer resb 4

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, msg_input
    mov edx, len_input
    int 0x80

    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 4
    int 0x80

    cmp eax, 3
    jl exit

    mov al, [buffer]
    mov bl, [buffer+1]
    mov cl, [buffer+2]

    cmp al, bl
    je found_match

    cmp al, cl
    je found_match

    cmp bl, cl
    je found_match

    mov eax, 4
    mov ebx, 1
    mov ecx, msg_no
    mov edx, len_no
    int 0x80
    jmp exit

found_match:
    mov [digit], al

    mov eax, 4
    mov ebx, 1
    mov ecx, msg_yes
    mov edx, len_yes
    int 0x80

exit:
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80

    mov eax, 1
    mov ebx, 0
    int 0x80