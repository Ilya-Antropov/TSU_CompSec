section .data
    prompt_a    db 'Vvedite chislo a: ', 0
    prompt_b    db 'Vvedite chislo b: ', 0  
    prompt_c    db 'Vvedite chislo c: ', 0
    sorted_msg  db 'Otsortirovannye chisla: ', 0
    result_msg  db 'Posle vychitania: ', 0
    newline     db 10, 0
    space       db ' ', 0
    buffer      times 16 db 0
    minus       db '-', 0

section .bss
    num1        resd 1
    num2        resd 1  
    num3        resd 1

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_a
    mov edx, 18
    int 0x80
    call read_int
    mov [num1], eax

    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_b
    mov edx, 18
    int 0x80
    call read_int
    mov [num2], eax

    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_c
    mov edx, 18
    int 0x80
    call read_int
    mov [num3], eax

    call sort_numbers

    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80
    
    mov eax, 4
    mov ebx, 1
    mov ecx, sorted_msg
    mov edx, 22
    int 0x80
    mov eax, [num1]
    call print_int
    
    mov eax, 4
    mov ebx, 1
    mov ecx, space
    mov edx, 1
    int 0x80
    mov eax, [num2]
    call print_int
    
    mov eax, 4
    mov ebx, 1
    mov ecx, space
    mov edx, 1
    int 0x80
    mov eax, [num3]
    call print_int
    
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80

    mov eax, [num3]
    mov ebx, [num1]
    sub ebx, eax
    mov [num1], ebx
    mov ebx, [num2]
    sub ebx, eax
    mov [num2], ebx

    mov eax, 4
    mov ebx, 1
    mov ecx, result_msg
    mov edx, 19
    int 0x80
    mov eax, [num1]
    call print_int
    
    mov eax, 4
    mov ebx, 1
    mov ecx, space
    mov edx, 1
    int 0x80
    mov eax, [num2]
    call print_int
    
    mov eax, 4
    mov ebx, 1
    mov ecx, space
    mov edx, 1
    int 0x80
    mov eax, [num3]
    call print_int
    
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80
    
    mov eax, 1
    xor ebx, ebx
    int 0x80

sort_numbers:
    push eax
    push ebx
    push ecx
    
    mov eax, [num1]
    mov ebx, [num2]
    cmp eax, ebx
    jle .compare2
    mov [num1], ebx
    mov [num2], eax

.compare2:
    mov eax, [num2]
    mov ebx, [num3]
    cmp eax, ebx
    jle .compare3
    mov [num2], ebx
    mov [num3], eax

.compare3:
    mov eax, [num1]
    mov ebx, [num2]
    cmp eax, ebx
    jle .done
    mov [num1], ebx
    mov [num2], eax

.done:
    pop ecx
    pop ebx
    pop eax
    ret

read_int:
    push ebx
    push ecx
    push edx
    push esi
    
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, 15
    int 0x80
    
    mov esi, buffer
    xor eax, eax
    xor ecx, ecx
    xor ebx, ebx
    xor edx, edx
    
    mov cl, [esi]
    cmp cl, '-'
    jne .parse_digits
    inc esi
    mov ebx, 1

.parse_digits:
    mov cl, [esi]
    inc esi
    cmp cl, 10
    je .done_parse
    cmp cl, 13
    je .done_parse
    cmp cl, 0
    je .done_parse
    sub cl, '0'
    imul eax, 10
    add eax, ecx
    jmp .parse_digits

.done_parse:
    test ebx, ebx
    jz .positive
    neg eax

.positive:
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

print_int:
    push eax
    push ebx
    push ecx
    push edx
    push esi
    
    test eax, eax
    jns .positive
    push eax
    mov eax, 4
    mov ebx, 1
    mov ecx, minus
    mov edx, 1
    int 0x80
    pop eax
    neg eax

.positive:
    mov esi, buffer + 15
    mov byte [esi], 0
    mov ebx, 10
    
.convert_loop:
    dec esi
    xor edx, edx
    div ebx
    add dl, '0'
    mov [esi], dl
    test eax, eax
    jnz .convert_loop
    
    mov ecx, esi
    mov edx, buffer + 16
    sub edx, ecx
    mov eax, 4
    mov ebx, 1
    int 0x80
    
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

section .data
minus db '-', 0