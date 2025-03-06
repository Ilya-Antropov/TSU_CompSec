#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(){
    srand(time(NULL));
    int A[10];
    for(int i=0; i<10; i++) {
        A[i] = rand() % 20 - 10;
        }

    int *pA = A;
    int count = 0;
    for(int i=1; i<10; i+=2) { /
        if (*(pA + i) < 0) {
            count++;
        }
    }
    printf("Количество отрицательных элементов с нечетными индексами: %d\n", count); 
    return 0;
}






