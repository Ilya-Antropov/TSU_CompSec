#include <mpi.h>
#include <cmath>
#include <iostream>
#include <iomanip>

inline double f(double x, double y) {
    return std::abs(x * y);
}

// Проверка принадлежности точки области D: x^4 + y^4 < 1
inline bool inDomain(double x, double y) {
    double x2 = x * x;
    double y2 = y * y;
    return (x2 * x2 + y2 * y2) < 1.0;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Параметры сетки
    // N x N ячеек, итого N^2 = 16 000 000 > 10^7
    const int    N     = 4000;
    const double X_MIN = -1.0, X_MAX = 1.0;
    const double Y_MIN = -1.0, Y_MAX = 1.0;
    const double DX    = (X_MAX - X_MIN) / N;
    const double DY    = (Y_MAX - Y_MIN) / N;
    const double AREA  = DX * DY;

    // Распределение строк (ось X) между процессами
    int base_rows = N / size;
    int remainder = N % size;
    int i_start = rank * base_rows + std::min(rank, remainder);
    int local_rows = base_rows + (rank < remainder ? 1 : 0);
    int i_end = i_start + local_rows;

    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    // Вычисление локальной суммы (метод средней точки)
    double local_sum = 0.0;
    for (int i = i_start; i < i_end; i++) {
        double x = X_MIN + (i + 0.5) * DX;
        for (int j = 0; j < N; j++) {
            double y = Y_MIN + (j + 0.5) * DY;
            if (inDomain(x, y)) {
                local_sum += f(x, y);
            }
        }
    }
    local_sum *= AREA;

    double global_result = 0.0;
    MPI_Reduce(&local_sum, &global_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Замер времени (берём максимальное по всем процессам)
    MPI_Barrier(MPI_COMM_WORLD);
    double t_end    = MPI_Wtime();
    double loc_time = t_end - t_start;
    double max_time = 0.0;
    MPI_Reduce(&loc_time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        const double EXACT = M_PI / 4.0;
        std::cout << std::fixed << std::setprecision(10);
        std::cout << "Число процессов   : " << size << std::endl;
        std::cout << "Сетка (N x N)     : " << N << " x " << N << std::endl;
        std::cout << "Всего ячеек       : " << (long long)N * N << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << "Результат         : " << global_result << std::endl;
        std::cout << "Точное значение   : " << EXACT << std::endl;
        std::cout << "Абс. погрешность  : " << std::abs(global_result - EXACT) << std::endl;
        std::cout << "--------------------------------------------" << std::endl;
        std::cout << std::setprecision(6);
        std::cout << "Время (сек)       : " << max_time << std::endl;
    }

    MPI_Finalize();
    return 0;
}
