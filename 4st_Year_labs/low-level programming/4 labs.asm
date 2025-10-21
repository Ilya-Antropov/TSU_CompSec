section .data
    prompt db "Enter n: ", 0
    prompt_len equ $ - prompt
    newline db 10
    space db ' '

section .bss
    input_buffer resb 10
    output_buffer resb 10
    n resd 1

section .text
    global _start

_start:
    mov eax, 4
    mov ebx, 1
    mov ecx, prompt
    mov edx, prompt_len
    int 0x80

    mov eax, 3
    mov ebx, 0
    mov ecx, input_buffer
    mov edx, 10
    int 0x80

    mov esi, input_buffer
    xor eax, eax
    xor ebx, ebx
convert_input:
    mov bl, [esi]
    cmp bl, 10      
    je done_convert
    sub bl, '0'
    imul eax, 10
    add eax, ebx
    inc esi
    jmp convert_input

done_convert:
    mov [n], eax    

    mov esi, 2      
outer_loop:
    mov eax, esi
    cmp eax, [n]
    ja end_outer   

    mov edi, esi    
    shr edi, 1      
    mov ecx, 2      

inner_loop:
    cmp ecx, edi
    ja prime        

    mov eax, esi
    xor edx, edx
    div ecx        
    test edx, edx
    jz not_prime    
    inc ecx
    jmp inner_loop

prime:
    mov eax, esi
    call print_number

not_prime:
    inc esi
    jmp outer_loop

end_outer:
    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80

    mov eax, 1
    mov ebx, 0
    int 0x80

print_number:
    pusha
    mov ebx, 10
    mov edi, output_buffer + 9
    mov byte [edi], 0

convert_loop:
    dec edi
    xor edx, edx
    div ebx
    add dl, '0'
    mov [edi], dl
    test eax, eax
    jnz convert_loop

    ; Вывод числа
    mov eax, 4
    mov ebx, 1
    mov ecx, edi
    mov edx, output_buffer + 10
    sub edx, edi
    int 0x80

    ; Вывод пробела
    mov eax, 4
    mov ebx, 1
    mov ecx, space
    mov edx, 1
    int 0x80

    popa
    ret