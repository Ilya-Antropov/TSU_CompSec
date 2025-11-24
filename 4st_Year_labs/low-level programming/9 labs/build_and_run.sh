#!/bin/bash

echo "=== Финальная версия компиляции для ARM64 ==="

echo "1. Создание ассемблерного файла для ARM64..."
cat > sort_arm_final.s << 'EOF'
.text
.global _insertion_sort_asm

_insertion_sort_asm:
    mov w2, #1

outer_loop:
    cmp w2, w1
    b.ge end_outer

    ldr w3, [x0, w2, sxtw #2]

    sub w4, w2, #1

inner_loop:
    cmp w4, #0
    b.lt end_inner

    ldr w5, [x0, w4, sxtw #2]
    cmp w5, w3
    b.le end_inner

    add w6, w4, #1
    str w5, [x0, w6, sxtw #2]

    sub w4, w4, #1
    b inner_loop

end_inner:
    add w6, w4, #1
    str w3, [x0, w6, sxtw #2]

    add w2, w2, #1
    b outer_loop

end_outer:
    ret
EOF

echo "2. Компиляция ассемблерного файла..."
clang -c sort_arm_final.s -o sort_arm.o

if [ $? -ne 0 ]; then
    echo "Ошибка компиляции ассемблерного файла!"
    exit 1
fi

echo "3. Компиляция C файла..."
clang -c main.c -o main_arm.o

if [ $? -ne 0 ]; then
    echo "Ошибка компиляции C файла!"
    exit 1
fi

echo "4. Линковка..."
clang main_arm.o sort_arm.o -o sort_comparison_final

if [ $? -ne 0 ]; then
    echo "Ошибка линковки!"
    exit 1
fi

echo "5. Запуск финальной версии..."
echo "=========================================="
./sort_comparison_final