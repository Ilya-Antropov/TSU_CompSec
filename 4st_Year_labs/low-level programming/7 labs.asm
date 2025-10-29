section .data
    prompt_n      db 'Enter n: ', 0
    prompt_tern   db 'Enter ternary vector: ', 0
    prompt_bool   db 'Enter boolean vector: ', 0
    msg_yes       db 'Vector belongs to the interval.', 0xA, 0
    msg_no        db 'Vector does not belong to the interval.', 0xA, 0
    newline       db 0xA

section .bss
    n         resd 1
    ternary   resb 100
    bool_vec  resb 100
    temp      resb 100

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_n
    mov edx, 9
    int 0x80

    call read_int
    mov [n], eax

    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_tern
    mov edx, 22
    int 0x80

    mov ecx, ternary     
    call read_string

    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_bool
    mov edx, 23
    int 0x80

    mov ecx, bool_vec
    call read_string

    mov ecx, [n]        
    xor ebx, ebx        

check_loop:
    mov al, [ternary + ebx] 
    mov dl, [bool_vec + ebx]

    cmp al, '-'
    je next_char

    cmp al, dl
    jne not_belong

next_char:
    inc ebx
    loop check_loop

    mov eax, 4
    mov ebx, 1
    mov ecx, msg_yes
    mov edx, 28
    int 0x80
    jmp exit

not_belong:
    mov eax, 4
    mov ebx, 1
    mov ecx, msg_no
    mov edx, 41
    int 0x80

exit:
    mov eax, 1
    xor ebx, ebx
    int 0x80

read_int:
    mov eax, 3
    mov ebx, 0
    mov ecx, temp
    mov edx, 10
    int 0x80

    xor eax, eax
    mov ecx, temp
convert_loop:
    movzx edx, byte [ecx]
    cmp dl, 0xA
    je done_convert
    imul eax, 10
    sub edx, '0'
    add eax, edx
    inc ecx
    jmp convert_loop
done_convert:
    ret

read_string:
    push ecx 
    
    mov eax, 3
    mov ebx, 0
    mov ecx, temp
    mov edx, 100
    int 0x80

    pop edi             
    mov esi, temp       
    mov ecx, eax        
    dec ecx             
    
copy_loop:
    mov al, [esi]
    mov [edi], al
    inc esi
    inc edi
    loop copy_loop
    
    mov byte [edi], 0   
    ret