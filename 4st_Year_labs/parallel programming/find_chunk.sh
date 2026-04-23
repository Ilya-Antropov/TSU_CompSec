#!/bin/bash
# Скрипт для подбора оптимального chunk_size для директивы dynamic
# Запуск: bash find_chunk.sh <num_threads>

#SBATCH --job-name lab1_chunk
#SBATCH --output lab1_chunk_%j.out
#SBATCH --time 00:20:00
#SBATCH --cpus-per-task 32
#SBATCH --mem-per-cpu 200M

THREADS=${1:-4}
export OMP_NUM_THREADS=$THREADS

echo "Поиск оптимального chunk_size"
echo "Потоков: $THREADS"

# Проверяем наличие исполняемого файла
if [ ! -f ./lab7 ]; then
    echo "Файл lab7 не найден. Компилируем..."
    g++ -O2 -fopenmp -o lab1 lab1.cpp
fi

CHUNKS=(1 2 4 8 12 16 20 24 32)

echo ""
echo "chunk_size | Время i-k-j (с) | Время k-i-j (с)"
echo "-----------|-----------------|----------------"

for CHUNK in "${CHUNKS[@]}"; do
    OUTPUT=$(./lab1 $THREADS $CHUNK 2>/dev/null | grep "^ikj_time")
    T_IKJ=$(echo $OUTPUT | grep -oP 'ikj_time=\K[0-9.]+')
    T_KIJ=$(echo $OUTPUT | grep -oP 'kij_time=\K[0-9.]+')
    printf "%10d | %15.4f | %15.4f\n" $CHUNK $T_IKJ $T_KIJ
done
