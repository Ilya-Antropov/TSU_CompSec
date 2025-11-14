section .data
    prompt_n db "Enter n: ", 0             
    len_prompt_n equ $ - prompt_n   
    prompt_element db "Enter element: ", 0 
    len_prompt_element equ $ - prompt_element 
    output_sum db "Sum: ", 0                
    len_output_sum equ $ - output_sum
    output_matrix db "Matrix:", 10, 0      
    len_output_matrix equ $ - output_matrix 
    space db " ", 0                         
    len_space equ $ - space        
    newline db 10, 0                       
    len_newline equ 1                    

section .bss
    n resd 1                 
    matrix resd 100          
    temp resd 1              
    num_buffer resb 12       
    input_buffer resb 12     

section .text
    global _start               

_start:
    mov eax, 4                 
    mov ebx, 1                  
    mov ecx, prompt_n          
    mov edx, len_prompt_n    
    int 0x80                  
    
    call read_int   
    mov [n], eax               

    mov ecx, 0                  
i_loop:
    cmp ecx, [n]               
    jge input_done             
    
    mov edx, 0                  
j_loop:
    push ecx                
    push edx                
    
    mov eax, 4                  
    mov ebx, 1                  
    mov ecx, prompt_element     
    mov edx, len_prompt_element 
    int 0x80                 
    
    call read_int            
    mov [temp], eax            
    
    pop edx             
    pop ecx             
    
    mov eax, ecx               
    imul eax, [n]              
    add eax, edx               
    shl eax, 2                 
    
    mov ebx, eax               
    mov eax, [temp]            
    mov [matrix + ebx], eax    
    
    inc edx                 
    cmp edx, [n]            
    jl j_loop               
    
    inc ecx                   
    jmp i_loop                

input_done:
    mov eax, 4                
    mov ebx, 1                 
    mov ecx, output_matrix
    mov edx, len_output_matrix 
    int 0x80                  
    
    call print_matrix        

    mov ecx, 0    
    mov esi, 0    
    
    mov edi, [n]            
    dec edi                 
    
sum_i_loop:
    cmp ecx, [n]           
    jge sum_done           
    mov edx, 0             
sum_j_loop:
    mov eax, ecx           
    add eax, eax           
    sub eax, edi           
    call abs_value         
    mov ebx, eax           
    
    mov eax, edx              
    add eax, eax              
    sub eax, edi              
    call abs_value            
    
    add eax, ebx           
    cmp eax, edi           
    jg skip_element        
    
    mov eax, ecx              
    imul eax, [n]             
    add eax, edx              
    shl eax, 2                
    
    mov ebx, [matrix + eax]    
    add esi, ebx               

skip_element:
    inc edx                 
    cmp edx, [n]            
    jl sum_j_loop           
    inc ecx                 
    jmp sum_i_loop          

sum_done:
    mov eax, 4               
    mov ebx, 1                
    mov ecx, output_sum 
    mov edx, len_output_sum 
    int 0x80                   
    
    mov eax, esi                
    call print_int              
    
    mov eax, 4                
    mov ebx, 1                
    mov ecx, newline          
    mov edx, len_newline      
    int 0x80                  
    
    mov eax, 1    
    mov ebx, 0    
    int 0x80      

print_matrix:
    push ecx                 
    push edx
    push eax
    push ebx
    mov ecx, 0 
print_i_loop:
    cmp ecx, [n]              
    jge print_matrix_done     
    
    mov edx, 0                
print_j_loop:
    mov eax, ecx               
    imul eax, [n]              
    add eax, edx               
    shl eax, 2                 
    
    mov eax, [matrix + eax]    
    call print_int              
    
    push ecx
    push edx
    mov eax, 4                  
    mov ebx, 1                  
    mov ecx, space              
    mov edx, len_space          
    int 0x80                    
    pop edx
    pop ecx
    
    inc edx                    
    cmp edx, [n]               
    jl print_j_loop            
    
    push ecx
    push edx
    mov eax, 4                 
    mov ebx, 1                 
    mov ecx, newline           
    mov edx, len_newline       
    int 0x80                   
    pop edx
    pop ecx
    
    inc ecx                   
    jmp print_i_loop          

print_matrix_done:
    pop ebx  
    pop eax
    pop edx
    pop ecx
    ret           

abs_value:
    cmp eax, 0                 
    jge abs_done               
    neg eax                    
abs_done:
    ret                      

read_int:
    push ebx           
    push ecx
    push edx
    
    mov eax, 3               
    mov ebx, 0               
    mov ecx, input_buffer    
    mov edx, 12              
    int 0x80                 
    
    mov ecx, input_buffer   
    xor eax, eax            
    xor ebx, ebx            
convert_loop:
    mov bl, [ecx]              
    cmp bl, 10                 
    je convert_done            
    cmp bl, '0'                
    jb convert_done            
    cmp bl, '9'                
    ja convert_done            
    sub bl, '0'                
    imul eax, 10               
    add eax, ebx               
    inc ecx                    
    jmp convert_loop           
convert_done:
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
    
    mov edi, num_buffer + 11    
    mov byte [edi], 0           
    mov ebx, 10                 
    
    test eax, eax               
    jnz .non_zero               
    dec edi                     
    mov byte [edi], '0'         
    jmp .print                  
    
.non_zero:
    xor edx, edx                
    div ebx                     
    add dl, '0'                 
    dec edi                     
    mov [edi], dl               
    test eax, eax               
    jnz .non_zero               
    
.print:
    mov ecx, edi                
    mov edx, num_buffer + 12    
    sub edx, ecx                
    mov eax, 4
    mov ebx, 1 
    int 0x80     
    
    pop edi  
    pop edx
    pop ecx
    pop ebx
    pop eax
    ret