section .data
    prompt_size db "Enter the number of elements: ", 0
    prompt_element db "Enter element: ", 0
    prompt_sum db "Sum between min and max: ", 0
    newline db 10, 0
    buffer_size equ 12

section .bss
    array resd 100
    buffer resb buffer_size
    size resd 1

section .text
    global _start

_start:
    mov eax, prompt_size
    call print_string
    call read_int
    mov [size], eax
    
    cmp eax, 2
    jl exit_program
    cmp eax, 100
    jg exit_program

    mov ecx, eax 
    mov esi, 0 
input_loop:
    push ecx 
    push esi  
    mov eax, prompt_element
    call print_string 
    call read_int 
    pop esi
    mov [array + esi*4], eax 
    inc esi
    pop ecx
    loop input_loop

    mov ecx, [size]   
	mov esi, 0        
	mov eax, [array]  
	mov ebx, [array]  
	mov edx, 0        
	mov edi, 0        

find_loop:
    mov ebp, [array + esi*4]
    cmp ebp, eax
    jge not_min
    mov eax, ebp 
    mov edx, esi
not_min:
    cmp ebp, ebx
    jle not_max
    mov ebx, ebp 
    mov edi, esi
not_max:
    inc esi
    loop find_loop
    
    cmp edx, edi
    jl min_first
    xchg edx, edi      

min_first:
    mov ecx, edx
    inc ecx 
    mov eax, 0

    mov esi, edi
    sub esi, edx
    cmp esi, 1
    jle sum_done

sum_loop: 
    cmp ecx, edi 
    jge sum_done
    mov ebx, [array + ecx*4]
    add eax, ebx 
    inc ecx
    jmp sum_loop

sum_done:
    push eax
    mov eax, prompt_sum
    call print_string
    pop eax
    call print_int
    mov eax, newline
    call print_string

exit_program:
    mov eax, 1
    xor ebx, ebx
    int 0x80

read_int:
    push ebx
    push ecx
    push edx
    push esi
    mov eax, 3
    mov ebx, 0
    mov ecx, buffer
    mov edx, buffer_size
    int 0x80
    
    mov esi, buffer
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    mov bl, byte [esi]
    cmp bl, '-'
    jne convert_loop
    inc esi
    mov ecx, 1 
    
convert_loop:
    mov bl, byte [esi]
    cmp bl, 10     
    je convert_done
    cmp bl, '0'
    jb convert_done
    cmp bl, '9'
    ja convert_done
    sub bl, '0'
    imul eax, 10
    add eax, ebx
    inc esi
    jmp convert_loop
    
convert_done:
    test ecx, ecx
    jz positive
    neg eax
positive:
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
    push edi
    
    mov ecx, buffer + 11
    mov byte [ecx], 0
    mov ebx, 10
    test eax, eax
    jns print_loop_unsigned
    neg eax
    push eax
    mov eax, 4
    mov ebx, 1
    mov ecx, minus_sign
    mov edx, 1
    int 0x80
    pop eax
    
print_loop_unsigned:
    dec ecx
    xor edx, edx
    div ebx
    add dl, '0'
    mov [ecx], dl
    test eax, eax
    jnz print_loop_unsigned
    
    mov eax, 4
    mov ebx, 1
    mov edx, buffer + 11
    sub edx, ecx
    int 0x80
    
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

print_string:
    push eax
    push ebx
    push ecx
    push edx
    
    mov ecx, eax
    mov edx, 0
str_len_loop:
    cmp byte [eax], 0
    je str_len_done
    inc eax
    inc edx
    jmp str_len_loop
str_len_done:
    mov eax, 4
    mov ebx, 1
    int 0x80
    
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret

section .data
minus_sign db '-', 0