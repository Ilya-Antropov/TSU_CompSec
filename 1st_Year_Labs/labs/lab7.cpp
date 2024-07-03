#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_STACK_SIZE 100

typedef struct {
    int top;
    double* items;
} Stack;

void initializeStack(Stack* stack, int size) {
    stack->top = -1;
    stack->items = (double*)malloc(size * sizeof(double));
}

int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

double performOperation(double operand1, double operand2, char znak) {
    switch (znak) {
            case '+': return operand1 + operand2;
            case '-': return operand1 - operand2;
            case '*': return operand1 * operand2;
            case '/': return operand1 / operand2;
            default: return 0.0;
    }
}

double evaluateReversePolish(char* expression) {
    Stack stack;
    initializeStack(&stack, MAX_STACK_SIZE);

    int i = 0;
    while (expression[i] != '\0') {
        if (isdigit(expression[i]) || (expression[i] == '-' && (i == 0 || expression[i-1] == '('))) {
            double operand = atof(&expression[i]);
            while (isdigit(expression[i]) || expression[i] == '.' || expression[i] == '-') {
                i++;
            }
            stack.items[++stack.top] = operand;
        } else if (isOperator(expression[i])) {
            double operand2 = stack.items[stack.top--];
            double operand1 = stack.items[stack.top--];
            double result = performOperation(operand1, operand2, expression[i]);
            stack.items[++stack.top] = result;
            i++;
        } else if (expression[i] == '(') {
            stack.items[++stack.top] = expression[i];
            i++;
        } else if (expression[i] == ')') {
            if (stack.top < 1 || stack.items[stack.top] == '(') {
                printf("Ошибка: некорректное количество открывающих и закрывающих скобок\n");
                exit(EXIT_FAILURE);
            }
            double operand2 = stack.items[stack.top--];
            double operand1 = stack.items[stack.top--];
            stack.top--;
            double result = performOperation(operand1, operand2, '(');
            stack.items[++stack.top] = result;
            i++;
        } else if (isspace(expression[i])) {
            i++;
        } else {
            printf("Ошибка: некорректный символ в выражении\n");
            exit(EXIT_FAILURE);
        }
    }

    if (stack.top != 0) {
        printf("Ошибка: некорректное количество операторов и операндов\n");
        exit(EXIT_FAILURE);
    }

    double result = stack.items[stack.top];
    free(stack.items);
    return result;
}

int main() {
    char expression[100];
    printf("Введите выражение в обратной польской записи: ");
    fgets(expression, sizeof(expression), stdin);

    if (expression[strlen(expression) - 1] == '\n') {
        expression[strlen(expression) - 1] = '\0';
    }

    double result = evaluateReversePolish(expression);
    printf("Результат вычисления: %.2f\n", result);

    return 0;
}
