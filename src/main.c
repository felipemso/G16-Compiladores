#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "semantico.h"

/* Declarações externas geradas pelo Flex e Bison */
extern int yyparse(void);
extern FILE *yyin;
extern int yynerrs;
extern No *raiz_ast;

int main(int argc, char *argv[]) {
    int mostrar_tabela = 0;
    const char *arquivo = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tabela") == 0) {
            mostrar_tabela = 1;
        } else {
            arquivo = argv[i];
        }
    }

    if (arquivo) {
        FILE *f = fopen(arquivo, "r");
        if (!f) {
            fprintf(stderr, "Erro ao abrir arquivo '%s'\n", arquivo);
            return 1;
        }
        yyin = f;
    }

    int res = yyparse();

    if (arquivo && yyin != stdin && yyin != NULL) {
        fclose(yyin);
    }

    if (res == 0 && yynerrs > 0) {
        res = 1;
    }

    if (res == 0 && mostrar_tabela) {
        analisa(raiz_ast);
        tabela_imprime();
        tabela_libera();
    }

    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return res;
}
