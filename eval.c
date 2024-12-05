#include "eval.h"

Result* eval(NODE_DEFAULT *node) {
    Result* resultado = malloc(sizeof(Result));
    resultado->data.matrix = NULL;
    resultado->data.value = 0;
    if (!node)return resultado;

    if (node->type == NUM) {
        NODE_NUM* num_node = (NODE_NUM*)node;
            resultado->type = NUM;
            resultado->data.value = num_node->valor;
        return resultado;
    }
    else if(node->type == OPERATOR){
        
        NODE_OPERATOR* op_node = (NODE_OPERATOR*)node;
        Result* left_val = eval(op_node->esq);
        Result* right_val = eval(op_node->dir);

        if(left_val->type == NUM && right_val->type== NUM){
            resultado->type = NUM;
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

        else if((left_val->type == TYPE_MAT && right_val->type == NUM)){
            resultado->type = TYPE_MAT;
            switch (op_node->operador){
                case '*' : resultado->data.matrix = mulMatrizEscalar(left_val->data.matrix,right_val->data.value);break;
                default:printf("VALORES INCOMPATIVEIS\n");break;
            }
        }
        else if((left_val->type == NUM && right_val->type == TYPE_MAT)){
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
    else if(node->type == UN_OPERATOR){
        NODE_UN_OPERATOR* UNop_node = (NODE_UN_OPERATOR*)node;
        Result* expression = eval(UNop_node->espr);
        if(expression->type == NUM){
            resultado->type = NUM;
            switch (UNop_node->operador){
                case '+' : resultado->data.value = expression->data.value;break;
                case '-' : resultado->data.value = 0 - expression->data.value;break;
                default : break;
            }
        }
        freeResult(expression);
    }
    else if(node->type == TRIG){
        NODE_TRIG* trig_node = (NODE_TRIG*)node;
        Result* expression = eval(trig_node->espr);
        if(expression->type == NUM){
            resultado->type = NUM;
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
        resultado->type = NUM;
        NODE_X* X_node = (NODE_X*)node;
        resultado->data.value = *X_node->value;
    }
    else if(node->type == TYPE_MAT){
        resultado->type = TYPE_MAT;
        NODE_MATRIX* matrix_node = (NODE_MATRIX*)node;
        resultado->data.matrix = copiaMatriz(matrix_node->matrix);
    }
    else if(node->type == VARIABLE){
        NODE_VARIABLE* var_node = (NODE_VARIABLE*)node;
        if(var_node->var_type == 0){
            if(getNumber(var_node->nome,&resultado->data.value))resultado->type = NUM;
            else printf("Variável não definida\n");
        }
        else if(var_node->var_type == 1){
            Matrix* aux = NULL;
            if(getMatrix(var_node->nome,&aux)){
                resultado->type = TYPE_MAT;
                resultado->data.matrix = copiaMatriz(aux);
            }
            else printf("Variável não definida\n");
        }
    }
    return resultado;
}

void rpn_eval(NODE_DEFAULT *node) {
    if (!node)return;

    if (node->type == NUM) {
        NODE_NUM* num_node = (NODE_NUM*)node;
        printf("%f ", num_node->valor);
    }
    else if(node->type == OPERATOR){
        NODE_OPERATOR* op_node = (NODE_OPERATOR*)node;
        rpn_eval(op_node->esq);
        rpn_eval(op_node->dir);
        printf("%c ",op_node->operador);
    }
    else if(node->type == UN_OPERATOR){
        NODE_UN_OPERATOR* UNop_node = (NODE_UN_OPERATOR*)node;
        rpn_eval(UNop_node->espr);
        printf("%c ",UNop_node->operador);
    }
    else if(node->type == TRIG){
        NODE_TRIG* trig_node = (NODE_TRIG*)node;
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
    else if(node->type == VARIABLE){
        NODE_VARIABLE* var_node = (NODE_VARIABLE*)node;
        printf("%s ",var_node->nome);
    }
}

NODE_DEFAULT* copy_tree(NODE_DEFAULT* node) {
    if (!node) return NULL;

    // Cria um novo nó
    NODE_DEFAULT* novo_node = malloc(sizeof(NODE_DEFAULT));
    if (!novo_node) {
        printf("Erro de alocação de memória para cópia da árvore.\n");
        return NULL;
    }

    novo_node->type = node->type;

    // Copia o conteúdo dependendo do tipo do nó
    if (node->type == NUM) {
        NODE_NUM* num_node = (NODE_NUM*)node;
        NODE_NUM* novo_num_node = (NODE_NUM*)novo_node;
        novo_num_node->valor = num_node->valor;
    }
    else if (node->type == OPERATOR) {
        NODE_OPERATOR* op_node = (NODE_OPERATOR*)node;
        NODE_OPERATOR* novo_op_node = (NODE_OPERATOR*)novo_node;
        novo_op_node->operador = op_node->operador;
        novo_op_node->esq = copy_tree(op_node->esq);
        novo_op_node->dir = copy_tree(op_node->dir);
    }
    else if (node->type == UN_OPERATOR) {
        NODE_UN_OPERATOR* unop_node = (NODE_UN_OPERATOR*)node;
        NODE_UN_OPERATOR* novo_unop_node = (NODE_UN_OPERATOR*)novo_node;
        novo_unop_node->operador = unop_node->operador;
        novo_unop_node->espr = copy_tree(unop_node->espr);
    }
    else if (node->type == TRIG) {
        NODE_TRIG* trig_node = (NODE_TRIG*)node;
        NODE_TRIG* novo_trig_node = (NODE_TRIG*)novo_node;
        novo_trig_node->funcao_trigonometrica = trig_node->funcao_trigonometrica;
        novo_trig_node->espr = copy_tree(trig_node->espr);
    }
    else if (node->type == TYPE_X) {
        NODE_X* x_node = (NODE_X*)node;
        NODE_X* novo_x_node = (NODE_X*)novo_node;
        novo_x_node->value = x_node->value;  // Cópia direta do ponteiro
    }
    else if (node->type == TYPE_MAT) {
        NODE_MATRIX* mat_node = (NODE_MATRIX*)node;
        NODE_MATRIX* novo_mat_node = (NODE_MATRIX*)novo_node;
        novo_mat_node->matrix = copiaMatriz(mat_node->matrix);  // Cópia da matriz
    }
    else if (node->type == VARIABLE) {
        NODE_VARIABLE* var_node = (NODE_VARIABLE*)node;
        NODE_VARIABLE* novo_var_node = (NODE_VARIABLE*)novo_node;
        novo_var_node->var_type = var_node->var_type;
        strcpy(novo_var_node->nome, var_node->nome);  // Cópia do nome da variável
    }

    return novo_node;
}
