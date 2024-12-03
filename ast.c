#include "ast.h"
#include "eval.h"

double x_value = 0;
// Função para criar um nó número
AST_NODE* new_num(double val) {
    AST_NUM* num = malloc(sizeof(AST_NUM));
    num->type = TYPE_NUM;
    num->valor = val;
    return (AST_NODE*)num;
}

AST_NODE* new_int(int val) {
    AST_INT* num = malloc(sizeof(AST_INT));
    num->type = TYPE_NUM;
    num->valor = val;
    return (AST_NODE*)num;
}

// Função para criar um nó operador
AST_NODE* new_op(AST_NODE* esq, char op, AST_NODE* dir) {
    AST_OP* op_node = malloc(sizeof(AST_OP));
    op_node->type = TYPE_OP;
    op_node->operador = op;
    op_node->esq = esq;
    op_node->dir = dir;
    return (AST_NODE*)op_node;
}

// Função para criar um nó operador
AST_NODE* new_UNop(char op,AST_NODE* espr) {
    AST_UNOP* UNop_node = malloc(sizeof(AST_OP));
    UNop_node->type = TYPE_UNOP;
    UNop_node->operador = op;
    UNop_node->espr = espr;
    return (AST_NODE*)UNop_node;
}

AST_NODE* new_trig(char funcao,AST_NODE* espr) {
    AST_TRIG* trig_node = malloc(sizeof(AST_TRIG));
    trig_node->type = TYPE_TRIG;
    trig_node->funcao_trigonometrica = funcao;
    trig_node->espr = espr;
    return (AST_NODE*)trig_node;
}

AST_NODE* new_X(){
    AST_X* X_node = malloc(sizeof(AST_X));
    X_node->type = TYPE_X;
    X_node->value = &x_value;
    return (AST_NODE*)X_node;
}

AST_NODE* new_matrix(Matrix* mat){
    AST_MATRIX* matrix_node = malloc(sizeof(AST_MATRIX));
    matrix_node->type = TYPE_MAT;
    matrix_node->matrix = copiaMatriz(mat);
    return (AST_NODE*)matrix_node;
}

AST_NODE* new_var(char* nome, int var_type){
    AST_VAR* var_node = malloc(sizeof(AST_VAR));
    var_node->type = TYPE_VAR;
    var_node->nome = nome;
    var_node->var_type = var_type;
    return (AST_NODE*)var_node;
}

// Função para liberar a árvore
void free_tree(AST_NODE *node) {
    if (!node) return;
    // printf("FREED NODE %d\n", node->type);
    if (node->type == TYPE_OP) {
        AST_OP* op_node = (AST_OP*)node;
        free_tree(op_node->esq);
        free_tree(op_node->dir);
    }
    else if(node->type == TYPE_UNOP){
        AST_UNOP* UNop_node =(AST_UNOP*)node;
        free_tree(UNop_node->espr);
    }
    else if(node->type == TYPE_TRIG){
        AST_TRIG* trig_node = (AST_TRIG*)node;
        free_tree(trig_node->espr);
    }
    else if(node->type == TYPE_MAT){
        AST_MATRIX* matrix_node = (AST_MATRIX*)node;
        freeMatrix(matrix_node->matrix);
    }
    else if(node->type == TYPE_VAR){
        AST_VAR* var_node = (AST_VAR*)node;
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
    if (node->type == TYPE_NUM) {
        return ((AST_NUM*)node)->valor;
    } else {
        printf("Erro: Esperado um número.\n");
        return 0; // Apenas para evitar warnings do compilador
    }
}

AST_NODE* apply_sign_to_value(AST_NODE* node) {
    if (node->type == TYPE_NUM) {
        AST_NUM* num_node = (AST_NUM*)node; // Cast para AST_NUM
        num_node->valor = -num_node->valor; // Aplica o sinal negativo ao valor
        return (AST_NODE*)num_node; // Retorna o nó alterado
    }
    // Se não for do tipo AST_NUM, apenas retorna o nó como está
    return node;
}