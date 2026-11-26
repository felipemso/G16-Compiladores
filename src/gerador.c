#include <stdio.h>
#include "gerador.h"

typedef struct {
    FILE *saida;
    int nivel;
    int linhas;
} Gerador;

static void gera_comando(Gerador *g, const No *no);

static void inicia_linha(Gerador *g) {
    for (int i = 0; i < g->nivel; i++) {
        fputs("    ", g->saida);
    }
}

static void termina_linha(Gerador *g) {
    fputc('\n', g->saida);
    g->linhas++;
}

static void gera_lista(Gerador *g, const No *lista) {
    for (const No *no = lista; no; no = no->proximo) {
        gera_comando(g, no);
    }
}

static void gera_corpo(Gerador *g, const No *corpo) {
    int antes = g->linhas;
    g->nivel++;
    gera_comando(g, corpo);
    if (g->linhas == antes) {
        inicia_linha(g);
        fputs("pass", g->saida);
        termina_linha(g);
    }
    g->nivel--;
}

static void gera_expressao(Gerador *g, const No *no) {
    if (!no) {
        return;
    }
    switch (no->categoria) {
        case NO_ID:
        case NO_STRING:
            fputs(no->valor.texto, g->saida);
            break;
        case NO_NUMERO:
            fprintf(g->saida, "%.15g", no->valor.numero);
            break;
        case NO_BOOLEANO:
            fputs(no->valor.booleano ? "True" : "False", g->saida);
            break;
        default:
            break;
    }
}

static void gera_comando(Gerador *g, const No *no) {
    if (!no) {
        return;
    }
    switch (no->categoria) {
        case NO_BLOCO:
            gera_lista(g, no->filhos[0]);
            break;
        case NO_DECL_VAR:
            inicia_linha(g);
            fprintf(g->saida, "%s = ", no->valor.texto);
            if (no->filhos[0]) {
                gera_expressao(g, no->filhos[0]);
            } else {
                fputs("None", g->saida);
            }
            termina_linha(g);
            break;
        case NO_ATRIBUICAO:
            inicia_linha(g);
            fprintf(g->saida, "%s = ", no->valor.texto);
            gera_expressao(g, no->filhos[0]);
            termina_linha(g);
            break;
        default:
            break;
    }
}

void gera_python(const No *raiz, FILE *saida) {
    Gerador g = { saida, 0, 0 };
    if (raiz) {
        gera_lista(&g, raiz->filhos[0]);
    }
}
