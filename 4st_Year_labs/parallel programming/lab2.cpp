#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // n = 4*size строк, m = 5 столбцов
    int n = 4 * size;
    int m = 5;
    int rows_per_proc = 4;

    int* A_global = nullptr;
    int* A_local = new int[rows_per_proc * m];

    // ШАГ 1: Процесс 0 заполняет матрицу случайными числами из [-9, 9]
    if (rank == 0) {
        A_global = new int[n * m];
        srand(42);

        std::cout << "=== A_global[" << n << "][" << m << "] (заполнена процессом 0) ===" << std::endl;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                A_global[i * m + j] = (rand() % 19) - 9;
                std::cout << A_global[i * m + j];
                if (j < m - 1) std::cout << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // ШАГ 2: Распределение матрицы по процессам (каждый получает 4 строки)
    MPI_Scatter(
        A_global,
        rows_per_proc * m,
        MPI_INT,
        A_local,
        rows_per_proc * m,
        MPI_INT,
        0,
        MPI_COMM_WORLD
    );

    // ШАГ 3: Каждый процесс ищет локальный максимум в своих 4 строках
    int local_max = INT_MIN;
    for (int i = 0; i < rows_per_proc * m; i++) {
        if (A_local[i] > local_max)
            local_max = A_local[i];
    }

    // ШАГ 4: Глобальный максимум собирается на процессе 1 (или 0 при size==1)
    int global_max = INT_MIN;
    int root = (size > 1) ? 1 : 0;

    MPI_Reduce(
        &local_max,
        &global_max,
        1,
        MPI_INT,
        MPI_MAX,
        root,
        MPI_COMM_WORLD
    );

    // ШАГ 5: Вывод результата на процессе с номером 1
    if (rank == root) {
        if (size == 1) {
            std::cout << "Результат на процессе 0 (запуск с 1 процессом, rank 1 отсутствует)" << std::endl;
        } else {
            std::cout << "Результат на процессе 1" << std::endl;
        }
        std::cout << "Глобальный максимальный элемент матрицы A_global: " << global_max << std::endl;
    }

    delete[] A_local;
    if (rank == 0 && A_global != nullptr)
        delete[] A_global;

    MPI_Finalize();
    return 0;
}
