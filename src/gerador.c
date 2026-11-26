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

/* Percorre blocos soltos e comandos vazios procurando exatamente um comando efetivo. */
static int busca_unico_comando(const No *no, const No **unico) {
    if (!no || no->categoria == NO_VAZIO) {
        return 1;
    }
    if (no->categoria == NO_BLOCO) {
        for (const No *item = no->filhos[0]; item; item = item->proximo) {
            if (!busca_unico_comando(item, unico)) {
                return 0;
            }
        }
        return 1;
    }
    if (*unico) {
        return 0;
    }
    *unico = no;
    return 1;
}

static const No *unico_if(const No *no) {
    const No *unico = NULL;
    return (busca_unico_comando(no, &unico) && unico && unico->categoria == NO_IF) ? unico : NULL;
}

static void gera_if(Gerador *g, const No *no) {
    const char *palavra = "if";
    while (no) {
        inicia_linha(g);
        fprintf(g->saida, "%s ", palavra);
        gera_expressao(g, no->filhos[0]);
        fputs(":", g->saida);
        termina_linha(g);
        gera_corpo(g, no->filhos[1]);

        const No *proximo_if = unico_if(no->filhos[2]);
        if (proximo_if) {
            palavra = "elif";
            no = proximo_if;
        } else {
            if (no->filhos[2]) {
                inicia_linha(g);
                fputs("else:", g->saida);
                termina_linha(g);
                gera_corpo(g, no->filhos[2]);
            }
            break;
        }
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
        case NO_VAZIO:
            break;
        case NO_DECL_VAR:
        case NO_ATRIBUICAO:
            inicia_linha(g);
            fprintf(g->saida, "%s = ", no->valor.texto);
            if (no->filhos[0]) {
                gera_expressao(g, no->filhos[0]);
            } else {
                fputs("None", g->saida);
            }
            termina_linha(g);
            break;
        case NO_IF:
            gera_if(g, no);
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
