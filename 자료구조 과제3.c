#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <fcntl.h>
#include <io.h>

#define MAX_NODES 100
#define MAX_NAME  30

typedef struct Node {
    wchar_t name[MAX_NAME];
    struct Node* parent;
    struct Node* children[MAX_NODES];
    int child_count;
} Node;

Node* nodes[MAX_NODES];
int node_count = 0;
Node* root = NULL;

Node* find_or_create_node(const wchar_t* name) {
    for (int i = 0; i < node_count; i++) {
        if (wcscmp(nodes[i]->name, name) == 0)
            return nodes[i];
    }
    Node* new_node = (Node*)malloc(sizeof(Node));
    wcscpy(new_node->name, name);
    new_node->parent = NULL;
    new_node->child_count = 0;
    nodes[node_count++] = new_node;
    return new_node;
}

void add_relation(const wchar_t* child_name, const wchar_t* parent_name) {
    Node* child = find_or_create_node(child_name);
    Node* parent = find_or_create_node(parent_name);
    child->parent = parent;
    parent->children[parent->child_count++] = child;
}

void print_tree() {
    wprintf(L"조선 왕조 트리 (이름: 자식들):\n\n");

    for (int i = 0; i < node_count; i++) {
        Node* node = nodes[i];
        wprintf(L"%ls:", node->name);
        for (int j = 0; j < node->child_count; j++) {
            wprintf(L" %ls", node->children[j]->name);
        }
        wprintf(L"\n");
    }
}

// 1. 조선의 왕을 순서대로 출력
void print_in_order(Node* node) {
    if (!node) return;
    wprintf(L"%ls ", node->name);
    for (int i = 0; i < node->child_count; i++) {
        print_in_order(node->children[i]);
    }
}

// 2. 조선의 왕을 역순으로 출력
void print_reverse_order(Node* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        print_reverse_order(node->children[i]);
    }
    wprintf(L"%ls ", node->name);
}

// 3. 조선의 왕 수
int count_kings() {
    return node_count;
}

// 4. 특정 왕의 후손들 출력 (예: 인조)
void print_descendants(Node* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        wprintf(L"%ls ", node->children[i]->name);
        print_descendants(node->children[i]);
    }
}

// 5. 직계후손이 왕이 되지 못한 왕 (leaf 노드)
void print_no_successor_kings() {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]->child_count == 0) {
            wprintf(L"%ls ", nodes[i]->name);
        }
    }
}

// 6. 직계 후손 왕이 가장 많은 왕
void print_most_children_king() {
    int max = 0;
    Node* result = NULL;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]->child_count > max) {
            max = nodes[i]->child_count;
            result = nodes[i];
        }
    }
    if (result) wprintf(L"%ls (%d명)", result->name, max);
}

// 7. 정종의 형제로 왕이 된 사람
void print_sibling_kings(const wchar_t* name) {
    Node* target = NULL;
    for (int i = 0; i < node_count; i++) {
        if (wcscmp(nodes[i]->name, name) == 0) {
            target = nodes[i];
            break;
        }
    }
    if (target && target->parent) {
        for (int i = 0; i < target->parent->child_count; i++) {
            Node* sibling = target->parent->children[i];
            if (wcscmp(sibling->name, name) != 0) {
                wprintf(L"%ls ", sibling->name);
            }
        }
    }
}

// 8. 특정 왕의 직계 선조 (예: 순종)
void print_ancestors(const wchar_t* name) {
    Node* node = NULL;
    for (int i = 0; i < node_count; i++) {
        if (wcscmp(nodes[i]->name, name) == 0) {
            node = nodes[i];
            break;
        }
    }
    while (node && node->parent) {
        wprintf(L"%ls ", node->parent->name);
        node = node->parent;
    }
}

// 9. 자식 중 2명 이상 왕이 된 사람
int count_multiple_successors() {
    int count = 0;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]->child_count >= 2) {
            count++;
        }
    }
    return count;
}

// 10. 예종은 태종의 몇 대 후손?
int count_generations(const wchar_t* descendant, const wchar_t* ancestor) {
    Node* node = NULL;
    for (int i = 0; i < node_count; i++) {
        if (wcscmp(nodes[i]->name, descendant) == 0) {
            node = nodes[i];
            break;
        }
    }

    int gen = 0;
    while (node && node->parent) {
        if (wcscmp(node->parent->name, ancestor) == 0) {
            return gen + 1;
        }
        node = node->parent;
        gen++;
    }
    return -1;
}

int main() {
    _setmode(_fileno(stdout), _O_U8TEXT); // UTF-8 출력
    _setmode(_fileno(stdin), _O_U8TEXT);
    setlocale(LC_ALL, "");

    FILE* fp = _wfopen(L"조선왕조.txt", L"rt, ccs=UTF-8"); // UTF-8 파일 읽기
    if (!fp) {
        wprintf(L"파일 열기 실패\n");
        return 1;
    }

    wchar_t line[100], child[MAX_NAME], parent[MAX_NAME];
    while (fgetws(line, sizeof(line) / sizeof(wchar_t), fp)) {
        int count = swscanf(line, L"%ls %ls", child, parent);
        if (count == 1) {
            root = find_or_create_node(child);
        }
        else if (count == 2) {
            add_relation(child, parent);
        }
    }
    fclose(fp);

    print_tree();

    wprintf(L"\n\n--- 질문에 의한 답변 ---\n");

    wprintf(L"\n[Q1] 조선의 왕을 순서대로 출력하시오:\n");
    print_in_order(root);

    wprintf(L"\n\n[Q2] 조선의 왕을 역순으로 출력하시오:\n");
    print_reverse_order(root);

    wprintf(L"\n\n[Q3] 조성의 왕은 모두 몇 명인가?\n%d명\n", count_kings());

    wprintf(L"\n[Q4] 조선의 왕 중에서 인조의 후손은 누구누구인가?\n");
    Node* injo = find_or_create_node(L"인조");
    print_descendants(injo);

    wprintf(L"\n\n[Q5] 직계 후손이 왕이 되지 못한 왕은 누구누구인가?\n");
    print_no_successor_kings();

    wprintf(L"\n\n[Q6] 직계 후손이 왕이 된 수가 가장 많은 왕은 누구인가?\n");
    print_most_children_king();

    wprintf(L"\n\n[Q7] 정종의 형제로 조선의 왕이 된 사람은 누구인가? \n");
    print_sibling_kings(L"정종");

    wprintf(L"\n\n[Q8] 순종의 직계 선조를 모두 출력하시오:\n");
    print_ancestors(L"순종");

    wprintf(L"\n\n[Q9] 직계 후손이 2명 이상 왕이 된 왕은 몇 명인가? \n%d명\n", count_multiple_successors());

    wprintf(L"\n[Q10] 예종은 태종의 몇 대 후손인가?\n%d대 후손입니다.\n", count_generations(L"예종", L"태종"));

    return 0;
}


