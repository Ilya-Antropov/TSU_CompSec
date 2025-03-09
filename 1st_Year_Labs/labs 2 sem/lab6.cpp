#include <stdio.h>
#include <string.h>

int line_search(char *str, int str_len, char *sub_str, int sub_str_len)
{
    for (int i = 0; i <= str_len - sub_str_len; i++) {
        int flag = 1;
        for (int j = i; j < i + sub_str_len && flag; j++) {
            if (str[j] != sub_str[j - i]) {
                flag = 0;
            }
        }
        if (flag) {
            return i;
        }
    }
    return -1;
}
int main()
{
    char str[100];
    char sub_str[100];
    printf("Введите строку: ");
    fgets(str, 100, stdin);

    printf("Введите подстроку: ");
    fgets(sub_str, 100, stdin);

    int str_len = strlen(str) - 1; 
    int sub_str_len = strlen(sub_str) - 1;
    sub_str[sub_str_len] = '\0';

    int flag = line_search(str, str_len, sub_str, sub_str_len);
    if (flag != -1) {
        printf("Подстрока найдена в позиции %d\n", flag);
    } else {
        printf("Не найдено");
    }

    return 0;
}
