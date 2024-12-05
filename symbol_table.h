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

/**
 * Função responsável por inicializar variáveis numéricas, na lista de variáveis.
 * 
 * Receba como argumentos, o nome (char* nome) da variável, e o valor numerico (double valor) dela.
 * 
 * Caso a variável já exista e seja um numero, o valor eh atualizado com (double valor). Caso a variável já exista, e seja
 * uma matriz, a variável se torna um numero, e receba (double valor). Caso a variável ainda não exista, eh criada com o
 * nome (char* nome) e valor (double valor).
 */
void setNumber (const char *nome, double valor);

/**
 * Função a qual confere se uma variável do tipo numerica, existe na tabela de variáveis, e atualiza o valor de (double valor).
 * 
 * Recebe como argumentos, o nome (char* nome) da variável, e um double qualquer (double* valor).
 * 
 * Caso a variável exista, a função retorna true, e o conteudo de (double* valor) eh atualizado com o valor da variável da tabela.
 * Caso a variável não exista ou nao seja do tipo numerico, retorna false.
 */
bool getNumber (const char *nome, double *valor);

/**
 * Funçaõ responsável por inicializar variáveis do tipo Matrix, na lista de variáveis.
 * 
 * Recebe como argumento, o nome (char* nome) da variável, e o valor da matriz (Matrix* matrix) dela.
 * 
 * Caso a variávle já exista e seja uma matriz, o valor eh atualizado com (Matrix* matrix). Caso a variável já exista, e seja
 * um numero, a variável se torna do tipo matriz, e receba (Matrix* matrix). Caso a variável ainda não exista, eh criada com
 * o nome (char* nome) e matriz (Matrix* matrix).
 */
void setMatrix (const char *nome, Matrix *matrix);

/**
 * Função a qual confere se uma variável do tipo matrix, existe na tabela de variáveis, e atualiza o valor de (Matrix** matrix).
 * 
 * Recebe como argumentos, o nome (char* nome) da variável, e uma Matrix qualquer (Matrix** matrix).
 * 
 * Caso a variável exista, a função retorna true, e o conteudo de (Matrix** maitrx) eh atualizado com o valor da variável da tabela.
 * Caso a variável não exista ou nao seja do tipo matrix, retorna false.
 */
bool getMatrix (const char *name, Matrix **matrix);

/**
 * Função a qual mostra o nome e tipo de todas as variavéis de declaradas no sistema. Caso não tenham, não motra nada.
 * 
 */
void showVariables ();

/**
 * Função a qual libera a memória de todas as variáveis já declaradas no sistema. 
 * 
 */
void freeVariables ();

/**
 * Função a qual libera memória de uma variável de nome (char* nome) da tabela de variáveis.
 * 
 */
bool removeVariable (const char *nome);

#endif
