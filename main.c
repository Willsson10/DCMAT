#include <stdio.h>

extern int yyparse();

int main() {
    printf("Calculadora\n> ");

    yyparse();

    return 0;
}
