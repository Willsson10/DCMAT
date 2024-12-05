%{

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "symbol_table.h"
#include "ast.h"
#include "eval.h"
#include "functions.h"
#include "matrix.h"

extern int yylex();
void yyerror(const char *s);
int yywrap () { return 1; };
extern void yylex_destroy();
Matrix *currentMatrix;
Matrix *func_matrix;
char* constanteSum;
bool variavel_somatorio = false;

%}

%union {
    struct Matrix* matrix;
    double *row;
    double val;
    char* name;
    AST_NODE* node;
}

%token <val> NUMBER
%token <name> IDENTIFIER MATRIX
%type <node> expression term factor base
%type <node> function termFunction factorFunction baseFunction
%type <node> rpn termRpn factorRpn baseRpn
%type <node> sum termSum factorSum baseSum
%type <val> onOff

%token ABOUT
%token ABS
%token AXIS
%token CONNECT_DOTS
%token COS
%token DETERMINANT
%token ERASE
%token E
%token FLOAT
%token H_VIEW
%token INTEGRAL_STEPS
%token INTEGRATE
%token LINEAR_SYSTEM
%token OFF
%token ON
%token PI
%token PLOT
%token PRECISION
%token QUIT
%token RESET
%token RPN 
%token SEN
%token SET
%token SETTINGS
%token SHOW
%token SOLVE
%token SUM 
%token SYMBOLS
%token TAN
%token V_VIEW
%token X
%token PLUS 
%token MINUS 
%token MULT 
%token DIV 
%token POW
%token PERCENT
%token LPAREN 
%token RPAREN
%token ASSIGN
%token COLON
%token EQUAL
%token RBRACKET
%token LBRACKET
%token SEMICOLON
%token COMMA
%token EOL

%left PLUS MINUS
%left MULT DIV PERCENT
%right POW
%nonassoc UMINUS
%nonassoc UPLUS

%start start

%%

start:
                             
    | start EOL                 {   printf("> "); }
    | start input EOL           {   printf("> "); }
    | error EOL                 {   yyerrok; printf("> "); }
    ;

input:
    SHOW showOptions SEMICOLON
    | RESET SETTINGS SEMICOLON                                                      {   reset_settings(); }
    | QUIT                                                                          {
                                                                                        if (currentMatrix) freeMatrix(currentMatrix);
                                                                                        if (func_matrix) freeMatrix(func_matrix);
                                                                                        free_func_atual();
                                                                                        free_all_func();
                                                                                        freeVariables(); 
                                                                                        yylex_destroy();
                                                                                        exit(0);
                                                                                    }
    | SET setOptions SEMICOLON
    | PLOT plotOptions SEMICOLON                            
    | RPN LPAREN rpn RPAREN SEMICOLON                                               {
                                                                                        printf("\nExpression in RPN format:\n\n");

                                                                                        rpn_eval($3);
                                                                                        printf("\n");
                                                                                        free_tree($3);
                                                                                    }
    | INTEGRATE LPAREN expression COLON expression COMMA function RPAREN SEMICOLON  {
                                                                                        double lower = get_num_value(eval($3));
                                                                                        double upper = get_num_value(eval($5));

                                                                                        printf("\n%.*f\n\n", get_float_precision(), integrate(lower, upper, $7));
                                                                                    }
    | SUM LPAREN IDENTIFIER COMMA   { 
                                        if (constanteSum) constanteSum = NULL;
                                        constanteSum = strdup($3);
                                        size_t size = strlen(constanteSum) + strlen("--variavel_somatorio--") + 1;
                                        char* temp = realloc(constanteSum, size);
                                        if (!temp) {
                                            yyerror("Erro de alocação de memória");
                                            YYERROR;
                                        }
                                        constanteSum = temp;
                                        strcat(constanteSum, "--variavel_somatorio--");

                                        setNumber(constanteSum, 0);
                                    }   sumContinuation SEMICOLON         
    | MATRIX EQUAL LBRACKET matrix RBRACKET SEMICOLON                               {
                                                                                        if(func_matrix){
                                                                                            freeMatrix(func_matrix);
                                                                                            func_matrix = NULL;
                                                                                        }
                                                                                        ajustaColunas(currentMatrix);
                                                                                        func_matrix = currentMatrix; 
                                                                                        currentMatrix = NULL;
                                                                                    }
    | SOLVE solveOptions SEMICOLON                          
    | ABOUT SEMICOLON                                                               {   about(); }
    | IDENTIFIER ASSIGN expression SEMICOLON                                        {
                                                                                        Result* result = eval($3);

                                                                                        if (result->type == TYPE_NUM) {
                                                                                            setNumber($1, (double)result->data.value);
                                                                                            printf("%.*f\n", get_float_precision(), get_num_value(result));
                                                                                        }
                                                                                        else if (result->type == TYPE_MAT && result->data.matrix) {
                                                                                            currentMatrix = copiaMatriz(result->data.matrix);
                                                                                            ajustaColunas(currentMatrix);
                                                                                            setMatrix($1, currentMatrix);
                                                                                            showMatrix(currentMatrix);
                                                                                            currentMatrix = NULL;
                                                                                        }
                                                                                        freeResult(result);
                                                                                        free($1);
                                                                                        free_tree($3);
                                                                                    }
    | IDENTIFIER ASSIGN LBRACKET matrix RBRACKET SEMICOLON                          {
                                                                                        ajustaColunas(currentMatrix);
                                                                                        setMatrix($1, currentMatrix);
                                                                                        showMatrix(currentMatrix);
                                                                                        currentMatrix = NULL;
                                                                                        free($1); 
                                                                                    }
    | IDENTIFIER SEMICOLON                                                          {
                                                                                        double valor;
                                                                                        Matrix* matrixAux = NULL;

                                                                                        if (getNumber($1, &valor)) {
                                                                                            printf("%s = %.*f\n", $1, get_float_precision(), get_num_value(eval(new_var($1, 0))));
                                                                                        }
                                                                                        else if (getMatrix($1, &matrixAux)) {
                                                                                            Result* result = eval(new_var($1, 1));
                                                                                            showMatrix(result->data.matrix);    
                                                                                            freeResult(result);
                                                                                        }
                                                                                        else {
                                                                                            printf("Variável não definida %s \n", $1);
                                                                                            YYERROR;
                                                                                        }
                                                                                        matrixAux = NULL;
                                                                                    }
    | expression                                                                    {  
                                                                                        Result* result = eval($1);

                                                                                        if (result->type == TYPE_NUM) {
                                                                                            printf("%.*f\n", get_float_precision(),result->data.value);
                                                                                        }
                                                                                        else if (result->type == TYPE_MAT && result->data.matrix) {
                                                                                             showMatrix(result->data.matrix);
                                                                                        }

                                                                                        freeResult(result);
                                                                                        free_tree($1);
                                                                                    }

showOptions:
    SETTINGS    {
                    printf("\n");
                    show_settings();
                    printf("\n");
                }
    | SYMBOLS   { showVariables(); }  
    | MATRIX    { 
                    if (func_matrix) showMatrix(func_matrix);
                    else printf("No Matrix defined!\n");
                }
    ;

setOptions: 
    H_VIEW expression COLON expression      {
                                                if (get_num_value(eval($2)) < get_num_value(eval($4))) {
                                                    set_h_view(get_num_value(eval($2)), get_num_value(eval($4)));
                                                }
                                                else {
                                                    yyerror(" h_view_lo must be smaller than h_view_hi");
                                                    YYERROR;
                                                }  
                                                free_tree($2);
                                                free_tree($4);
                                            }
    | V_VIEW expression COLON expression    {
                                                if (get_num_value(eval($2)) < get_num_value(eval($4))) {
                                                    set_v_view(get_num_value(eval($2)), get_num_value(eval($4)));
                                                }
                                                else {
                                                    yyerror(" v_view_lo must be smaller than v_view_hi");
                                                    YYERROR;
                                                }   
                                                free_tree($2);
                                                free_tree($4);
                                            }
    | AXIS onOff                            {   set_axis($2); }
    | ERASE PLOT onOff                      {   set_erase_plot($3); }
    | INTEGRAL_STEPS expression             {
                                                if (get_num_value(eval($2)) <= 0 || get_num_value(eval($2)) != floor(get_num_value(eval($2)))) {
                                                yyerror("integral_steps must be a positive non-zero integer");
                                                YYERROR;
                                                }
                                                else set_integral_steps(get_num_value(eval($2)));
                                                free_tree($2);
                                            }
    | FLOAT PRECISION expression            {
                                                if (get_num_value(eval($3)) < 0 || get_num_value(eval($3)) > 8 || get_num_value(eval($3)) != floor(get_num_value(eval($3)))) {
                                                    yyerror("float precision must be from 0 to 8");
                                                    YYERROR;
                                                }
                                                else set_float_precision(get_num_value(eval($3)));
                                                free_tree($3);
                                            }
    | CONNECT_DOTS onOff                    {}
    ;                        

solveOptions:
    DETERMINANT     {
                        if (func_matrix) printf("%g\n",determinante(func_matrix));
                        else printf("No Matrix defined!\n"); 
                    }
    | LINEAR_SYSTEM {
                        if (func_matrix) {
                            Matrix* aux = solveLinearSystem(func_matrix);
                            if (aux) {
                                printf("\nMatrix x:\n\n");
                                showMatrix(aux);
                                printf("\n");
                                free(aux);
                            }
                        }
                        else printf("No Matrix defined!\n");
                    }
    ;

plotOptions:
    LPAREN function RPAREN  {
                                if (get_erase_plot() == 1) set_func_atual($2);
                                else store_func($2);

                                plot_func($2);
                            }
    |                       {
                                if (!get_func_atual()) {
                                    printf("\nNo function defined!\n\n");
                                }
                                else plot_func(get_func_atual());

                            }
    ;

sumContinuation:
    expression COLON expression COMMA sum RPAREN    {
                                                        if (variavel_somatorio) {
                                                            int lower = get_num_value(eval($1));
                                                            int upper = get_num_value(eval($3));

                                                            printf("\n%.*f\n\n", get_float_precision(), sum_func(constanteSum, lower, upper, $5));
                                                            if (constanteSum) {
                                                                removeVariable(constanteSum);
                                                                free(constanteSum);
                                                                constanteSum = NULL;
                                                            }
                                                            variavel_somatorio = false;
                                                        }
                                                        else {
                                                            if (constanteSum) {
                                                                removeVariable(constanteSum);
                                                                free(constanteSum);
                                                                constanteSum = NULL;
                                                            }
                                                            yyerror("Variavel incompativel");
                                                            YYERROR;
                                                        }

                                                    }
    ;

onOff:
    ON      {   $$ = 1; }
    | OFF   {   $$ = 0; }
    ;

matrix:
    LBRACKET expression {
                            if (!currentMatrix) currentMatrix = criaMatriz();
                            adicionaLinha(currentMatrix);
                            adicionaElemento(currentMatrix, get_num_value(eval($2)));
                            
                        }  matrixValues RBRACKET matrixRows
    ;

matrixValues:
    COMMA expression {   adicionaElemento(currentMatrix, get_num_value(eval($2))); } matrixValues
    |                               
    ;

matrixRows:
    | COMMA matrix
    ;

expression:
    term                        {   $$ = $1; }
    | expression PLUS term      {   $$ = new_op($1, '+', $3); }
    | expression MINUS term     {   $$ = new_op($1, '-', $3); }
    ;

term:
    factor                  {   $$ = $1; }
    | term MULT factor      {   $$ = new_op($1, '*', $3); }
    | term DIV factor       {   $$ = new_op($1, '/', $3); }
    | term PERCENT factor   {   $$ = new_op($1, '%', $3); }
    ;

factor:
    base                            {   $$ = $1; }
    | base POW factor               {   $$ = new_op($1, '^', $3); }
    | MINUS factor %prec UMINUS     {   $$ = new_UNop('-', $2); }
    | PLUS factor %prec UPLUS       {   $$ = new_UNop('+', $2); }
    ;

base:
    NUMBER                          {   $$ = new_num($1); }
    | PI                            {   $$ = new_num(M_PI); }
    | E                             {   $$ = new_num(M_E); }
    | LPAREN expression RPAREN      {   $$ = $2; }
    | IDENTIFIER                    {
                                        double valor;
                                        Matrix* matrixAux = NULL;

                                        if (getNumber($1, &valor)) $$ = new_var($1, 0);
                                        else if (getMatrix($1, &matrixAux)) $$ = new_var($1, 1);
                                        else {
                                            printf("Variável não definida %s \n", $1);
                                            YYERROR;
                                        }
                                        matrixAux = NULL;
                                    }
    | ABS LPAREN expression RPAREN  {   $$ = new_trig('a', $3); }
    ;

function:
    termFunction                    {   $$ = $1; }
    | function PLUS termFunction    {   $$ = new_op($1, '+', $3); }
    | function MINUS termFunction   {   $$ = new_op($1, '-', $3); }
    ;

termFunction:
    factorFunction                          {   $$ = $1; }
    | termFunction MULT factorFunction      {   $$ = new_op($1, '*', $3); }
    | termFunction DIV factorFunction       {   $$ = new_op($1, '/', $3); }
    | termFunction PERCENT factorFunction   {   $$ = new_op($1, '%', $3); }
    ;

factorFunction:
    baseFunction                        {   $$ = $1; }
    | baseFunction POW factorFunction   {   $$ = new_op($1, '^', $3); }
    | MINUS factorFunction %prec UMINUS {   $$ = new_UNop('-', $2); }
    | PLUS factorFunction %prec UPLUS   {   $$ = new_UNop('+', $2); }
    ;

baseFunction:
    NUMBER                          {   $$ = new_num($1); }
    | PI                            {   $$ = new_num(M_PI); }
    | E                             {   $$ = new_num(M_E); }
    | LPAREN function RPAREN        {   $$ = $2; }
    | IDENTIFIER                    {
                                        double value;

                                        if (!getNumber($1, &value)) {
                                            yyerror("Variável não definida, ou do tipo matriz.");
                                            YYERROR;
                                        }

                                        $$ = new_var($1, 0);
                                    }
    | X                             {   $$ = new_X(); }
    | SEN LPAREN function RPAREN    {   $$ = new_trig('s', $3); }
    | COS LPAREN function RPAREN    {   $$ = new_trig('c', $3); }
    | TAN LPAREN function RPAREN    {   $$ = new_trig('t', $3); }
    | ABS LPAREN function RPAREN  {   $$ = new_trig('a', $3); }
    ;

rpn:
    termRpn             {   $$ = $1; }
    | rpn PLUS termRpn  {   $$ = new_op($1, '+', $3); }
    | rpn MINUS termRpn {   $$ = new_op($1, '-', $3); }
    ;

termRpn:
    factorRpn                   {   $$ = $1; }
    | termRpn MULT factorRpn    {   $$ = new_op($1, '*', $3); }
    | termRpn DIV factorRpn     {   $$ = new_op($1, '/', $3); }
    | termRpn PERCENT factorRpn {   $$ = new_op($1, '%', $3); }
    ;

factorRpn:
    baseRpn                         {   $$ = $1; }
    | baseRpn POW factorRpn         {   $$ = new_op($1, '^', $3); }
    | MINUS factorRpn %prec UMINUS  {   $$ = new_UNop('-', $2); }
    | PLUS factorRpn %prec UPLUS    {   $$ = new_UNop('+', $2); }
    ; 

baseRpn:
    NUMBER                      {   $$ = new_num($1); }
    | PI                        {   $$ = new_num(M_PI); }
    | E                         {   $$ = new_num(M_E); }
    | LPAREN rpn RPAREN         {   $$ = $2; }
    | IDENTIFIER                {   $$ = new_var($1, 2); }
    | X                         {   $$ = new_X(); }
    | SEN LPAREN rpn RPAREN     {   $$ = new_trig('s', $3); }
    | COS LPAREN rpn RPAREN     {   $$ = new_trig('c', $3); }
    | TAN LPAREN rpn RPAREN     {   $$ = new_trig('t', $3); }
    | ABS LPAREN rpn RPAREN     {   $$ = new_trig('a', $3); }
    ;

sum:
    termSum             {   $$ = $1; }
    | sum PLUS termSum  {   $$ = new_op($1, '+', $3); }
    | sum MINUS termSum {   $$ = new_op($1, '-', $3); }
    ;

termSum:
    factorSum                   {   $$ = $1; }
    | termSum MULT factorSum    {   $$ = new_op($1, '*', $3); }
    | termSum DIV factorSum     {   $$ = new_op($1, '/', $3); }
    | termSum PERCENT factorSum {   $$ = new_op($1, '%', $3); }
    ;

factorSum:
    baseSum                         {   $$ = $1; }
    | baseSum POW factorSum         {   $$ = new_op($1, '^', $3); }
    | MINUS factorSum %prec UMINUS  {   $$ = new_UNop('-', $2); }
    | PLUS factorSum %prec UPLUS    {   $$ = new_UNop('+', $2); }
    ;

baseSum:
    NUMBER                  {   $$ = new_num($1); }
    | PI                    {   $$ = new_num(M_PI); }
    | E                     {   $$ = new_num(M_E); }
    | LPAREN sum RPAREN     {   $$ = $2; }
    | IDENTIFIER            {
                                double valor;

                                char* aux = strdup($1);
                                size_t size = strlen(aux) + strlen("--variavel_somatorio--") + 1;
                                char* temp = realloc(aux, size);
                                if (!temp) {
                                    yyerror("Erro de alocação de memória");
                                    YYERROR;
                                }
                                aux = temp;
                                strcat(aux, "--variavel_somatorio--");
        
                                if (getNumber(aux, &valor)) {
                                    $$ = new_var(aux, 0);
                                    variavel_somatorio = true;
                                }
                                else if (getNumber($1, &valor)) {
                                    $$ = new_var($1, 0);
                                }
                                else {
                                    if (constanteSum) {
                                        removeVariable(constanteSum);
                                        free(constanteSum);
                                        constanteSum = NULL;
                                    }
                                    yyerror("Variavel nao definida");
                                    YYERROR;
                                }
                                aux = NULL;
                                free(aux);
                            }
    | ABS LPAREN sum RPAREN {   $$ = new_trig('a', $3); }
    ;

%%
void yyerror(const char *s) {
    if(currentMatrix)freeMatrix(currentMatrix);
    currentMatrix = NULL;
    fprintf(stderr, "ERROR: %s\n", s);
}
