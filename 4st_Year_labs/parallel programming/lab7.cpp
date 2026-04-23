#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <omp.h>


const int N = 512;
// Минимальное количество запусков для усреднения
const int RUNS = 3;


static double A[N * N];
static double B[N * N];
static double C[N * N];
static double C_ref[N * N];

// Инициализация матриц случайными числами
void init_matrices() {
    srand(42);
    for (int i = 0; i < N * N; i++) {
        A[i] = (double)(rand() % 100) / 10.0;
        B[i] = (double)(rand() % 100) / 10.0;
    }
}

void reset_C() {
    memset(C, 0, sizeof(C));
}

//  Порядок i-k-j
double multiply_ikj(int chunk) {
    reset_C();
    double t_min = 1e18;

    for (int run = 0; run < RUNS; run++) {
        reset_C();
        double t_start = omp_get_wtime();

        #pragma omp parallel for schedule(dynamic, chunk) default(none) shared(A, B, C) firstprivate(chunk)
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < N; k++) {
                double a_ik = A[i * N + k];
                for (int j = 0; j < N; j++) {
                    C[i * N + j] += a_ik * B[k * N + j];
                }
            }
        }

        double elapsed = omp_get_wtime() - t_start;
        if (elapsed < t_min) t_min = elapsed;
    }
    return t_min;
}

// Порядок k-i-j
double multiply_kij(int chunk) {
    reset_C();
    double t_min = 1e18;

    for (int run = 0; run < RUNS; run++) {
        reset_C();
        double t_start = omp_get_wtime();

        #pragma omp parallel default(none) shared(A, B, C) firstprivate(chunk)
        {
            for (int k = 0; k < N; k++) {
                #pragma omp for schedule(dynamic, chunk)
                for (int i = 0; i < N; i++) {
                    double a_ik = A[i * N + k];
                    for (int j = 0; j < N; j++) {
                        C[i * N + j] += a_ik * B[k * N + j];
                    }
                }
            }
        }

        double elapsed = omp_get_wtime() - t_start;
        if (elapsed < t_min) t_min = elapsed;
    }
    return t_min;
}

void multiply_seq_ref() {
    memset(C_ref, 0, sizeof(C_ref));
    for (int i = 0; i < N; i++)
        for (int k = 0; k < N; k++)
            for (int j = 0; j < N; j++)
                C_ref[i * N + j] += A[i * N + k] * B[k * N + j];
}

bool verify(const char* label) {
    for (int i = 0; i < N * N; i++) {
        if (fabs(C[i] - C_ref[i]) > 1e-6) {
            std::cerr << "[FAIL] " << label << ": Некорректный результат на элементе "
                      << i << " (C=" << C[i] << " ref=" << C_ref[i] << ")" << std::endl;
            return false;
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    int num_threads = 1;
    int chunk_size  = 1;

    if (argc > 1) num_threads = atoi(argv[1]);
    if (argc > 2) chunk_size  = atoi(argv[2]);

    omp_set_num_threads(num_threads);

    std::cout << "Параметры запуска:" << std::endl;
    std::cout << "  Размер матрицы : " << N << " x " << N << std::endl;
    std::cout << "  Число потоков  : " << num_threads << std::endl;
    std::cout << "  chunk_size     : " << chunk_size << std::endl;
    std::cout << "  Запусков (min) : " << RUNS << std::endl;

    init_matrices();

    //  Проверка корректности (только при 1 потоке или первом запуске)
    if (num_threads == 1) {
        multiply_seq_ref();
    }

    //  i-k-j
    double t_ikj = multiply_ikj(chunk_size);
    bool ok_ikj = true;
    if (num_threads == 1) ok_ikj = verify("i-k-j");

    //  k-i-j
    double t_kij = multiply_kij(chunk_size);
    bool ok_kij = true;
    if (num_threads == 1) ok_kij = verify("k-i-j");

    std::cout << std::endl;
    std::cout << "RESULTS threads=" << num_threads
              << " chunk=" << chunk_size << std::endl;
    std::cout << "ikj_time=" << t_ikj
              << " kij_time=" << t_kij << std::endl;

    if (num_threads == 1 && (!ok_ikj || !ok_kij)) {
        std::cerr << "ОШИБКА: Проверка корректности не пройдена!" << std::endl;
        return 1;
    }
    if (num_threads == 1) {
        std::cout << "Проверка корректности: OK" << std::endl;
    }

    return 0;
}
