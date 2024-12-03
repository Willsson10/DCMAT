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


void set_variable_number(const char *name, double value) {
    unsigned int index = hash(name);
    Variable *var = symbol_table[index];

    // Procura a variável na lista encadeada
    while (var != NULL) {
        if (strcmp(var->name, name) == 0) {
            if(var->type == TYPE_MATRIX){
                printf("Variavel era matriz\n");
                freeMatrix(var->data.matrix);
                var->data.matrix = NULL;
                var->type = TYPE_NUMBER;
            }
            var->data.value = value;
            return;
        }
        var = var->next;
    }

    // Se a variável não existe, cria uma nova
    var = malloc(sizeof(Variable));
    var->name = strdup(name);
    var->type = TYPE_NUMBER;
    var->data.value = value;
    var->next = symbol_table[index];
    symbol_table[index] = var;
}


void set_variable_matrix(const char *name, Matrix *matrix) {
    unsigned int index = hash(name);
    Variable *var = symbol_table[index];

    // Procura a variável na lista encadeada
    while (var != NULL) {
        if (strcmp(var->name, name) == 0) {
            if(var->type == TYPE_NUMBER)var->type = TYPE_MATRIX;
            else freeMatrix(var->data.matrix); // Libera a matriz anterior
            var->data.matrix = NULL;
            var->data.matrix = matrix;
            return;
        }
        var = var->next;
    }

    // Se a variável não existe, cria uma nova
    var = malloc(sizeof(Variable));
    var->name = strdup(name);
    var->type = TYPE_MATRIX;
    var->data.matrix = matrix;
    var->next = symbol_table[index];
    symbol_table[index] = var;
}


bool get_variable_number(const char *name, double *value) {
    unsigned int index = hash(name);
    Variable *var = symbol_table[index];

    while (var != NULL) {
        if (strcmp(var->name, name) == 0 && var->type == TYPE_NUMBER) {
            *value = var->data.value;
            return true;
        }
        var = var->next;
    }
    return false;
}

bool get_variable_matrix(const char *name, Matrix **matrix) {
    unsigned int index = hash(name);
    Variable *var = symbol_table[index];

    while (var != NULL) {
        if (strcmp(var->name, name) == 0 && var->type == TYPE_MATRIX) {
            *matrix = var->data.matrix;
            return true;
        }
        var = var->next;
    }
    return false;
}

void show_symbols(){
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

void free_symbol_table() {
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

bool remove_variable(const char *name) {
    unsigned int index = hash(name);
    Variable *var = symbol_table[index];
    Variable *prev = NULL;

    while (var != NULL) {
        if (strcmp(var->name, name) == 0) {
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