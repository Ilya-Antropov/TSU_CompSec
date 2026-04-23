#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <cmath>

double f(double x) {
    return 1.0 / (std::sqrt(x) * (std::exp(0.9 * x) + 3.0));
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const double a = 0.5;
    const double b = 2.0;
    const long long n = 10000000LL;
    const double h = (b - a) / static_cast<double>(n);

    double t_start = MPI_Wtime();
    long long local_n    = n / size;
    long long remainder  = n % size;

    long long i_start, i_end;
    if (rank < remainder) {
        local_n++;
        i_start = rank * local_n;
    } else {
        i_start = rank * local_n + remainder;
    }
    i_end = i_start + local_n;

    // Метод средних прямоугольников: x_mid = a + (i + 0.5) * h
    double local_sum = 0.0;
    for (long long i = i_start; i < i_end; i++) {
        double x_mid = a + (static_cast<double>(i) + 0.5) * h;
        local_sum += f(x_mid);
    }
    local_sum *= h;

    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    double t_end = MPI_Wtime();
    double elapsed = t_end - t_start;

    if (rank == 0) {
        std::cout << std::fixed << std::setprecision(12);
        std::cout << "  Лабораторная работа №5 — Интегрирование"    << std::endl;
        std::cout << "  Число процессов:  " << size                 << std::endl;
        std::cout << "  Число разбиений:  " << n                    << std::endl;
        std::cout << "  a = " << a << ",  b = " << b                << std::endl;
        std::cout << "---------------------------------------------" << std::endl;
        std::cout << "  Результат интеграла: " << global_sum         << std::endl;
        std::cout << "  Время выполнения:    " << std::setprecision(6)
                  << elapsed << " с"                                 << std::endl;
    }

    MPI_Finalize();
    return 0;
}
