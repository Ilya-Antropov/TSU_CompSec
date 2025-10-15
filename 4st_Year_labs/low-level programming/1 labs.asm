section .data
    prompt db "Введите N: ", 0
    prompt_len equ $ - prompt
    result db "Секунд с начала последней минуты: ", 0
    result_len equ $ - result
    newline db 10

section .bss
    input resb 10
    output resb 2

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
    mov ecx, input
    mov edx, 10
    int 0x80

    mov esi, input      
    xor eax, eax       
    xor ebx, ebx       

convert_loop:
    mov bl, [esi]     
    cmp bl, 10        
    je done_convert   
    sub bl, '0'       
    imul eax, 10      
    add eax, ebx      
    inc esi           
    jmp convert_loop  

done_convert:
    xor edx, edx        
    mov ecx, 60
    div ecx             

    mov eax, edx
    xor edx, edx
    mov ecx, 10
    div ecx            
    add al, '0'         
    add dl, '0'         

    mov [output], al    
    mov [output+1], dl  

    mov eax, 4
    mov ebx, 1
    mov ecx, result
    mov edx, result_len
    int 0x80

    mov eax, 4
    mov ebx, 1
    mov ecx, output
    mov edx, 2
    int 0x80

    mov eax, 4
    mov ebx, 1
    mov ecx, newline
    mov edx, 1
    int 0x80

    mov eax, 1
    xor ebx, ebx
    int 0x80