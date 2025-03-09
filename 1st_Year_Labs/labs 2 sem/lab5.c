/* A={a,b,c}. Преобразовать слово P так, чтобы сначала шли все символы a,
затем – все символы b и в конце – все символы c. */

#include <stdio.h>
#include <stdlib.h>

char *get_string(int *len) {
    *len = 0;
    int size = 1;
    char *s = (char *) malloc(sizeof(char));
    char c = getchar();
    while (c != '\n') {
        s[(*len)++] = c;
       
        if (*len >= size) {
            size *= 2;
            s = (char *) realloc(s, size * sizeof(char));
        }
        c = getchar();
    }
    s[*len] = '\0';
    return s;
}

int main() {
    int len;
    char *s = get_string(&len);    \
    for (int i = 0; i < len-1; ++i) {
        if (s[i] == 'b' && s[i + 1] == 'a') {
            char temp = s[i];
            s[i] = s[i + 1];
            s[i + 1] = temp;
            i = -1;  
        } else if (s[i] == 'c' && s[i + 1] == 'b') {
            char temp = s[i];
            s[i] = s[i + 1];
            s[i + 1] = temp;
            i = -1;
        } else if (s[i] == 'c' && s[i + 1] == 'a') {
            char temp = s[i];
            s[i] = s[i + 1];
            s[i + 1] = temp;
            i = -1;
        }

    }
    printf(s);
    free(s);
    return 0;
}
