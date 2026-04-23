#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Длина векторов
    long long n = 1000000LL;
    if (argc > 1) {
        n = std::atoll(argv[1]);
    }

    //  Распределение элементов по процессам
    long long remainder  = n % size;
    long long local_n    = n / size + (rank < remainder ? 1 : 0);
    long long offset     = rank * (n / size) + std::min((long long)rank, remainder);

    //  Заполнение локальных векторов единицами
    std::vector<double> x_local(local_n, 1.0);
    std::vector<double> y_local(local_n, 1.0);

    // Синхронизация перед замером времени
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    //  Вычисление локального скалярного произведения
    double local_S = 0.0;
    for (long long i = 0; i < local_n; i++) {
        local_S += x_local[i] * y_local[i];
    }

    //  КАСКАДНАЯ СХЕМА СУММИРОВАНИЯ
    //    Шаг 1: stride = 1
    //      Процессы с нечётным rank/stride → отправляют, выходят из цикла
    //      Процессы с чётным  rank/stride  → принимают от (rank + stride)
    //    Повторяем, удваивая stride, пока stride < size
    //  В итоге rank 0 накапливает полную сумму.
    int step = 1;
    while (step < size)
    {
        if (rank % (2 * step) == 0)
        {
            if (rank + step < size)
            {
                double recv_val = 0.0;
                MPI_Recv(&recv_val, 1, MPI_DOUBLE,
                         rank + step, 0,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                local_S += recv_val;
            }
        }
        else if (rank % (2 * step) == step)
        {
            MPI_Send(&local_S, 1, MPI_DOUBLE,
                     rank - step, 0,
                     MPI_COMM_WORLD);
            break;
        }
        step *= 2;
    }

    double t_end = MPI_Wtime();
    double elapsed = t_end - t_start;

    // Вывод результата на процессе 0
    if (rank == 0)
    {
        double expected = (double)n;
        bool   correct  = (std::fabs(local_S - expected) < 1e-6);

        std::cout << "  Скалярное произведение (MPI + КС)" << std::endl;
        std::cout << "=================================================" << std::endl;
        std::cout << "  Число процессов : " << size     << std::endl;
        std::cout << "  Длина векторов n: " << n        << std::endl;
        std::cout << "  Результат S      = " << local_S  << std::endl;
        std::cout << "  Ожидаемое S      = " << expected << std::endl;
        std::cout << "  Проверка         : " << (correct ? "ВЕРНО ✓" : "ОШИБКА ✗") << std::endl;
        std::cout << "  Время выполнения : " << elapsed << " сек." << std::endl;
        std::cout << "=================================================" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
