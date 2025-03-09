

/* Дана квадратная матрица А. В матрице А заменить 1 столбец на последний, 2 на предпоследний и т.д */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 5 
int main() {
    int A[N][N];
    int B[N][N];
    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = rand() % 10;
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            B[i][j] = A[i][N-j-1];
        }
    }
    printf("Original matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    printf("Changed matrix:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }
    return 0;
}

