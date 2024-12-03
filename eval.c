#include "eval.h"

Result* eval(AST_NODE *node) {
    Result* resultado = malloc(sizeof(Result));
    resultado->data.matrix = NULL;
    resultado->data.value = 0;
    if (!node)return resultado;

    if (node->type == TYPE_NUM) {
        AST_NUM* num_node = (AST_NUM*)node;
            resultado->type = TYPE_NUM;
            resultado->data.value = num_node->valor;
        return resultado;
    }
    else if(node->type == TYPE_OP){
        
        AST_OP* op_node = (AST_OP*)node;
        Result* left_val = eval(op_node->esq);
        Result* right_val = eval(op_node->dir);

        if(left_val->type == TYPE_NUM && right_val->type== TYPE_NUM){
            resultado->type = TYPE_NUM;
            switch (op_node->operador) {
                case '+': resultado->data.value = left_val->data.value + right_val->data.value;break;
                case '-': resultado->data.value = left_val->data.value - right_val->data.value;break;
                case '*': resultado->data.value = left_val->data.value * right_val->data.value;break;
                case '/': resultado->data.value = left_val->data.value / right_val->data.value;break;
                case '^': resultado->data.value = pow(left_val->data.value, right_val->data.value);break;
                case '%': resultado->data.value = fmod(left_val->data.value, right_val->data.value);break;
                default: resultado->data.value = 0;break;
            }
        }
        else if(left_val->type == TYPE_MAT && right_val->type == TYPE_MAT){
            resultado->type = TYPE_MAT;
            switch (op_node->operador){
                case '+' : resultado->data.matrix = somaMatrizes(left_val->data.matrix,right_val->data.matrix);break;
                case '-' : resultado->data.matrix = subtraiMatrizes(left_val->data.matrix,right_val->data.matrix);break;
                case '*' : resultado->data.matrix = mulMatrizMatriz(left_val->data.matrix,right_val->data.matrix);break;
                default : break;
                }
        }

        else if((left_val->type == TYPE_MAT && right_val->type == TYPE_NUM)){
            resultado->type = TYPE_MAT;
            switch (op_node->operador){
                case '*' : resultado->data.matrix = mulMatrizEscalar(left_val->data.matrix,right_val->data.value);break;
                default:printf("VALORES INCOMPATIVEIS\n");break;
            }
        }
        else if((left_val->type == TYPE_NUM && right_val->type == TYPE_MAT)){
            resultado->type = TYPE_MAT;
            switch (op_node->operador){
                case '*' : resultado->data.matrix = mulMatrizEscalar(right_val->data.matrix,left_val->data.value);break;
                default:printf("VALORES INCOMPATIVEIS\n");break;
            }
        }
        freeResult(left_val);
        freeResult(right_val);
        return resultado;
    }
    else if(node->type == TYPE_UNOP){
        AST_UNOP* UNop_node = (AST_UNOP*)node;
        Result* expression = eval(UNop_node->espr);
        if(expression->type == TYPE_NUM){
            resultado->type = TYPE_NUM;
            switch (UNop_node->operador){
                case '+' : resultado->data.value = expression->data.value;break;
                case '-' : resultado->data.value = 0 - expression->data.value;break;
                default : break;
            }
        }
        freeResult(expression);
    }
    else if(node->type == TYPE_TRIG){
        AST_TRIG* trig_node = (AST_TRIG*)node;
        Result* expression = eval(trig_node->espr);
        if(expression->type == TYPE_NUM){
            resultado->type = TYPE_NUM;
            switch (trig_node->funcao_trigonometrica)
            {
                case 's': resultado->data.value = sin(expression->data.value);break;
                case 'c': resultado->data.value = cos(expression->data.value);break;
                case 't': resultado->data.value = tan(expression->data.value);break;
                case 'a': resultado->data.value = fabs(expression->data.value);break; 
                default : break;
            }
        }
        freeResult(expression);
    }   
    else if(node->type == TYPE_X){
        resultado->type = TYPE_NUM;
        AST_X* X_node = (AST_X*)node;
        resultado->data.value = *X_node->value;
    }
    else if(node->type == TYPE_MAT){
        resultado->type = TYPE_MAT;
        AST_MATRIX* matrix_node = (AST_MATRIX*)node;
        resultado->data.matrix = copiaMatriz(matrix_node->matrix);
    }
    else if(node->type == TYPE_VAR){
        AST_VAR* var_node = (AST_VAR*)node;
        if(var_node->var_type == 0){
            if(get_variable_number(var_node->nome,&resultado->data.value))resultado->type = TYPE_NUM;
            else printf("1Variável não definida");
        }
        else if(var_node->var_type == 1){
            Matrix* aux = NULL;
            if(get_variable_matrix(var_node->nome,&aux)){
                resultado->type = TYPE_MAT;
                resultado->data.matrix = copiaMatriz(aux);
            }
            else printf("2Variável não definida");
        }
    }
    return resultado;
}

void rpn_eval(AST_NODE *node) {
    if (!node)return;

    if (node->type == TYPE_NUM) {
        AST_NUM* num_node = (AST_NUM*)node;
        if(num_node->valor == 0){
            AST_INT* int_node = (AST_INT*)node;
            printf("%d ", int_node->valor);
        }
        else {
            printf("%f ", num_node->valor);
        }
    }
    else if(node->type == TYPE_OP){
        AST_OP* op_node = (AST_OP*)node;
        rpn_eval(op_node->esq);
        rpn_eval(op_node->dir);
        printf("%c ",op_node->operador);
    }
    else if(node->type == TYPE_UNOP){
        AST_UNOP* UNop_node = (AST_UNOP*)node;
        rpn_eval(UNop_node->espr);
        printf("%c ",UNop_node->operador);
    }
    else if(node->type == TYPE_TRIG){
        AST_TRIG* trig_node = (AST_TRIG*)node;
        rpn_eval(trig_node->espr);
        switch (trig_node->funcao_trigonometrica)
        {
            case 's': printf("SEN ");break;
            case 'c': printf("COS ");break;
            case 't': printf("TAN ");break;
            case 'a': printf("ABS ");break;
            default : break;
        }
        
    }   
    else if(node->type == TYPE_X){
        printf("x ");
    }
    else if(node->type == TYPE_MAT){
        return;
    }
    else if(node->type == TYPE_VAR){
        AST_VAR* var_node = (AST_VAR*)node;
        printf("%s ",var_node->nome);
    }
}

AST_NODE* copy_tree(AST_NODE* node) {
    if (!node) return NULL;

    // Cria um novo nó
    AST_NODE* novo_node = malloc(sizeof(AST_NODE));
    if (!novo_node) {
        printf("Erro de alocação de memória para cópia da árvore.\n");
        return NULL;
    }

    novo_node->type = node->type;

    // Copia o conteúdo dependendo do tipo do nó
    if (node->type == TYPE_NUM) {
        AST_NUM* num_node = (AST_NUM*)node;
        AST_NUM* novo_num_node = (AST_NUM*)novo_node;
        novo_num_node->valor = num_node->valor;
    }
    else if (node->type == TYPE_OP) {
        AST_OP* op_node = (AST_OP*)node;
        AST_OP* novo_op_node = (AST_OP*)novo_node;
        novo_op_node->operador = op_node->operador;
        novo_op_node->esq = copy_tree(op_node->esq);
        novo_op_node->dir = copy_tree(op_node->dir);
    }
    else if (node->type == TYPE_UNOP) {
        AST_UNOP* unop_node = (AST_UNOP*)node;
        AST_UNOP* novo_unop_node = (AST_UNOP*)novo_node;
        novo_unop_node->operador = unop_node->operador;
        novo_unop_node->espr = copy_tree(unop_node->espr);
    }
    else if (node->type == TYPE_TRIG) {
        AST_TRIG* trig_node = (AST_TRIG*)node;
        AST_TRIG* novo_trig_node = (AST_TRIG*)novo_node;
        novo_trig_node->funcao_trigonometrica = trig_node->funcao_trigonometrica;
        novo_trig_node->espr = copy_tree(trig_node->espr);
    }
    else if (node->type == TYPE_X) {
        AST_X* x_node = (AST_X*)node;
        AST_X* novo_x_node = (AST_X*)novo_node;
        novo_x_node->value = x_node->value;  // Cópia direta do ponteiro
    }
    else if (node->type == TYPE_MAT) {
        AST_MATRIX* mat_node = (AST_MATRIX*)node;
        AST_MATRIX* novo_mat_node = (AST_MATRIX*)novo_node;
        novo_mat_node->matrix = copiaMatriz(mat_node->matrix);  // Cópia da matriz
    }
    else if (node->type == TYPE_VAR) {
        AST_VAR* var_node = (AST_VAR*)node;
        AST_VAR* novo_var_node = (AST_VAR*)novo_node;
        novo_var_node->var_type = var_node->var_type;
        strcpy(novo_var_node->nome, var_node->nome);  // Cópia do nome da variável
    }

    return novo_node;
}
