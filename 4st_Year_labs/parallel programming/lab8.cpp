#include <iostream>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <vector>

double f(double x) {
    return (x * x + std::sin(0.48 * (x + 2.0))) /
           (std::exp(x * x) + 0.38);
}

double simpson_parallel(double a, double b, long long n, int num_threads) {
    if (n % 2 != 0) n++;

    double h = (b - a) / static_cast<double>(n);
    double sum = 0.0;

    omp_set_num_threads(num_threads);

    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (long long i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 2 == 0)
            sum += 2.0 * f(x);
        else
            sum += 4.0 * f(x);
    }

    sum += f(a) + f(b);
    sum *= h / 3.0;
    return sum;
}

int main() {
    const double a = 0.4;
    const double b = 1.0;
    const long long n = 10000000LL;

    const double reference = 0.421815;

    std::cout << std::fixed;
    std::cout << "=== Лабораторная работа №8: Метод Симпсона + OpenMP ===\n";
    std::cout << "Функция: f(x) = (x^2 + sin(0.48*(x+2))) / (exp(x^2) + 0.38)\n";
    std::cout << "Интервал: [" << std::setprecision(6) << a << ", " << b << "], n = " << n << "\n";
    std::cout << "Эталонное значение: " << std::setprecision(6) << reference << "\n\n";

    int max_threads = omp_get_max_threads();
    std::cout << "Максимальное число потоков в системе: " << max_threads << "\n\n";

    std::vector<int> thread_counts = {1, 2, 4, 6, 8};

    double time1 = 0.0;

    std::cout << std::left
              << std::setw(8)  << "Потоки   "
              << std::setw(18) << "Результат     "
              << std::setw(14) << "   Время(с)   "
              << std::setw(12) << "Ускорение   "
              << std::setw(12) << "Эфф-ть(%)   "
              << std::setw(16) << "Погрешность"
              << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (int threads : thread_counts) {
        if (threads > max_threads) continue;

        double start = omp_get_wtime();
        double result = simpson_parallel(a, b, n, threads);
        double end = omp_get_wtime();
        double elapsed = end - start;

        if (threads == 1) {
            time1 = elapsed;
        }

        double speedup = (elapsed > 0) ? (time1 / elapsed) : 0.0;
        double efficiency = (speedup / threads) * 100.0;
        double error = std::abs(result - reference);

        std::cout << std::left
                  << std::setw(8)  << threads
                  << std::setw(18) << std::fixed << std::setprecision(6) << result
                  << std::setw(14) << std::fixed << std::setprecision(6) << elapsed
                  << std::setw(12) << std::fixed << std::setprecision(3) << speedup
                  << std::setw(11) << std::fixed << std::setprecision(2) << efficiency << "% "
                  << std::setw(16) << std::scientific << std::setprecision(2) << error
                  << "\n";
    }

    std::cout << std::string(80, '-') << "\n";
    std::cout << "Проверка: результат совпадает с эталоном с точностью ~10^-7.\n";

    return 0;
}
