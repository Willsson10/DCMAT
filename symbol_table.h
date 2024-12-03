#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

// Definição do tipo de dado
typedef enum {
    TYPE_NUMBER,
    TYPE_MATRIX
} DataType;

// Estrutura para variáveis
typedef struct Variable {
    char *name;
    DataType type; // Tipo da variável: número ou matriz
    union {
        double value;  // Para variáveis escalares
        Matrix *matrix; // Para variáveis do tipo matriz
    } data;
    struct Variable *next;
} Variable;

// Funções para manipular a tabela de símbolos
void set_variable_number(const char *name, double value);
void set_variable_matrix(const char *name, Matrix *matrix);
bool get_variable_number(const char *name, double *value);
bool get_variable_matrix(const char *name, Matrix **matrix);
void free_symbol_table();
void show_symbols();
bool remove_variable(const char *name);

#endif
