#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TERMS 100
#define MAX_LINE 256

typedef struct {
    int coef;
    int exp;
} Term;

typedef struct {
    Term terms[MAX_TERMS];
    int num_terms;
} Polynomial;

void add_term(Polynomial* poly, int coef, int exp) {
    if (coef == 0) return; // 계수가 0이면 추가하지 않음
    for (int i = 0; i < poly->num_terms; i++) {
        if (poly->terms[i].exp == exp) {
            poly->terms[i].coef += coef;
            return;
        }
    }
    poly->terms[poly->num_terms].coef = coef;
    poly->terms[poly->num_terms].exp = exp;
    poly->num_terms++;
}

void parse_polynomial(char* line, Polynomial* poly) {
    poly->num_terms = 0;
    int i = 0, len = strlen(line);
    int sign = 1;

    while (i < len) {
        while (i < len && line[i] == ' ') i++;
        if (i >= len) break;

        if (line[i] == '+') {
            sign = 1;
            i++;
        }
        else if (line[i] == '-') {
            sign = -1;
            i++;
        }

        while (i < len && line[i] == ' ') i++;
        if (i >= len) break;

        int coef = 0, exp = 0;
        int j = i;

        while (j < len && line[j] != ' ' && line[j] != '+' && line[j] != '-') j++;

        char term[50] = { 0 };
        strncpy(term, line + i, j - i);
        term[j - i] = '\0';

        char* x_pos = strstr(term, "x");
        if (x_pos == NULL) {
            sscanf(term, "%d", &coef);
            exp = 0;
        }
        else {
            if (x_pos == term) {
                coef = 1;
            }
            else if (x_pos == term + 1 && term[0] == '-') {
                coef = -1;
            }
            else {
                char coef_str[20] = { 0 };
                strncpy(coef_str, term, x_pos - term);
                sscanf(coef_str, "%d", &coef);
            }

            char* exp_pos = strstr(term, "^");
            if (!exp_pos) exp_pos = strstr(term, "**");

            if (exp_pos) {
                sscanf(exp_pos + (exp_pos[1] == '*' ? 2 : 1), "%d", &exp);
            }
            else {
                exp = 1;
            }
        }

        add_term(poly, sign * coef, exp);
        sign = 1;
        i = j;
    }
}

void print_polynomial(Polynomial* poly) {
    // 지수 내림차순으로 정렬
    for (int i = 0; i < poly->num_terms - 1; i++) {
        for (int j = i + 1; j < poly->num_terms; j++) {
            if (poly->terms[i].exp < poly->terms[j].exp) {
                Term temp = poly->terms[i];
                poly->terms[i] = poly->terms[j];
                poly->terms[j] = temp;
            }
        }
    }

    int first = 1;
    for (int i = 0; i < poly->num_terms; i++) {
        int coef = poly->terms[i].coef;
        int exp = poly->terms[i].exp;

        if (coef == 0) continue;

        if (first) {
            if (coef < 0) printf("-");
            if (abs(coef) != 1 || exp == 0) printf("%d", abs(coef));
            first = 0;
        }
        else {
            printf(" %c ", coef > 0 ? '+' : '-');
            if (abs(coef) != 1 || exp == 0) printf("%d", abs(coef));
        }

        if (exp > 0) {
            printf("x");
            if (exp > 1) printf("^%d", exp);
        }
    }
    if (first) printf("0");
}

// 다항식 덧셈
void add_polynomials(Polynomial* poly1, Polynomial* poly2, Polynomial* result) {
    result->num_terms = 0;
    for (int i = 0; i < poly1->num_terms; i++) {
        add_term(result, poly1->terms[i].coef, poly1->terms[i].exp);
    }
    for (int i = 0; i < poly2->num_terms; i++) {
        add_term(result, poly2->terms[i].coef, poly2->terms[i].exp);
    }
}

// 다항식 곱셈
void multiply_polynomials(Polynomial* poly1, Polynomial* poly2, Polynomial* result) {
    result->num_terms = 0;
    for (int i = 0; i < poly1->num_terms; i++) {
        for (int j = 0; j < poly2->num_terms; j++) {
            int coef = poly1->terms[i].coef * poly2->terms[j].coef;
            int exp = poly1->terms[i].exp + poly2->terms[j].exp;
            add_term(result, coef, exp);
        }
    }
}

int main() {
    FILE* fp = fopen("input.txt", "r");
    if (fp == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 1;
    }

    char line[MAX_LINE];
    fgets(line, MAX_LINE, fp);
    int num_pairs = atoi(line);

    Polynomial poly1, poly2, sum, product;
    int pair_count = 0;

    while (fgets(line, MAX_LINE, fp) && pair_count < num_pairs) {
        parse_polynomial(line, &poly1);

        if (fgets(line, MAX_LINE, fp)) {
            parse_polynomial(line, &poly2);
            pair_count++;

            // 덧셈과 곱셈 계산
            add_polynomials(&poly1, &poly2, &sum);
            multiply_polynomials(&poly1, &poly2, &product);

            // 출력
            printf("== %d번째다항식쌍==\n", pair_count);
            printf("첫번째: ");
            print_polynomial(&poly1);
            printf("\n두번째: ");
            print_polynomial(&poly2);
            printf("\n덧셈결과: ");
            print_polynomial(&sum);
            printf("\n곱셈결과: ");
            print_polynomial(&product);
            printf("\n\n");
        }
    }

    fclose(fp);
    return 0;
}