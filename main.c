#include <stdio.h>

extern int yyparse();

int main() {
    printf("> ");
    yyparse();

    return 0;
}
