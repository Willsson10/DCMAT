#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "matrix.h"
#include "functions.h"

#define EPSILON 1e-9

// Variável global para definir o número de casas decimais
int decimalPlaces = 6;

void setDecimalPlaces(int casas){
    decimalPlaces = casas;
}

// Função para criar uma nova matriz vazia
Matrix* criaMatriz() {
    Matrix *mat = malloc(sizeof(Matrix));
    if (!mat) {
        fprintf(stderr, "Erro ao alocar memória para a matriz.\n");
        exit(1);
    }
    mat->rows = 0;
    mat->cols = 0;
    mat->rows_cols = NULL;
    mat->data = NULL;
    return mat;
}

// Função para adicionar uma nova linha à matriz
void adicionaLinha(Matrix *mat) {
    mat->data = realloc(mat->data, (mat->rows + 1) * sizeof(double *));
    if (!mat->data) {
        printf("Erro ao realocar memória para as linhas.\n");
        exit(1);
    }
    //ALOCA O VETOR DE COLUNAS POR LINHA
    mat->rows_cols = realloc(mat->rows_cols, (mat->rows+1) * sizeof(double));
    mat->rows_cols[mat->rows] = 0;
    
    mat->data[mat->rows] = NULL;
    mat->rows++;
}

void adicionaElemento(Matrix *mat, double valor) {
     if (mat->rows == 0) {
        printf("Erro: Nenhuma linha foi criada antes de adicionar um elemento.\n");
        exit(1);
    }
    int lastRow = mat->rows - 1;
    int currentCols = mat->rows_cols[lastRow];

    // Adiciona o elemento à última linha
    mat->data[lastRow] = realloc(mat->data[lastRow], (currentCols + 1) * sizeof(double));
    if (!mat->data[lastRow]) {
        printf("Erro ao realocar memória para a coluna.\n");
        exit(1);
    }
    mat->data[lastRow][currentCols] = valor;

    mat->rows_cols[lastRow] = currentCols + 1;

    // Atualiza o número de colunas global se necessário
    if (currentCols + 1 > mat->cols) {
        mat->cols = currentCols + 1;
    }
}

void ajustaColunas(Matrix *mat) {
    for (int i = 0; i < mat->rows; i++) {
        if (mat->rows_cols[i] < mat->cols) {
            // Realoca a linha para o número máximo de colunas
            mat->data[i] = realloc(mat->data[i], mat->cols * sizeof(double));
            if (!mat->data[i]) {
                printf("Erro ao realocar memória para ajustar as colunas na linha %d.\n", i);
                exit(1);
            }

            // Preenche as colunas adicionais com 0
            for (int j = mat->rows_cols[i]; j < mat->cols; j++) {
                mat->data[i][j] = 0.0;
            }

            // Atualiza o número de colunas na linha
            mat->rows_cols[i] = mat->cols;
        }
    }
}

Matrix* copiaMatriz(const Matrix* original) {
    if (!original) return NULL;

    // Aloca a nova matriz
    Matrix* copia = malloc(sizeof(Matrix));
    if (!copia) {
        printf("Erro ao alocar memória para a cópia da matriz.\n");
        exit(1);
    }

    copia->rows = original->rows;
    copia->cols = original->cols;

    // Aloca espaço para as linhas
    copia->data = malloc(copia->rows * sizeof(double*));
    copia->rows_cols = malloc(copia->rows * sizeof(double));
    if (!copia->data) {
        printf("Erro ao alocar memória para as linhas da cópia da matriz.\n");
        free(copia);
        exit(1);
    }

    // Copia as linhas e os dados
    for (int i = 0; i < copia->rows; i++) {
        copia->data[i] = malloc(copia->cols * sizeof(double));
        copia->rows_cols[i] = original->rows_cols[i];
        if (!copia->data[i]) {
            printf("Erro ao alocar memória para os dados da linha %d da cópia da matriz.\n", i);
            for (int j = 0; j < i; j++) free(copia->data[j]);
            free(copia->data);
            free(copia);
            exit(1);
        }
        // Copia os valores da linha
        for (int j = 0; j < copia->cols; j++) {
            copia->data[i][j] = original->data[i][j];
        }
    }

    return copia;
}



void showMatrix(Matrix *mat) {
    // Determina o número máximo de casas decimais
    int precision = get_float_precision();

    // Determina a largura das colunas dinamicamente com base nos números
    int colWidth = 0;
    for (int i = 0; i < mat->rows; i++) {
        for (int j = 0; j < mat->cols; j++) {
            // Calcula o número de dígitos do valor absoluto
            int num_digits = (int)log10(fabs(mat->data[i][j])) + 1;
            if (mat->data[i][j] < 0) {
                num_digits++;  // Considera o sinal negativo
            }

            // Ajusta a largura da coluna para o número máximo
            int current_colWidth = num_digits + precision + 2;  // 1 para o ponto decimal e 1 para o sinal
            if (current_colWidth > colWidth) {
                colWidth = current_colWidth;
            }
        }
    }

    // Formato para imprimir com a largura ajustada
    char format[20];
    snprintf(format, sizeof(format), "%%%d.%df", colWidth, precision);

    // Impressão da linha superior
    printf("+-");
    for (int i = 0; i < mat->cols; i++) {
        for (int j = 0; j < colWidth; j++) printf(" ");
    }
    printf("-+\n");

    // Impressão das linhas da matriz
    for (int i = 0; i < mat->rows; i++) {
        printf("|"); // Sem espaço extra antes do primeiro número
        for (int j = 0; j < mat->cols; j++) {
            // Imprime o número com espaçamento fixo
            printf(format, mat->data[i][j]);
        }
        printf("  |\n"); // Espaçamento final antes da borda
    }

    // Impressão da linha inferior
    printf("+-");
    for (int i = 0; i < mat->cols; i++) {
        for (int j = 0; j < colWidth; j++) printf(" ");
    }
    printf("-+\n");
}

// Função para somar duas matrizes
Matrix* somaMatrizes(Matrix *mat1, Matrix *mat2) {
    // Verificar se as dimensões das matrizes são compatíveis
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        printf("Incorrect dimensions for operator '+' - have MATRIX [%d][%d] and MATRIX [%d][%d]\n",mat1->rows,mat1->cols,mat2->rows,mat2->cols);
        return NULL;
    }

    // Criar uma nova matriz para armazenar o resultado
    Matrix *result = criaMatriz();
    result->rows = mat1->rows;
    result->cols = mat1->cols;
    result->data = malloc(result->rows * sizeof(double*));
    result->rows_cols = malloc(mat1->rows * sizeof(int));

    for (int i = 0; i < mat1->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(double));
        result->rows_cols[i] = mat1->rows_cols[i];
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] + mat2->data[i][j];
        }
    }

    return result;
}

// Função para somar duas matrizes
Matrix* subtraiMatrizes(Matrix *mat1, Matrix *mat2) {
    // Verificar se as dimensões das matrizes são compatíveis
    if (mat1->rows != mat2->rows || mat1->cols != mat2->cols) {
        printf("Incorrect dimensions for operator '-' - have MATRIX [%d][%d] and MATRIX [%d][%d]\n",mat1->rows,mat1->cols,mat2->rows,mat2->cols);
        return NULL;
    }

    // Criar uma nova matriz para armazenar o resultado
    Matrix *result = criaMatriz();
    result->rows = mat1->rows;
    result->cols = mat1->cols;
    result->data = malloc(result->rows * sizeof(double*));
    result->rows_cols = malloc(mat1->rows * sizeof(int));

    for (int i = 0; i < mat1->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(double));
        result->rows_cols[i] = mat1->rows_cols[i];
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        }
    }

    return result;
}

// Função para somar duas matrizes
Matrix* mulMatrizEscalar(Matrix *mat1, double escalar) {
    // Criar uma nova matriz para armazenar o resultado
    Matrix *result = criaMatriz();
    result->rows = mat1->rows;
    result->cols = mat1->cols;
    result->data = malloc(result->rows * sizeof(double*));
    result->rows_cols = malloc(mat1->rows * sizeof(int));

    for (int i = 0; i < mat1->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(double));
        result->rows_cols[i] = mat1->rows_cols[i];
        for (int j = 0; j < mat1->cols; j++) {
            result->data[i][j] = mat1->data[i][j] * escalar;
        }
    }

    return result;
}

Matrix* mulMatrizMatriz(Matrix *mat1, Matrix *mat2) {
    // Verificar se as dimensões são compatíveis
    if (mat1->cols != mat2->rows) {
        printf("Incorrect dimensions for operator '*' - have MATRIX [%d][%d] and MATRIX [%d][%d]\n",mat1->rows,mat1->cols,mat2->rows,mat2->cols);
        return NULL;
    }

    // Criar a matriz resultado com dimensões (mat1->rows x mat2->cols)
    Matrix *result = criaMatriz();
    result->rows = mat1->rows;
    result->cols = mat2->cols;
    result->data = malloc(result->rows * sizeof(double*));
    result->rows_cols = malloc(mat1->rows * sizeof(int));

    // Realizar a multiplicação
    for (int i = 0; i < mat1->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(double));
        result->rows_cols[i] = mat1->rows_cols[i];
        for (int j = 0; j < mat2->cols; j++) {
            result->data[i][j] = 0; // Inicializar o valor
            for (int k = 0; k < mat1->cols; k++) {
                result->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
            }
        }
    }

    return result;
}

Matrix* submatriz(Matrix* mat, int remove_row, int remove_col) {
    // Cria a submatriz
    Matrix* sub = criaMatriz();
    sub->rows = mat->rows - 1;
    sub->cols = mat->cols - 1;
    sub->rows_cols = malloc(sub->rows * sizeof(int));
    sub->data = malloc(sub->rows * sizeof(double*));

    for (int i = 0; i < sub->rows; i++) {
        sub->data[i] = malloc(sub->cols * sizeof(double));
        sub->rows_cols[i] = sub->cols;
    }

    int subi = 0;
    for (int i = 0; i < mat->rows; i++) {
        if (i == remove_row) continue; // Pula a linha a ser removida
        int subj = 0;
        for (int j = 0; j < mat->cols; j++) {
            if (j == remove_col) continue; // Pula a coluna a ser removida
            sub->data[subi][subj] = mat->data[i][j];
            subj++;
        }
        subi++;
    }

    return sub;
}

double determinante(Matrix* mat) {
    if (mat->rows != mat->cols) {
        printf("Erro: O determinante só pode ser calculado para matrizes quadradas.\n");
        exit(1);
    }

    // Caso base: matriz 1x1
    if (mat->rows == 1) {
        return mat->data[0][0];
    }

    // Caso base: matriz 2x2
    if (mat->rows == 2) {
        return mat->data[0][0] * mat->data[1][1] - mat->data[0][1] * mat->data[1][0];
    }

    // Expansão por cofatores para matrizes maiores
    double det = 0.0;

    for (int col = 0; col < mat->cols; col++) {
        Matrix* sub = submatriz(mat, 0, col); // Gera submatriz
        double cofactor = (col % 2 == 0 ? 1 : -1) * mat->data[0][col];
        det += cofactor * determinante(sub);

        freeMatrix(sub);
    }

    return det;
}


// Função para liberar a memória da matriz
void freeMatrix(Matrix *mat) {
    // printf("FREED MATRIX\n");
    if(!mat)return;
    for (int i = 0; i < mat->rows; i++) {
        free(mat->data[i]); // Libera cada linha
    }
    free(mat->data);        // Libera o array de linhas
    free(mat->rows_cols);   // Libera o array de contagem de colunas
    mat->data = NULL;
    mat->rows_cols = NULL;
    mat->rows = 0;
    mat->cols = 0;
    free(mat);
}

Matrix* solveLinearSystem(Matrix* mat) {
    if (mat->cols <= 1) {
        printf("Matrix format incorrect!\n");
        return NULL;
    }

    int rows = mat->rows;
    int cols = mat->cols;
    int n = cols - 1; // Número de variáveis

    // Cria uma cópia da matriz para preservá-la
    Matrix* augmented = copiaMatriz(mat);

    // Eliminação de Gauss
    for (int i = 0; i < n; i++) {
        // Escolhe o pivô
        int maxRow = i;
        for (int k = i + 1; k < rows; k++) {
            if (fabs(augmented->data[k][i]) > fabs(augmented->data[maxRow][i])) {
                maxRow = k;
            }
        }

        // Troca a linha do pivô
        double* temp = augmented->data[i];
        augmented->data[i] = augmented->data[maxRow];
        augmented->data[maxRow] = temp;

        // Verifica se o pivô é zero
        if (fabs(augmented->data[i][i]) < EPSILON) {
            continue; // Pule para a próxima coluna
        }

        // Normaliza o pivô e elimina abaixo
        for (int k = i + 1; k < rows; k++) {
            double factor = augmented->data[k][i] / augmented->data[i][i];
            for (int j = i; j < cols; j++) {
                augmented->data[k][j] -= factor * augmented->data[i][j];
            }
        }
    }

    // Verifica consistência e tipo de solução
    int rank = 0; // Número de linhas com pivôs
    for (int i = 0; i < rows; i++) {
        bool allZero = true;
        for (int j = 0; j < cols - 1; j++) {
            if (fabs(augmented->data[i][j]) > EPSILON) {
                allZero = false;
                break;
            }
        }
        if (!allZero) {
            rank++;
        } else if (fabs(augmented->data[i][cols - 1]) > EPSILON) {
            freeMatrix(augmented);
            printf("SI - The Linear System has no solution\n");
            return NULL;
        }
    }

    // Se o rank for menor que o número de variáveis, temos soluções infinitas
    if (rank < n) {
        freeMatrix(augmented);
        printf("SPI - The Linear System has infinitely many solutions\n");
        return NULL;
    }

    // Substituição retroativa
    Matrix* solution = criaMatriz();
    adicionaLinha(solution); // Adiciona a linha de solução
    solution->rows = n;
    solution->cols = 1;
    solution->data = malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        solution->data[i] = malloc(sizeof(double));
    }

    // Substituição retroativa para resolver o sistema
    for (int i = n - 1; i >= 0; i--) {
        solution->data[i][0] = augmented->data[i][n];
        for (int j = i + 1; j < n; j++) {
            solution->data[i][0] -= augmented->data[i][j] * solution->data[j][0];
        }
        solution->data[i][0] /= augmented->data[i][i];
    }

    freeMatrix(augmented);
    return solution;
}


