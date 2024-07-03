

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int countDigits(char* str) {
    int count = 0;
    while (*str) {
        if (isdigit(*str)) {
            count++;
        }
        str++;
    }
    return count;
}
int main() {
    int n;
    printf("Введите количество строк: ");
    scanf("%d", &n);
    char** arr = (char**) malloc(n * sizeof(char*)); 
    for (int i = 0; i < n; i++) {
        arr[i] = (char*) malloc(100 * sizeof(char));
        printf("Строка #%d: ", i+1);
        scanf("%s", arr[i]);
    }
    int* digitsCount = (int*) malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        digitsCount[i] = countDigits(arr[i]);
    }
    int maxDigits = digitsCount[0];
    for (int i = 1; i < n; i++) {
        if (digitsCount[i] > maxDigits) {
            maxDigits = digitsCount[i];
        }
    }
    printf("Строка с максимальным количеством цифр: \n");
    for (int i = 0; i < n; i++) {
        if (digitsCount[i] == maxDigits) {
            printf("%s\n", arr[i]);
            free(arr[i]);
        }
    }
    printf("Остальные строки: \n");
    for (int i = 0; i < n; i++) {
        if (digitsCount[i] != maxDigits) {
            printf("%s\n", arr[i]);
            free(arr[i]);
        }
    }
    free(arr);
    free(digitsCount);
}




