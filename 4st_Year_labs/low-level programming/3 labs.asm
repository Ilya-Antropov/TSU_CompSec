section .data
    prompt1 db "Введите m: ", 0         
    prompt2 db "Введите n: ", 0           
    result_msg db "Общие кратные: ", 0  
    no_multiples_msg db "Нет общих кратных", 0  
    newline db 10, 0              
    space db " ", 0         

section .bss
    m resd 1          
    n resd 1    
    gcd resd 1        
    lcm resd 1        
    current resd 1    
    product resd 1    
    count resd 1      
    buffer resb 12    
    num_buffer resb 12 

section .text
    global _start    

print_string:
    push ebp           
    mov ebp, esp       
    push ebx           
    push ecx
    push edx
    
    mov ecx, [ebp + 8] 
    call strlen        
    mov edx, eax       
    mov eax, 4         
    mov ebx, 1         
    int 0x80           
    
    pop edx            
    pop ecx
    pop ebx
    mov esp, ebp       
    pop ebp            
    ret                

input_number:
    push ebp           
    mov ebp, esp       
    push ebx           
    push ecx
    push edx
    
    mov eax, 3         
    mov ebx, 0         
    mov ecx, buffer    
    mov edx, 12        
    int 0x80           
    
    mov esi, buffer    
    call atoi          
    
    pop edx            
    pop ecx
    pop ebx
    mov esp, ebp       
    pop ebp            
    ret                

strlen:
    push ebx           
    mov ebx, ecx       
    xor eax, eax       
.strlen_loop:
    cmp byte [ebx + eax], 0  
    je .strlen_done    
    inc eax            
    jmp .strlen_loop   
.strlen_done:
    pop ebx            
    ret                

atoi:
    xor eax, eax       
    xor ebx, ebx       
    xor ecx, ecx       
.atoi_loop:
    mov bl, [esi]      
    cmp bl, 10         
    je .atoi_done      
    cmp bl, 13         
    je .atoi_done      
    cmp bl, 0          
    je .atoi_done      
    sub bl, '0'        
    jl .atoi_done      
    cmp bl, 9          
    jg .atoi_done      
    imul eax, 10       
    add eax, ebx       
    inc esi            
    jmp .atoi_loop     
.atoi_done:
    ret                

print_int:
    push ebp           
    mov ebp, esp       
    push eax           
    push ebx
    push ecx
    push edx
    
    mov eax, [ebp + 8] 
    mov edi, num_buffer + 11  
    mov byte [edi], 0  
    mov ebx, 10        
    
.print_int_loop:
    dec edi            
    xor edx, edx       
    div ebx            
    add dl, '0'        
    mov [edi], dl      
    test eax, eax      
    jnz .print_int_loop
    
    push edi           
    call print_string  
    add esp, 4         
    
    pop edx            
    pop ecx
    pop ebx
    pop eax
    mov esp, ebp       
    pop ebp            
    ret                

calculate_gcd:
    push ebp           
    mov ebp, esp       
    push ebx           
    push edx
    
    mov eax, [ebp + 8] 
    mov ebx, [ebp + 12]
    
.gcd_loop:
    test ebx, ebx      
    jz .gcd_done       
    xor edx, edx       
    div ebx            
    mov eax, ebx       
    mov ebx, edx       
    jmp .gcd_loop      
    
.gcd_done:
    pop edx            
    pop ebx
    mov esp, ebp       
    pop ebp            
    ret                

_start:
    push prompt1       
    call print_string  
    add esp, 4         
    
    call input_number  
    mov [m], eax       
    
    push prompt2       
    call print_string  
    add esp, 4         
    
    call input_number  
    mov [n], eax       
    
    mov eax, [m]       
    mul dword [n]      
    mov [product], eax 
    
    push dword [n]     
    push dword [m]     
    call calculate_gcd 
    add esp, 8         
    mov [gcd], eax     
    
    mov eax, [product] 
    div dword [gcd]    
    mov [lcm], eax     
    
    mov dword [count], 0  
    
    push result_msg    
    call print_string  
    add esp, 4         
    
    mov eax, [lcm]     
    mov [current], eax 
    
.find_multiples:
    mov eax, [current] 
    cmp eax, [product] 
    jge .check_count   
    
    push eax           
    call print_int     
    add esp, 4         
    
    push space         
    call print_string  
    add esp, 4         
    
    inc dword [count]  
    
.next_multiple:
    mov eax, [current] 
    add eax, [lcm]     
    mov [current], eax 
    
    jc .check_count    
    
    jmp .find_multiples

.check_count:
    cmp dword [count], 0  
    jne .end_program   
    
    push no_multiples_msg 
    call print_string  
    add esp, 4         
    
.end_program:
    push newline       
    call print_string  
    add esp, 4         
    
    mov eax, 1         
    xor ebx, ebx       
    int 0x80           