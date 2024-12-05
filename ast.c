#include "ast.h"
#include "eval.h"

double x_value = 0;
// Função para criar um nó número
NODE_DEFAULT* new_num (double val) {
    NODE_NUM* num = malloc(sizeof(NODE_NUM));
    num->type = NUM;
    num->valor = val;
    return (NODE_DEFAULT*)num;
}

// Função para criar um nó operador
NODE_DEFAULT* new_op(NODE_DEFAULT* esq, char op, NODE_DEFAULT* dir) {
    NODE_OPERATOR* op_node = malloc(sizeof(NODE_OPERATOR));
    op_node->type = OPERATOR;
    op_node->operador = op;
    op_node->esq = esq;
    op_node->dir = dir;
    return (NODE_DEFAULT*)op_node;
}

// Função para criar um nó operador
NODE_DEFAULT* new_UNop(char op,NODE_DEFAULT* espr) {
    NODE_UN_OPERATOR* UNop_node = malloc(sizeof(NODE_OPERATOR));
    UNop_node->type = UN_OPERATOR;
    UNop_node->operador = op;
    UNop_node->espr = espr;
    return (NODE_DEFAULT*)UNop_node;
}

NODE_DEFAULT* new_trig(char funcao,NODE_DEFAULT* espr) {
    NODE_TRIG* trig_node = malloc(sizeof(NODE_TRIG));
    trig_node->type = TRIG;
    trig_node->funcao_trigonometrica = funcao;
    trig_node->espr = espr;
    return (NODE_DEFAULT*)trig_node;
}

NODE_DEFAULT* new_X(){
    NODE_X* X_node = malloc(sizeof(NODE_X));
    X_node->type = TYPE_X;
    X_node->value = &x_value;
    return (NODE_DEFAULT*)X_node;
}

NODE_DEFAULT* new_matrix(Matrix* mat){
    NODE_MATRIX* matrix_node = malloc(sizeof(NODE_MATRIX));
    matrix_node->type = TYPE_MAT;
    matrix_node->matrix = copiaMatriz(mat);
    return (NODE_DEFAULT*)matrix_node;
}

NODE_DEFAULT* new_var(char* nome, int var_type){
    NODE_VARIABLE* var_node = malloc(sizeof(NODE_VARIABLE));
    var_node->type = VARIABLE;
    var_node->nome = nome;
    var_node->var_type = var_type;
    return (NODE_DEFAULT*)var_node;
}

// Função para liberar a árvore
void free_tree(NODE_DEFAULT *node) {
    if (!node) return;
    // printf("FREED NODE %d\n", node->type);
    if (node->type == OPERATOR) {
        NODE_OPERATOR* op_node = (NODE_OPERATOR*)node;
        free_tree(op_node->esq);
        free_tree(op_node->dir);
    }
    else if(node->type == UN_OPERATOR){
        NODE_UN_OPERATOR* UNop_node =(NODE_UN_OPERATOR*)node;
        free_tree(UNop_node->espr);
    }
    else if(node->type == TRIG){
        NODE_TRIG* trig_node = (NODE_TRIG*)node;
        free_tree(trig_node->espr);
    }
    else if(node->type == TYPE_MAT){
        NODE_MATRIX* matrix_node = (NODE_MATRIX*)node;
        freeMatrix(matrix_node->matrix);
    }
    else if(node->type == VARIABLE){
        NODE_VARIABLE* var_node = (NODE_VARIABLE*)node;
        free(var_node->nome);
    }
    free(node);
}

void freeResult(Result *result) {
    if (!result) return;

    // Verifica o tipo armazenado e libera se for necessário
    if (result->type == TYPE_MAT && result->data.matrix) {
        freeMatrix(result->data.matrix); // Libera a matriz usando a função apropriada
        result->data.matrix = NULL;     // Previne acessos inválidos
    }

    // Libera o próprio Result
    free(result);
}


void change_X_value(double new_value){
    x_value = new_value;
}

double get_X_value(){
    return x_value;
}

double get_num_value(Result* node) {
    if (node->type == NUM) {
        return ((NODE_NUM*)node)->valor;
    } else {
        printf("Erro: Esperado um número.\n");
        return 0; // Apenas para evitar warnings do compilador
    }
}

NODE_DEFAULT* apply_sign_to_value(NODE_DEFAULT* node) {
    if (node->type == NUM) {
        NODE_NUM* num_node = (NODE_NUM*)node; // Cast para NODE_NUM
        num_node->valor = -num_node->valor; // Aplica o sinal negativo ao valor
        return (NODE_DEFAULT*)num_node; // Retorna o nó alterado
    }
    // Se não for do tipo NODE_NUM, apenas retorna o nó como está
    return node;
}