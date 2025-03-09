#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

bool isPalindrome(unsigned int num);

int main() {
    unsigned int x;
    printf("Enter a positive integer: ");
    scanf("%u", &x);
    if (isPalindrome(x)) {
        printf("%u is palindrome \n", x);
    } else {
        printf("%u is not palindrome \n", x);
    }
    return 0;
}

bool isPalindrome(unsigned int num) {
    int size = (int)log2(num) + 1;
    bool *bits = (bool*)malloc(size * sizeof(bool));
    memset(bits, 0, size * sizeof(bool));
    int i = 0;
    while (num > 0) {
        bits[i] = num % 2;
        num = num / 2;
        i++;
    }
    for (int j = 0; j < size / 2; j++) {
        if (bits[j] != bits[size - j - 1]) {
            free(bits);
            return false;
        }
    }
    free(bits);
    return true;
}


// Является ли число X палиндромом в двоичной системе счисления.
