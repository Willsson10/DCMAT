#ifndef MATRIX_H
#define MATRIX_H

// Definição da estrutura da matriz
typedef struct Matrix{
    int rows;        // Número de linhas
    int cols;        // Número de colunas
    int *rows_cols;
    double **data;   // Dados da matriz
} Matrix;

void setDecimalPlaces(int casas);
// Funções para manipulação de matrizes
Matrix* criaMatriz();
void adicionaLinha(Matrix *mat);
void adicionaElemento(Matrix *mat, double valor);
void showMatrix(Matrix *mat);
void freeMatrix(Matrix *mat);
Matrix* somaMatrizes(Matrix *mat1, Matrix *mat2);
Matrix* subtraiMatrizes(Matrix *mat1, Matrix *mat2);
Matrix* mulMatrizEscalar(Matrix *mat1, double escalar);
Matrix* mulMatrizMatriz(Matrix *mat1, Matrix *mat2);
void ajustaColunas(Matrix *mat);
Matrix* copiaMatriz(const Matrix* original);
double determinante(Matrix* mat);
Matrix* solveLinearSystem(Matrix* mat);

#endif // MATRIX_H
