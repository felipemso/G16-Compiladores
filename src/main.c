#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "semantico.h"
#include "gerador.h"

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

static int grava_python(const No *raiz, const char *caminho) {
    FILE *saida = fopen(caminho, "w");
    if (!saida) {
        fprintf(stderr, "Erro ao criar arquivo '%s'\n", caminho);
        return 1;
    }
    gera_python(raiz, saida);
    fclose(saida);
    return 0;
}

int main(int argc, char *argv[]) {
    int mostrar_tabela = 0;
    int mostrar_ast = 0;
    int mostrar_python = 0;
    const char *arquivo = NULL;
    const char *arquivo_python = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--tabela") == 0) {
            mostrar_tabela = 1;
        } else if (strcmp(argv[i], "--ast") == 0) {
            mostrar_ast = 1;
        } else if (strcmp(argv[i], "--python") == 0) {
            mostrar_python = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Erro: a opcao -o exige o nome do arquivo de saida\n");
                return 1;
            }
            arquivo_python = argv[++i];
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
        /* A semantica so roda sobre uma AST completa, sem erros de sintaxe */
        analisa(raiz_ast);
        /* Impressa depois da semantica, para mostrar o tipo inferido de cada expressao */
        if (mostrar_ast) {
            ast_imprime(raiz_ast);
        }
        if (mostrar_tabela) {
            tabela_imprime();
        }
        if (erros_semanticos > 0) {
            reporta_falha("semantica", erros_semanticos);
            res = 1;
        } else {
            if (mostrar_python) {
                gera_python(raiz_ast, stdout);
            }
            if (arquivo_python && grava_python(raiz_ast, arquivo_python) != 0) {
                res = 1;
            }
        }
        tabela_libera();
    }

    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return res;
}
