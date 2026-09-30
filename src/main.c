#include <stdio.h>
#include <stdlib.h>

/* Declarações externas geradas pelo Flex e Bison */
extern int yyparse(void);
extern FILE *yyin;
extern int yynerrs;

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (!f) {
            fprintf(stderr, "Erro ao abrir arquivo '%s'\n", argv[1]);
            return 1;
        }
        yyin = f;
    }

    int res = yyparse();

    if (argc > 1 && yyin != stdin && yyin != NULL) {
        fclose(yyin);
    }

    if (res == 0 && yynerrs > 0) {
        res = 1;
    }

    return res;
}