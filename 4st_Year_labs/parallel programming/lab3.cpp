#include <mpi.h>
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int m = 4;
    const int n = 4 * size;
    const int rows_per_proc = n / size;

    // 1. Процесс 0 создаёт и выводит глобальную матрицу
    int* A_global = nullptr;

    if (rank == 0) {
        A_global = new int[n * m];
        srand(42);
        for (int i = 0; i < n * m; i++) {
            A_global[i] = (rand() % 19) - 9;
        }

        printf("=== A_global[%d][%d] (заполнена процессом 0) ===\n", n, m);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                printf("%4d", A_global[i * m + j]);
            }
            printf("\n");
        }
        printf("\n");
    }

    //  2. Рассылка строк по процессам (по 4 строки каждому)
    int* A_local = new int[rows_per_proc * m];

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

    // 3. Каждый процесс вычисляет частичную сумму квадратов
    double local_sum_sq = 0.0;
    for (int i = 0; i < rows_per_proc; i++) {
        for (int j = 0; j < m; j++) {
            double val = static_cast<double>(A_local[i * m + j]);
            local_sum_sq += val * val;
        }
    }

    // 4. Глобальная редукция: суммируем квадраты со всех процессов
    double global_sum_sq = 0.0;
    MPI_Allreduce(
        &local_sum_sq,
        &global_sum_sq,
        1,
        MPI_DOUBLE,
        MPI_SUM,
        MPI_COMM_WORLD
    );

    // 5. Каждый процесс самостоятельно вычисляет норму
    double norm = sqrt(global_sum_sq);

    // 6. Вывод результата с каждого процесса
    for (int r = 0; r < size; r++) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == r) {
            printf("Результат на процессе %d\n", rank);
            printf("  Локальные строки [%d..%d], частичная сумма квадратов: %.2f\n",
                   rank * rows_per_proc, (rank + 1) * rows_per_proc - 1, local_sum_sq);
            printf("  Норма Фробениуса матрицы A_global: %.6f\n\n", norm);
        }
    }

    // 7. Проверка корректности: процесс 0 считает норму последовательно
    if (rank == 0) {
        double seq_sum_sq = 0.0;
        for (int i = 0; i < n * m; i++) {
            double v = static_cast<double>(A_global[i]);
            seq_sum_sq += v * v;
        }
        double seq_norm = sqrt(seq_sum_sq);
        printf("=== Проверка (последовательный расчёт на процессе 0) ===\n");
        printf("  Норма (последовательно): %.6f\n", seq_norm);
        printf("  Норма (параллельно):     %.6f\n", norm);
        printf("  Совпадение: %s\n\n",
               (fabs(seq_norm - norm) < 1e-9) ? "ДА ✓" : "НЕТ ✗");

        delete[] A_global;
    }

    delete[] A_local;
    MPI_Finalize();
    return 0;
}
