#include <stdio.h>
#include "gerador.h"

typedef struct {
    FILE *saida;
    int nivel;
    int linhas;
} Gerador;

static void gera_comando(Gerador *g, const No *no);

static const char *AUXILIAR_PRINT =
    "def _js_str(valor):\n"
    "    if valor is None:\n"
    "        return \"undefined\"\n"
    "    if isinstance(valor, bool):\n"
    "        return \"true\" if valor else \"false\"\n"
    "    if isinstance(valor, float) and valor.is_integer() and abs(valor) < 1e21:\n"
    "        return str(int(valor))\n"
    "    return str(valor)\n"
    "\n"
    "\n"
    "def _js_print(*valores):\n"
    "    print(\" \".join(_js_str(valor) for valor in valores))\n"
    "\n"
    "\n";

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

static int precedencia(const No *no) {
    if (no->categoria == NO_UNARIO) {
        return no->operador == OP_NAO ? 3 : 7;
    }
    if (no->categoria != NO_BINARIO) {
        return 8;
    }
    switch (no->operador) {
        case OP_OU: return 1;
        case OP_E: return 2;
        case OP_SOMA:
        case OP_SUBTRACAO: return 5;
        case OP_MULTIPLICACAO:
        case OP_DIVISAO: return 6;
        default: return 4;
    }
}

static int eh_comparacao(const No *no) {
    return no->categoria == NO_BINARIO && precedencia(no) == 4;
}

static void gera_expressao(Gerador *g, const No *no);
static void gera_separados(Gerador *g, const No *lista);

static void gera_operando(Gerador *g, const No *pai, const No *filho, int direita) {
    int p_pai = precedencia(pai);
    int p_filho = precedencia(filho);
    int parenteses = p_filho < p_pai || (direita && p_filho == p_pai) ||
                     (eh_comparacao(pai) && eh_comparacao(filho));
    if (parenteses) {
        fputc('(', g->saida);
    }
    gera_expressao(g, filho);
    if (parenteses) {
        fputc(')', g->saida);
    }
}

static void gera_expressao(Gerador *g, const No *no) {
    if (!no) {
        return;
    }
    switch (no->categoria) {
        case NO_ID:
        case NO_PARAMETRO:
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
            gera_operando(g, no, no->filhos[0], 0);
            fprintf(g->saida, " %s ", operador_python(no->operador));
            gera_operando(g, no, no->filhos[1], 1);
            break;
        case NO_UNARIO:
            fputs(operador_python(no->operador), g->saida);
            gera_operando(g, no, no->filhos[0], 0);
            break;
        case NO_CHAMADA:
            fprintf(g->saida, "%s(", no->valor.texto);
            gera_separados(g, no->filhos[0]);
            fputc(')', g->saida);
            break;
        default:
            break;
    }
}

static void gera_separados(Gerador *g, const No *lista) {
    for (const No *no = lista; no; no = no->proximo) {
        gera_expressao(g, no);
        if (no->proximo) {
            fputs(", ", g->saida);
        }
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
        case NO_WHILE:
            inicia_linha(g);
            fputs("while ", g->saida);
            gera_expressao(g, no->filhos[0]);
            fputs(":", g->saida);
            termina_linha(g);
            gera_corpo(g, no->filhos[1]);
            break;
        case NO_FUNCAO:
            inicia_linha(g);
            fprintf(g->saida, "def %s(", no->valor.texto);
            gera_separados(g, no->filhos[0]);
            fputs("):", g->saida);
            termina_linha(g);
            gera_corpo(g, no->filhos[1]);
            break;
        case NO_RETURN:
            inicia_linha(g);
            fputs("return", g->saida);
            if (no->filhos[0]) {
                fputc(' ', g->saida);
                gera_expressao(g, no->filhos[0]);
            }
            termina_linha(g);
            break;
        case NO_CHAMADA:
            inicia_linha(g);
            gera_expressao(g, no);
            termina_linha(g);
            break;
        case NO_PRINT:
            inicia_linha(g);
            fputs("_js_print(", g->saida);
            gera_separados(g, no->filhos[0]);
            fputc(')', g->saida);
            termina_linha(g);
            break;
        default:
            break;
    }
}

static int usa_print(const No *no) {
    for (; no; no = no->proximo) {
        if (no->categoria == NO_PRINT) {
            return 1;
        }
        for (int i = 0; i < NO_MAX_FILHOS; i++) {
            if (usa_print(no->filhos[i])) {
                return 1;
            }
        }
    }
    return 0;
}

void gera_python(const No *raiz, FILE *saida) {
    Gerador g = { saida, 0, 0 };
    if (raiz) {
        if (usa_print(raiz->filhos[0])) {
            fputs(AUXILIAR_PRINT, saida);
        }
        gera_lista(&g, raiz->filhos[0]);
    }
}
