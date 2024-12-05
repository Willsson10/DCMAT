#ifndef __AST_H__
#define __AST_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "symbol_table.h"

typedef enum {
    NUM,
    OPERATOR,
    UN_OPERATOR,
    TRIG,
    TYPE_X,
    TYPE_MAT,
    VARIABLE
} Type;

// Estrutura para variáveis
typedef struct Result {
    Type type; // Tipo da variável: número ou matriz
    union {
        double value;  // Para variáveis escalares
        Matrix *matrix; // Para variáveis do tipo matriz
    } data;
} Result;

// Definição das estruturas
typedef struct node_default {
    Type type;
} NODE_DEFAULT;

typedef struct node_num {
    Type type;
    double valor;
} NODE_NUM;

typedef struct node_operator {
    Type type;
    char operador;
    NODE_DEFAULT *esq;
    NODE_DEFAULT *dir;
} NODE_OPERATOR;

typedef struct node_un_operator {
    Type type;
    char operador;
    NODE_DEFAULT *espr;
} NODE_UN_OPERATOR;

typedef struct node_trig {
    Type type;
    char funcao_trigonometrica;
    NODE_DEFAULT* espr;
} NODE_TRIG;

typedef struct node_x {
    Type type;
    double* value;
} NODE_X;

typedef struct node_matrix {
    Type type;
    Matrix* matrix;
} NODE_MATRIX;

// 0 = NUMBER, 1 = MATRIX, 2 = UNDEF
typedef struct node_variable {
    Type type;
    int var_type;
    char* nome;
} NODE_VARIABLE;

NODE_DEFAULT* new_num(double val);
NODE_DEFAULT* new_int(int val);
NODE_DEFAULT* new_op(NODE_DEFAULT* esq, char op, NODE_DEFAULT* dir);
NODE_DEFAULT* new_UNop(char op,NODE_DEFAULT* espr);
NODE_DEFAULT* new_trig(char funcao,NODE_DEFAULT* espr);
NODE_DEFAULT* new_X();
NODE_DEFAULT* new_matrix(Matrix* mat);
NODE_DEFAULT* new_var(char* nome, int var_type);
void free_tree(NODE_DEFAULT *node);
void change_X_value(double new_value);
double get_X_value();
void freeResult(Result *result);

double get_num_value(Result* node);
NODE_DEFAULT* apply_sign_to_value(NODE_DEFAULT* node);

#endif 