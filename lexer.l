%{
#include "ast.h"
#include "parser.tab.h"
#include "symbol_table.h"
#include <stdlib.h>
%}

%option noinput nounput

%%

"about"                 { return ABOUT; }
"abs"                   { return ABS; }
"axis"                  { return AXIS; }
"connect_dots"          { return CONNECT_DOTS; }
"cos"                   { return COS; }
"determinant"           { return DETERMINANT; }
"erase"                 { return ERASE; }
"e"|"E"                 { return E; }
"float"                 { return FLOAT; }
"h_view"                { return H_VIEW; }
"integral_steps"        { return INTEGRAL_STEPS; }
"integrate"             { return INTEGRATE; }
"linear_system"         { return LINEAR_SYSTEM; }
"matrix"                { return MATRIX; }
"off"                   { return OFF; }
"on"                    { return ON; }
"pi"|"Pi"|"pI"|"PI"     { return PI; }
"plot"                  { return PLOT; }
"precision"             { return PRECISION; }
"quit"                  { return QUIT; }
"reset"                 { return RESET; }
"rpn"                   { return RPN; }
"sen"                   { return SEN; }
"set"                   { return SET; }
"settings"              { return SETTINGS; }
"show"                  { return SHOW; }
"solve"                 { return SOLVE;}
"sum"                   { return SUM; }
"symbols"               { return SYMBOLS; }
"tan"                   { return TAN; }
"v_view"                { return V_VIEW; }
"x"|"X"                 { return X; }
"+"                     { return PLUS; }
"-"                     { return MINUS; }
"*"                     { return MULT; }
"/"                     { return DIV; }
"^"                     { return POW; }
"%"                     { return PERCENT; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
":="                    { return ASSIGN; }
":"                     { return COLON; }
"="                     { return EQUAL;}
"]"                     { return RBRACKET; }
"["                     { return LBRACKET; }
";"                     { return SEMICOLON; }
","                     { return COMMA; }
[0-9]+(\.[0-9]+)?       { yylval.val = atof(yytext); return NUMBER; }
[a-zA-Z][a-zA-Z0-9_]*    { yylval.name = strdup(yytext); return IDENTIFIER; }
\n                      { return EOL; }
[ \t]                   { /* ignorar espaços em branco */ }
.                       { printf("ERRO LEXICO\n"); }

%%
