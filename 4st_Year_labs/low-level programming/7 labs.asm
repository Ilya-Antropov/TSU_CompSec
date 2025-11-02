section .data
    prompt_ternary db "Enter ternary vector (32 symbols, 0/1/2): ", 0
    prompt_bool    db "Enter boolean vector (32 symbols, 0/1): ", 0
    yes_msg        db "Vector belongs to the interval", 0xA, 0
    no_msg         db "Vector does not belong to the interval", 0xA, 0
    newline        db 0xA

section .bss
    ternary resb 33
    bool_str resb 33

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_ternary
    mov edx, 38
    int 0x80

    mov eax, 3
    mov ebx, 0
    mov ecx, ternary
    mov edx, 33
    int 0x80

    mov eax, 4
    mov ebx, 1
    mov ecx, prompt_bool
    mov edx, 35
    int 0x80

    mov eax, 3
    mov ebx, 0
    mov ecx, bool_str
    mov edx, 33
    int 0x80

    xor edi, edi       
    xor esi, esi       
    mov ecx, 0         

convert_ternary:
    mov al, [ternary + ecx] 
    cmp al, '0'
    je set_mask
    cmp al, '1'
    je set_both
    cmp al, '2'
    je next_ternary 
    jmp check_done

set_mask:
    bts edi, ecx    
    jmp next_ternary

set_both:
    bts edi, ecx
    bts esi, ecx

next_ternary:
    inc ecx
    cmp ecx, 32
    jl convert_ternary

check_done:
    xor ebx, ebx
    mov ecx, 0

convert_bool:
    mov al, [bool_str + ecx]
    cmp al, '1'
    jne skip_set
    bts ebx, ecx    

skip_set:
    inc ecx
    cmp ecx, 32
    jl convert_bool


    mov eax, ebx
    and eax, edi        
    xor eax, esi        
    test eax, edi       
    jnz not_belongs

belongs: 
    mov eax, 4
    mov ebx, 1
    mov ecx, yes_msg
    mov edx, 28
    int 0x80
    jmp exit

not_belongs:
    mov eax, 4
    mov ebx, 1
    mov ecx, no_msg
    mov edx, 36
    int 0x80

exit:
    mov eax, 1
    xor ebx, ebx
    int 0x80