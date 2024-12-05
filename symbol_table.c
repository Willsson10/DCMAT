#include "symbol_table.h"

#define TABLE_SIZE 100  // Tamanho da hash table

// Array de ponteiros para as listas de variáveis (encadeamento para colisão)
static Variable *symbol_table[TABLE_SIZE] = { NULL };

// Função hash para mapear nomes a índices
unsigned int hash(const char *name) {
    unsigned int hash_value = 0;
    for (const char *p = name; *p != '\0'; p++) {
        hash_value = (hash_value * 31) + *p;  // Escolhe 31 como multiplicador
    }
    return hash_value % TABLE_SIZE;
}


void setNumber (const char *nome, double valor) {
    unsigned int index = hash(nome);
    Variable *var = symbol_table[index];

    // Confere se a variavel existe, e atualiza seu valor
    while (var != NULL) {
        if (strcmp(var->name, nome) == 0) {
            if(var->type == TYPE_MATRIX){
                freeMatrix(var->data.matrix);
                var->data.matrix = NULL;
                var->type = TYPE_NUMBER;
            }
            var->data.value = valor;
            return;
        }
        var = var->next;
    }

    // Caso a variável não exista, eh criada uma nova
    var = malloc(sizeof(Variable));
    var->name = strdup(nome);
    var->type = TYPE_NUMBER;
    var->data.value = valor;
    var->next = symbol_table[index];
    symbol_table[index] = var;
}


void setMatrix (const char *nome, Matrix *matrix) {
    unsigned int index = hash(nome);
    Variable *var = symbol_table[index];

    // Confere se a variavel existe, e atualiza seu valor
    while (var != NULL) {
        if (strcmp(var->name, nome) == 0) {
            if(var->type == TYPE_NUMBER)var->type = TYPE_MATRIX;
            else freeMatrix(var->data.matrix); // Libera a matriz anterior
            var->data.matrix = NULL;
            var->data.matrix = matrix;
            return;
        }
        var = var->next;
    }

    // Caso a variável não exista, eh criada uma nova
    var = malloc(sizeof(Variable));
    var->name = strdup(nome);
    var->type = TYPE_MATRIX;
    var->data.matrix = matrix;
    var->next = symbol_table[index];
    symbol_table[index] = var;
}


bool getNumber (const char *nome, double *valor) {
    unsigned int index = hash(nome);
    Variable *var = symbol_table[index];

    while (var != NULL) {
        if (strcmp(var->name, nome) == 0 && var->type == TYPE_NUMBER) {
            *valor = var->data.value;
            return true;
        }
        var = var->next;
    }
    return false;
}

bool getMatrix (const char *nome, Matrix **matrix) {
    unsigned int index = hash(nome);
    Variable *var = symbol_table[index];

    while (var != NULL) {
        if (strcmp(var->name, nome) == 0 && var->type == TYPE_MATRIX) {
            *matrix = var->data.matrix;
            return true;
        }
        var = var->next;
    }
    return false;
}

void showVariables () {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Variable *var = symbol_table[i];
        while (var != NULL) {   
            printf("%s - ", var->name);
            if(var->type == TYPE_NUMBER)printf("FLOAT\n");
            else if(var->type == TYPE_MATRIX)printf("MATRIX [%d][%d]\n", var->data.matrix->rows,var->data.matrix->cols);
            var = var->next;
        }
    }
}

void freeVariables () {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Variable *var = symbol_table[i];
        while (var != NULL) {
            Variable *next = var->next;
            // printf("FREED %s\n",var->name);
            free(var->name);
            if (var->type == TYPE_MATRIX) {
                freeMatrix(var->data.matrix);
            }
            free(var);
            var = next;
        }
        symbol_table[i] = NULL;
    }
}

bool removeVariable (const char *nome) {
    unsigned int index = hash(nome);
    Variable *var = symbol_table[index];
    Variable *prev = NULL;

    while (var != NULL) {
        if (strcmp(var->name, nome) == 0) {
            if (prev == NULL) symbol_table[index] = var->next;
            else prev->next = var->next;

            free(var->name);
            if (var->type == TYPE_MATRIX) freeMatrix(var->data.matrix);
            free(var);

            return true; // Remoção bem-sucedida
        }

        prev = var;
        var = var->next;
    }

    return false; // Variável não encontrada
}