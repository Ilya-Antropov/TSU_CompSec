#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern void insertion_sort_asm(int *arr, int n);

void insertion_sort_c(int *arr, int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

int is_sorted(int *arr, int n) {
    for (int i = 0; i < n - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;
        }
    }
    return 1;
}

void print_array(int *arr, int n, const char *label) {
    printf("%s: ", label);
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
        if (i > 0 && i % 10 == 0) printf("\n");
    }
    printf("\n");
}

void copy_array(int *src, int *dest, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

void print_time(double seconds, const char *label) {
    if (seconds >= 1.0) {
        printf("%s: %.4f секунд\n", label, seconds);
    } else if (seconds >= 0.001) {
        printf("%s: %.4f миллисекунд\n", label, seconds * 1000.0);
    } else if (seconds >= 0.000001) {
        printf("%s: %.4f микросекунд\n", label, seconds * 1000000.0);
    } else {
        printf("%s: %.4f наносекунд\n", label, seconds * 1000000000.0);
    }
}

int main() {
    int n;
    printf("Введите количество элементов: ");
    scanf("%d", &n);

    if (n <= 0) {
        printf("Количество элементов должно быть положительным\n");
        return 1;
    }

    int *arr_original = malloc(n * sizeof(int));
    int *arr_c = malloc(n * sizeof(int));
    int *arr_asm = malloc(n * sizeof(int));

    if (arr_original == NULL || arr_c == NULL || arr_asm == NULL) {
        printf("Ошибка выделения памяти\n");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < n; i++) {
        arr_original[i] = rand() % 1000;
    }

    copy_array(arr_original, arr_c, n);
    copy_array(arr_original, arr_asm, n);

    printf("Сортировка массива из %d элементов...\n", n);

    if (n <= 50) {
        print_array(arr_original, n, "Исходный массив");
    }

    int repetitions = 1;
    if (n < 1000) {
        repetitions = 1000;
    } else if (n < 10000) {
        repetitions = 100;
    } else {
        repetitions = 10;
    }

    printf("Выполняем %d повторений для точного замера времени...\n", repetitions);

    clock_t start_c = clock();
    for (int i = 0; i < repetitions; i++) {
        copy_array(arr_original, arr_c, n);
        insertion_sort_c(arr_c, n);
    }
    clock_t end_c = clock();
    double total_time_c = ((double)(end_c - start_c)) / CLOCKS_PER_SEC;
    double time_c = total_time_c / repetitions;

    clock_t start_asm = clock();
    for (int i = 0; i < repetitions; i++) {
        copy_array(arr_original, arr_asm, n);
        insertion_sort_asm(arr_asm, n);
    }
    clock_t end_asm = clock();
    double total_time_asm = ((double)(end_asm - start_asm)) / CLOCKS_PER_SEC;
    double time_asm = total_time_asm / repetitions;

    int sorted_c = is_sorted(arr_c, n);
    int sorted_asm = is_sorted(arr_asm, n);

    if (n <= 50) {
        print_array(arr_c, n, "После сортировки C");
        print_array(arr_asm, n, "После сортировки ASM");
    }

    printf("\n=== РЕЗУЛЬТАТЫ ===\n");
    printf("Сортировка на C:    %s\n", sorted_c ? "успешно" : "неудача");
    print_time(time_c, "Время одного выполнения");
    printf("Сортировка на ASM:  %s\n", sorted_asm ? "успешно" : "неудача");
    print_time(time_asm, "Время одного выполнения");

    if (time_asm > 0) {
        printf("Отношение времени C/ASM: %.2f\n", time_c / time_asm);
        if (time_c / time_asm > 1.0) {
            printf("Ассемблерная версия быстрее в %.2f раз\n", time_c / time_asm);
        } else {
            printf("C версия быстрее в %.2f раз\n", time_asm / time_c);
        }
    } else {
        printf("Время выполнения ASM слишком мало для сравнения\n");
    }

    printf("\n=== ДОПОЛНИТЕЛЬНАЯ ИНФОРМАЦИЯ ===\n");
    printf("Размер массива: %d элементов\n", n);
    printf("Количество повторений для замера: %d\n", repetitions);
    print_time(total_time_c, "Общее время выполнения C");
    print_time(total_time_asm, "Общее время выполнения ASM");

    free(arr_original);
    free(arr_c);
    free(arr_asm);

    return 0;
}