#include "functions.h"
#include "eval.h"

#define WIDTH 80   // Largura do gráfico (colunas)
#define HEIGHT 25  // Altura do gráfico (linhas)


// Variáveis internas de configuração
double h_view_lo = -6.5;
double h_view_hi = 6.5;
double v_view_lo = -3.5;
double v_view_hi = 3.5;
int float_precision = 6;
int integral_steps = 1000;
int draw_axis = 1;       // 1 para ON, 0 para OFF
int erase_plot = 1;      // 1 para ON, 0 para OFF
int connect_dots = 0;    // 1 para ON, 0 para OFF

AST_NODE* func_atual = NULL;
AST_NODE* last_plotted_func = NULL; // Guarda a última função plotada

Functions* funcs_storage = NULL;

void show_settings() {
    printf("h_view_lo: %.6f\n", h_view_lo);
    printf("h_view_hi: %.6f\n", h_view_hi);
    printf("v_view_lo: %.6f\n", v_view_lo);
    printf("v_view_hi: %.6f\n", v_view_hi);
    printf("float precision: %d\n", float_precision);
    printf("integral_steps: %d\n\n", integral_steps);
    printf("Draw Axis: %s\n", draw_axis ? "ON" : "OFF");
    printf("Erase Plot: %s\n", erase_plot ? "ON" : "OFF");
    printf("Connect Dots: %s\n", connect_dots ? "ON" : "OFF");
}

int get_erase_plot(){
    return erase_plot;
}

// void set_erase_plot(int state){
//     erase_plot = state;
//     if (state == 1) {
//         if (funcs_storage != NULL) func_atual = copy_tree(funcs_storage->func);
//         free_all_func();
//     }
//     else{
//         if (func_atual != NULL) { 
//             // Armazena uma cópia da função atual, se ela não for NULL
//             store_func(func_atual);
//             last_plotted_func = func_atual;
//         }
//         else {
//             printf("Nenhuma função atual para armazenar.\n");
//         }
//     }
// }

void set_erase_plot(int state){
    erase_plot = state;
    if(state == 1)free_all_func();
    else{
        if(func_atual)store_func(func_atual);
    }
}

void set_h_view(double lo, double hi) {
    h_view_lo = lo;
    h_view_hi = hi;
}

void set_v_view(double lo, double hi) {
    v_view_lo = lo;
    v_view_hi = hi;
}

void set_float_precision (int valor) {
    float_precision = valor;
}

int get_float_precision () {
    return float_precision;
}

void set_axis(int state) {
    draw_axis = state;
}

void set_integral_steps(int valor){
    integral_steps = valor;
}

// void store_func(AST_NODE* func){
//     Functions* new_func = malloc(sizeof(Functions));
//     new_func->func = func;
//     new_func->next = funcs_storage;

//     funcs_storage = new_func;
// }

// void store_func(AST_NODE* func){
//     if (func == NULL) {
//         printf("Função inválida para armazenar.\n");
//         return;
//     }

//     // Cria uma cópia da função usando copy_tree
//     AST_NODE* copied_func = copy_tree(func);

//     // Aloca e armazena a função copiada na lista de funções
//     Functions* new_func = malloc(sizeof(Functions));
//     new_func->func = copied_func;
//     new_func->next = funcs_storage;
//     funcs_storage = new_func;
// }

void store_func(AST_NODE* func){
    AST_NODE* clone = copy_tree(func);
    Functions* new_func = malloc(sizeof(Functions));
    new_func->func = clone;
    new_func->next = funcs_storage;

    funcs_storage = new_func;
}

// void free_all_func() {
//     Functions *var = funcs_storage;

//     // Itera pela lista e libera cada nó
//     while (var != NULL) {
//         Functions *next = var->next; // Armazena o próximo nó antes de liberar
//         free_tree(var->func);        // Libera a função armazenada no nó
//         free(var);                   // Libera o nó em si
//         var = next;                  // Avança para o próximo nó
//     }

//     // Define o início da lista como NULL após liberar todos os nós
//     funcs_storage = NULL;
// }

void free_all_func() {
    Functions *var = funcs_storage;

    // Itera pela lista e libera cada nó
    while (var != NULL) {
        Functions *next = var->next; // Armazena o próximo nó antes de liberar
        if(var->func && var->func!=func_atual)free_tree(var->func);        // Libera a função armazenada no nó
        var->func = NULL;
        free(var);                   // Libera o nó em si
        var = next;                  // Avança para o próximo nó
    }
    // Define o início da lista como NULL após liberar todos os nós
    funcs_storage = NULL;
}


void reset_settings() {
    h_view_lo = -6.5;
    h_view_hi = 6.5;
    v_view_lo = -3.5;
    v_view_hi = 3.5;
    float_precision = 6;
    integral_steps = 1000;
    draw_axis = 1;
    erase_plot = 1;
    connect_dots = 0;
    printf("Configurações resetadas.\n");
}


void free_func_atual(){
    if(func_atual) {
        free_tree(func_atual);
        func_atual = NULL;
    }
}

void set_func_atual(AST_NODE* func){
    //libera funcao antiga
    if(func_atual) free_tree(func_atual);
    func_atual = func;
}

AST_NODE* get_func_atual(){
    return func_atual;
}

// Função para plotar a função matemática
void plot_func(AST_NODE* func) {
    if (func == NULL) {
        func = last_plotted_func;
        if (func == NULL) {
            printf("Nenhuma função disponível para plotar.\n");
            return;
        }
    }

    int rows = HEIGHT;
    int cols = WIDTH;
    char plot[HEIGHT][WIDTH];
    
    // Inicializa a matriz de plotagem com espaços em branco
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            plot[i][j] = ' ';
        }
    }

    // Calcule o incremento no eixo X para cada coluna
    double x_increment = (h_view_hi - h_view_lo) / (cols - 1);
    if (x_increment == 0.0) {
        x_increment = 0.0001; // Garantir que o incremento seja pequeno, mas não zero
    }

    double y_range = v_view_hi - v_view_lo;

    // Desenho dos eixos
    if (draw_axis) {
        // Mapeia o eixo X (onde y=0)
        int x_axis_row = (int)((v_view_hi - 0.0) * rows / y_range);
        // Garante que o eixo X esteja dentro dos limites
        if (x_axis_row < 0) {
            x_axis_row = 0; // Garantir que não saia da parte superior
        } else if (x_axis_row >= rows) {
            x_axis_row = rows - 1; // Garantir que não saia da parte inferior
        }

        // Mapeia o eixo Y (onde x=0)
        int y_axis_col = (int)((0.0 - h_view_lo) * cols / (h_view_hi - h_view_lo));
        // Garante que o eixo Y esteja dentro dos limites
        if (y_axis_col < 0) {
            y_axis_col = 0; // Garantir que o eixo Y não saia da borda esquerda
        } else if (y_axis_col >= cols) {
            y_axis_col = cols - 1; // Garantir que o eixo Y não saia da borda direita
        }

        // Desenha o eixo X (linha correspondente a y = 0)
        if (x_axis_row >= 0 && x_axis_row < rows) {
            for (int i = 0; i < cols; i++) {
                plot[x_axis_row][i] = '-';
            }
        }

        // Desenha o eixo Y (coluna correspondente a x = 0)
        if (y_axis_col >= 0 && y_axis_col < cols) {
            for (int i = 0; i < rows; i++) {
                plot[i][y_axis_col] = '|';
            }
        }

        // Marca a origem
        if (x_axis_row >= 0 && x_axis_row < rows && y_axis_col >= 0 && y_axis_col < cols) {
            plot[x_axis_row][y_axis_col] = '+';
        }
    }

    if (!erase_plot) {
        Functions* aux = funcs_storage;
        while (aux) {
            for (int col = 0; col < cols; col++) {
                double x = h_view_lo + col * x_increment;
                change_X_value(x);
                Result* y = eval(aux->func);

                // Escala o valor de Y para caber na resolução da plotagem
                int row = (int)((v_view_hi - y->data.value) * rows / y_range);

                // Limita a posição Y para estar dentro do gráfico
                if (row >= 0 && row < rows) {
                    plot[row][col] = '*';
                }
                freeResult(y);
            }
            aux = aux->next;
        }
    } else {
        // Percorre cada coluna e calcula a posição Y da função
        for (int col = 0; col < cols; col++) {
            double x = h_view_lo + col * x_increment;
            change_X_value(x);
            Result* y = eval(func);

            // Escala o valor de Y para caber na resolução da plotagem
            int row = (int)((v_view_hi - y->data.value) * rows / y_range);

            // Limita a posição Y para estar dentro do gráfico
            if (row >= 0 && row < rows) {
                plot[row][col] = '*';
            }
            freeResult(y);
        }
    }

    // Exibe o gráfico no console
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            putchar(plot[i][j]);
        }
        putchar('\n');
    }
}


double integrate(double lower, double upper, AST_NODE* func) {
    if (!func) {
        printf("Erro: Função inválida.\n");
        return 0.0;
    }

    if (integral_steps <= 0) {
        printf("Erro: O número de subintervalos deve ser maior que zero.\n");
        exit(1);
    }

    double h = (upper - lower) / integral_steps;  // Largura de cada subintervalo
    double integral = 0.0;

    // Ajuste para a soma de Riemann à esquerda ou direita
    if (upper < lower) {
        // Ajuste direção da soma (limites invertidos)
        for (int i = 0; i < integral_steps; i++) {
            double x = upper + (i * h);  // Ponto no intervalo, ajustado para a direção
            change_X_value(x);

            Result* res = eval(func);
            integral -= res->data.value;  // Subtrai para ajustar a direção da soma
            freeResult(res);
        }
    } else {
        // Soma de Riemann à esquerda
        for (int i = 0; i < integral_steps; i++) {
            double x = lower + (i * h);  // Ponto à esquerda de cada subintervalo
            change_X_value(x);

            Result* res = eval(func);
            integral += res->data.value;  // Soma os valores da função
            freeResult(res);
        }
    }

    integral *= h;  // Multiplica pela largura do subintervalo (base dos retângulos)

    // Imprimir o valor da integral para verificar

    return integral;
}


double sum_func(const char* variable, int lower, int upper, AST_NODE* expr) {
    if (!expr) {
        printf("Erro: Expressão inválida.\n");
        return 0.0;
    }

    double sum = 0.0;
    // Itera sobre os valores do intervalo
    for (int i = lower; i <= upper; i++) {
        setNumber(variable, i); // Define o valor da variável de iteração

        Result* result = eval(expr); // Avalia a expressão
        if (result->type != TYPE_NUM) {
            printf("Erro: A expressão não é numérica.\n");
            freeResult(result);
            return 0.0;
        }

        sum += result->data.value; // Adiciona o valor ao somatório
        freeResult(result);       // Libera o resultado
    }

    return sum;
}

void about () {
    printf("+----------------------------------------------+\n");
    printf("|                                              |\n");
    printf("|                 202200560538                 |\n");
    printf("|      Francisco Barone Gasparini Mugnani      |\n");
    printf("|                                              |\n");
    printf("+----------------------------------------------+\n");
}

