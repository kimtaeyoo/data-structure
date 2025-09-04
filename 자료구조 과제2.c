#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LEN 1024
#define STACK_SIZE 1024

typedef struct {
    char data[STACK_SIZE];
    int top;
} CharStack;

typedef struct {
    double data[STACK_SIZE];
    int top;
} DoubleStack;

void initCharStack(CharStack* s) { s->top = -1; }
int isEmptyCharStack(CharStack* s) { return s->top == -1; }
char peekChar(CharStack* s) { return s->data[s->top]; }
void pushChar(CharStack* s, char c) { s->data[++s->top] = c; }
char popChar(CharStack* s) { return s->data[s->top--]; }

void initDoubleStack(DoubleStack* s) { s->top = -1; }
void pushDouble(DoubleStack* s, double val) { s->data[++s->top] = val; }
double popDouble(DoubleStack* s) { return s->data[s->top--]; }

int precedence(char op) {
    switch (op) {
    case '^': return 3;
    case '*': case '/': return 2;
    case '+': case '-': return 1;
    default: return 0;
    }
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

void infix_to_postfix(const char* infix, char* postfix, size_t max_postfix_len) {
    CharStack s;
    initCharStack(&s);
    char token[128];
    int i = 0, k;
    postfix[0] = '\0';

    // "-(" 로 시작하는 경우만 단항 마이너스로 간주
    if (infix[0] == '-' && infix[1] == '(') {
        strcat(postfix, "0 ");
        pushChar(&s, '-'); // 단항 마이너스를 연산자로 처리
        pushChar(&s, '(');
        i = 2; // "-(" 건너뜀
    }

    while (infix[i]) {
        if (isspace(infix[i])) {
            i++;
            continue;
        }

        // 숫자 또는 소수
        if (isdigit(infix[i]) || infix[i] == '.') {
            k = 0;
            while ((isdigit(infix[i]) || infix[i] == '.' || infix[i] == 'e' || infix[i] == 'E' || infix[i] == 'f') && k < 127)
                token[k++] = infix[i++];
            token[k] = '\0';

            if (strlen(postfix) + strlen(token) + 2 >= max_postfix_len) {
                fprintf(stderr, "Postfix 버퍼 초과\n");
                exit(1);
            }

            strcat(postfix, token);
            strcat(postfix, " ");
        }
        // 음수 리터럴 (예: -1, -3.5)
        else if (infix[i] == '-' && isdigit(infix[i + 1])) {
            k = 0;
            token[k++] = infix[i++];
            while ((isdigit(infix[i]) || infix[i] == '.' || infix[i] == 'e' || infix[i] == 'E' || infix[i] == 'f') && k < 127)
                token[k++] = infix[i++];
            token[k] = '\0';

            strcat(postfix, token);
            strcat(postfix, " ");
        }
        else if (infix[i] == '(') {
            pushChar(&s, infix[i++]);
        }
        else if (infix[i] == ')') {
            while (!isEmptyCharStack(&s) && peekChar(&s) != '(') {
                char op[3] = { popChar(&s), ' ', '\0' };
                strcat(postfix, op);
            }
            if (!isEmptyCharStack(&s) && peekChar(&s) == '(') popChar(&s);
            i++;
        }
        else if (is_operator(infix[i])) {
            while (!isEmptyCharStack(&s) && precedence(peekChar(&s)) >= precedence(infix[i])) {
                char op[3] = { popChar(&s), ' ', '\0' };
                strcat(postfix, op);
            }
            pushChar(&s, infix[i++]);
        }
        else {
            i++; // 알 수 없는 문자 무시
        }
    }

    while (!isEmptyCharStack(&s)) {
        char op[3] = { popChar(&s), ' ', '\0' };
        strcat(postfix, op);
    }
}


double eval_postfix(char* postfix) {
    DoubleStack s;
    initDoubleStack(&s);
    char* token = strtok(postfix, " ");

    while (token) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            pushDouble(&s, atof(token));
        }
        else if (is_operator(token[0]) && strlen(token) == 1) {
            double b = popDouble(&s);
            double a = popDouble(&s);
            switch (token[0]) {
            case '+': pushDouble(&s, a + b); break;
            case '-': pushDouble(&s, a - b); break;
            case '*': pushDouble(&s, a * b); break;
            case '/': pushDouble(&s, a / b); break;
            case '^': pushDouble(&s, pow(a, b)); break;
            }
        }
        token = strtok(NULL, " ");
    }

    return popDouble(&s);
}

int main() {
    FILE* file = fopen("input.txt", "r");
    if (!file) {
        perror("파일 열기 실패");
        return 1;
    }

    int n;
    fscanf(file, "%d\n", &n);

    char infix[MAX_EXPR_LEN], postfix[MAX_EXPR_LEN], postfix_copy[MAX_EXPR_LEN];

    for (int i = 0; i < n; i++) {
        if (!fgets(infix, sizeof(infix), file)) break;
        infix[strcspn(infix, "\n")] = '\0'; // 줄바꿈 제거

        infix_to_postfix(infix, postfix, sizeof(postfix));
        strcpy(postfix_copy, postfix); // 평가용 사본

        double result = eval_postfix(postfix_copy);

        printf("Infix   : %s\n", infix);
        printf("Postfix : %s\n", postfix);
        if (fabs(result - round(result)) < 1e-6)
            printf("Result  : %.0f\n\n", result);
        else
            printf("Result  : %.2f\n\n", result);
    }

    fclose(file);
    return 0;
}












