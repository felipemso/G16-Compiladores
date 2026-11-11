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

/* Mensagem final padronizada: informa em qual fase a compilacao falhou */
static void reporta_falha(const char *fase, int erros) {
    if (erros > 0) {
        fprintf(stderr, "Compilacao falhou na fase %s (%d erro(s))\n", fase, erros);
    } else {
        fprintf(stderr, "Compilacao falhou na fase %s\n", fase);
    }
}

int main(int argc, char *argv[]) {
    int mostrar_tabela = 0;
    int mostrar_ast = 0;
    const char *arquivo = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tabela") == 0) {
            mostrar_tabela = 1;
        } else if (strcmp(argv[i], "--ast") == 0) {
            mostrar_ast = 1;
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

    /* Erros lexicos chegam ao parser como YYUNDEF, entao as duas fases sao relatadas juntas */
    if (res != 0) {
        reporta_falha("lexica/sintatica", yynerrs);
    } else {
        if (mostrar_ast) {
            ast_imprime(raiz_ast);
        }
        /* A semantica so roda sobre uma AST completa, sem erros de sintaxe */
        analisa(raiz_ast);
        if (mostrar_tabela) {
            tabela_imprime();
        }
        if (erros_semanticos > 0) {
            reporta_falha("semantica", erros_semanticos);
            res = 1;
        }
        tabela_libera();
    }

    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return res;
}
