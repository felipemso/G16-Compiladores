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
    gera_lista(g, corpo);
    if (g->linhas == antes) {
        inicia_linha(g);
        fputs("pass", g->saida);
        termina_linha(g);
    }
    g->nivel--;
}

static const char *operador_python(Operador operador) {
    switch (operador) {
        case OP_SOMA: return "+";
        case OP_SUBTRACAO: return "-";
        case OP_MULTIPLICACAO: return "*";
        case OP_DIVISAO: return "/";
        case OP_IGUAL: return "==";
        case OP_DIFERENTE: return "!=";
        case OP_MENOR: return "<";
        case OP_MENOR_IGUAL: return "<=";
        case OP_MAIOR: return ">";
        case OP_MAIOR_IGUAL: return ">=";
        case OP_E: return "and";
        case OP_OU: return "or";
        case OP_NEGATIVO: return "-";
        case OP_NAO: return "not ";
        default: return "";
    }
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
        case NO_BINARIO:
            gera_expressao(g, no->filhos[0]);
            fprintf(g->saida, " %s ", operador_python(no->operador));
            gera_expressao(g, no->filhos[1]);
            break;
        case NO_UNARIO:
            fputs(operador_python(no->operador), g->saida);
            gera_expressao(g, no->filhos[0]);
            break;
        default:
            break;
    }
}

/* Percorre blocos soltos e comandos vazios; devolve 0 se houver mais de um comando efetivo. */
static int coleta_ate_um_comando(const No *no, const No **unico) {
    for (; no; no = no->proximo) {
        if (no->categoria == NO_VAZIO) {
            continue;
        }
        if (no->categoria == NO_BLOCO) {
            if (!coleta_ate_um_comando(no->filhos[0], unico)) {
                return 0;
            }
            continue;
        }
        if (*unico) {
            return 0;
        }
        *unico = no;
    }
    return 1;
}

static const No *unico_if(const No *no) {
    const No *unico = NULL;
    return (coleta_ate_um_comando(no, &unico) && unico && unico->categoria == NO_IF) ? unico : NULL;
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
