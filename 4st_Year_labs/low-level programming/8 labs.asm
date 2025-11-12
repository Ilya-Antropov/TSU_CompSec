section .data
    ; Данные для первой задачи
    x_start    dd 0.1
    x_end      dd 1.0
    step       dd 0.05
    two        dd 2.0
    one        dd 1.0
    hundred    dd 100.0

    ; Данные для второй задачи
    x0         dd 0.5
    epsilon    dd 1.0e-6

    sum        dd 0.0
    term       dd 0.0
    sign       dd 1
    n          dd 1

    msg_table  db "Table of values for y=(1+2x^2)e^(x^2):", 0x0A, 0
    msg_table_header db "x    y", 0x0A, 0
    msg_calc   db 0x0A, "Calculation of ln(1+x) for x=0.5:", 0x0A, 0
    msg_series db "Series: ", 0
    msg_std    db "Standard: ", 0
    msg_diff   db "Difference: ", 0
    newline    db 0x0A, 0
    space      db "    ", 0

    buffer     times 64 db 0
    int_buf    times 16 db 0
    temp       dd 0
    current_x  dd 0.0
    current_y  dd 0.0

section .text
global _start

SYS_WRITE equ 4
SYS_EXIT  equ 1
STDOUT    equ 1

print_string:
    pusha
    mov ecx, esi
    call strlen
    mov edx, eax
    mov ebx, STDOUT
    mov eax, SYS_WRITE
    int 0x80
    popa
    ret

strlen:
    push esi
    xor eax, eax
.count_loop:
    cmp byte [esi+eax], 0
    je .done
    inc eax
    jmp .count_loop
.done:
    pop esi
    ret

int_to_string:
    pusha
    mov ebx, 10
    mov ecx, int_buf + 15
    mov byte [ecx], 0
    dec ecx
    test eax, eax
    jnz .convert
    mov byte [ecx], '0'
    jmp .copy
.convert:
    xor edx, edx
    div ebx
    add dl, '0'
    mov [ecx], dl
    dec ecx
    test eax, eax
    jnz .convert
.copy:
    inc ecx
    mov esi, ecx
.copy_loop:
    mov al, [esi]
    test al, al
    jz .done
    mov [edi], al
    inc esi
    inc edi
    jmp .copy_loop
.done:
    mov byte [edi], 0
    popa
    ret

print_float:
    pusha
    fst dword [temp]
    fabs
    fmul dword [hundred]
    frndint
    fist dword [buffer]
    fld dword [temp]
    mov eax, [buffer]
    mov ecx, 100
    xor edx, edx
    div ecx
    push edx
    mov edi, buffer
    call int_to_string
    mov esi, buffer
    call print_string
    pop edx
    mov byte [buffer], '.'
    mov esi, buffer
    call print_string
    mov eax, edx
    cmp eax, 10
    jae .two_digits
    mov byte [buffer], '0'
    mov esi, buffer
    call print_string
    mov eax, edx
    mov edi, buffer
    call int_to_string
    mov esi, buffer
    call print_string
    jmp .done
.two_digits:
    mov eax, edx
    mov edi, buffer
    call int_to_string
    mov esi, buffer
    call print_string
.done:
    popa
    ret

compute_exp:
    fldl2e
    fmulp st1, st0
    fld st0
    frndint
    fsub st1, st0
    fxch st1
    f2xm1
    fld1
    faddp st1, st0
    fscale
    fstp st1
    ret

_start:
    call task1_fixed
    call task2_fixed
    mov eax, SYS_EXIT
    xor ebx, ebx
    int 0x80

task1_fixed:
    mov esi, msg_table
    call print_string
    mov esi, msg_table_header
    call print_string
    finit
    fld dword [x_start]
    fstp dword [current_x]
    mov ecx, 0
.loop:
    inc ecx
    finit
    fld dword [current_x]
    fcomp dword [x_end]
    fstsw ax
    sahf
    ja .done
    finit
    fld dword [current_x]
    fld st0
    fmul st0, st0
    fld st0
    call compute_exp
    fld1
    fld st2
    fmul dword [two]
    faddp st1, st0
    fmulp st1, st0
    fstp dword [current_y]
    ffree st0
    ffree st1
    finit
    fld dword [current_x]
    call print_float
    mov esi, space
    call print_string
    finit
    fld dword [current_y]
    call print_float
    mov esi, newline
    call print_string
    finit
    fld dword [current_x]
    fadd dword [step]
    fstp dword [current_x]
    cmp ecx, 50
    jb .loop
.done:
    ret

task2_fixed:
    mov esi, msg_calc
    call print_string
    finit
    fldz
    fstp dword [sum]
    mov dword [sign], 1
    mov dword [n], 1
    fld dword [x0]
    fstp dword [term]
    mov ecx, 1000
.loop:
    finit
    fld dword [term]
    fimul dword [sign]
    fidiv dword [n]
    fadd dword [sum]
    fstp dword [sum]
    finit
    fld dword [term]
    fmul dword [x0]
    fstp dword [term]
    mov eax, [sign]
    neg eax
    mov [sign], eax
    inc dword [n]
    finit
    fld dword [term]
    fabs
    fcomp dword [epsilon]
    fstsw ax
    sahf
    jb .done
    dec ecx
    jnz .loop
.done:
    mov esi, msg_series
    call print_string
    finit
    fld dword [sum]
    call print_float
    mov esi, newline
    call print_string
    finit
    fld1
    fadd dword [x0]
    fldln2
    fxch
    fyl2x
    fst dword [temp]
    mov esi, msg_std
    call print_string
    finit
    fld dword [temp]
    call print_float
    mov esi, newline
    call print_string
    mov esi, msg_diff
    call print_string
    finit
    fld dword [temp]
    fsub dword [sum]
    fabs
    call print_float
    mov esi, newline
    call print_string
    ret