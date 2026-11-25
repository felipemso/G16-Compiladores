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

static void gera_comando(Gerador *g, const No *no) {
    if (!no) {
        return;
    }
    switch (no->categoria) {
        case NO_BLOCO:
            gera_lista(g, no->filhos[0]);
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
