#ifndef __AST_H__
#define __AST_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "symbol_table.h"

typedef enum {
    TYPE_DEFAULT,
    TYPE_NUM,
    TYPE_INT,
    TYPE_OP,
    TYPE_UNOP,
    TYPE_TRIG,
    TYPE_X,
    TYPE_MAT,
    TYPE_VAR
} Type;

// Estrutura para variáveis
typedef struct Result {
    Type type; // Tipo da variável: número ou matriz
    union {
        int ivalue;
        double value;  // Para variáveis escalares
        Matrix *matrix; // Para variáveis do tipo matriz
    } data;
} Result;

// Definição das estruturas
typedef struct ast_node {
    Type type;
} AST_NODE;

typedef struct ast_num {
    Type type;
    double valor;
} AST_NUM;

typedef struct ast_int {
    Type type;
    int valor;
}AST_INT;

typedef struct ast_op {
    Type type;
    char operador;
    AST_NODE *esq;
    AST_NODE *dir;
} AST_OP;

typedef struct ast_UNop {
    Type type;
    char operador;
    AST_NODE *espr;
} AST_UNOP;

typedef struct ast_trig {
    Type type;
    char funcao_trigonometrica;
    AST_NODE* espr;
}AST_TRIG;

typedef struct ast_x {
    Type type;
    double* value;
}AST_X;

typedef struct ast_matrix {
    Type type;
    Matrix* matrix;
}AST_MATRIX;

// 0 = NUMBER, 1 = MATRIX, 2 = UNDEF
typedef struct var {
    Type type;
    int var_type;
    char* nome;
}AST_VAR;

AST_NODE* new_num(double val);
AST_NODE* new_int(int val);
AST_NODE* new_op(AST_NODE* esq, char op, AST_NODE* dir);
AST_NODE* new_UNop(char op,AST_NODE* espr);
AST_NODE* new_trig(char funcao,AST_NODE* espr);
AST_NODE* new_X();
AST_NODE* new_matrix(Matrix* mat);
AST_NODE* new_var(char* nome, int var_type);
void free_tree(AST_NODE *node);
void change_X_value(double new_value);
double get_X_value();
void freeResult(Result *result);

double get_num_value(Result* node);
AST_NODE* apply_sign_to_value(AST_NODE* node);

#endif 