
#include <stdio.h>
#include <stdlib.h>
int main() {
    int n, count = 0;
    printf("Enter a number: ");
    scanf("%d", &n);
    // Создаем массив для хранения количества вхождений каждой цифры
    int *digits = (int *) calloc(10, sizeof(int));
    // Пока число не равно 0, получаем последнюю цифру и увеличиваем соответствующий ей элемент в массиве
    while (n != 0) {
        int digit = n % 10;
        digits[digit]++;
        n /= 10;
    }
    // Подсчитываем количество ненулевых элементов в массиве
    for (int i = 0; i < 10; i++) {
        if (digits[i] != 0) {
            count++;
        }
    }
    printf("Number of different digits: %d", count);
    free(digits); // Освобождаем память, выделенную под массив
    return 0;
}
