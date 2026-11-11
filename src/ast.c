#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

static No *no_novo(TipoNo categoria, int linha) {
    No *no = calloc(1, sizeof(No));
    if (!no) {
        fprintf(stderr, "Erro: memoria insuficiente para a AST\n");
        exit(1);
    }
    no->categoria = categoria;
    no->tipo_inferido = TIPO_DESCONHECIDO;
    no->operador = OP_NENHUM;
    no->linha = linha;
    return no;
}

No *no_numero(double valor, int linha) {
    No *no = no_novo(NO_NUMERO, linha);
    no->valor.numero = valor;
    return no;
}

No *no_string(char *texto, int linha) {
    No *no = no_novo(NO_STRING, linha);
    no->valor.texto = texto;
    return no;
}

No *no_booleano(int valor, int linha) {
    No *no = no_novo(NO_BOOLEANO, linha);
    no->valor.booleano = valor ? 1 : 0;
    return no;
}

No *no_id(char *nome, int linha) {
    No *no = no_novo(NO_ID, linha);
    no->valor.texto = nome;
    return no;
}

No *no_binario(Operador operador, No *esquerda, No *direita, int linha) {
    No *no = no_novo(NO_BINARIO, linha);
    no->operador = operador;
    no->filhos[0] = esquerda;
    no->filhos[1] = direita;
    return no;
}

No *no_unario(Operador operador, No *operando, int linha) {
    No *no = no_novo(NO_UNARIO, linha);
    no->operador = operador;
    no->filhos[0] = operando;
    return no;
}

No *no_decl_var(Declarador declarador, char *nome, No *inicializador, int linha) {
    No *no = no_novo(NO_DECL_VAR, linha);
    no->declarador = declarador;
    no->valor.texto = nome;
    no->filhos[0] = inicializador;
    return no;
}

No *no_atribuicao(char *nome, No *expressao, int linha) {
    No *no = no_novo(NO_ATRIBUICAO, linha);
    no->valor.texto = nome;
    no->filhos[0] = expressao;
    return no;
}

No *no_if(No *condicao, No *entao, No *senao, int linha) {
    No *no = no_novo(NO_IF, linha);
    no->filhos[0] = condicao;
    no->filhos[1] = entao;
    no->filhos[2] = senao;
    return no;
}

No *no_while(No *condicao, No *corpo, int linha) {
    No *no = no_novo(NO_WHILE, linha);
    no->filhos[0] = condicao;
    no->filhos[1] = corpo;
    return no;
}

No *no_for(No *init, No *condicao, No *incremento, No *corpo, int linha) {
    No *no = no_novo(NO_FOR, linha);
    no->filhos[0] = init;
    no->filhos[1] = condicao;
    no->filhos[2] = incremento;
    no->filhos[3] = corpo;
    return no;
}

No *no_funcao(char *nome, No *parametros, No *corpo, int linha) {
    No *no = no_novo(NO_FUNCAO, linha);
    no->valor.texto = nome;
    no->filhos[0] = parametros;
    no->filhos[1] = corpo;
    return no;
}

No *no_return(No *expressao, int linha) {
    No *no = no_novo(NO_RETURN, linha);
    no->filhos[0] = expressao;
    return no;
}

No *no_vazio(int linha) {
    return no_novo(NO_VAZIO, linha);
}

No *no_programa(No *comandos, int linha) {
    No *no = no_novo(NO_PROGRAMA, linha);
    no->filhos[0] = comandos;
    return no;
}

No *no_bloco(No *comandos, int linha) {
    No *no = no_novo(NO_BLOCO, linha);
    no->filhos[0] = comandos;
    return no;
}

No *no_parametro(char *nome, int linha) {
    No *no = no_novo(NO_PARAMETRO, linha);
    no->valor.texto = nome;
    return no;
}

No *no_chamada(char *nome, No *argumentos, int linha) {
    No *no = no_novo(NO_CHAMADA, linha);
    no->valor.texto = nome;
    no->filhos[0] = argumentos;
    return no;
}

No *no_print(No *argumentos, int linha) {
    No *no = no_novo(NO_PRINT, linha);
    no->filhos[0] = argumentos;
    return no;
}

No *no_lista_adiciona(No *lista, No *novo) {
    if (!novo) {
        return lista;
    }
    if (!lista) {
        return novo;
    }
    No *ultimo = lista;
    while (ultimo->proximo) {
        ultimo = ultimo->proximo;
    }
    ultimo->proximo = novo;
    return lista;
}

static int usa_texto(TipoNo categoria) {
    switch (categoria) {
        case NO_DECL_VAR:
        case NO_ATRIBUICAO:
        case NO_FUNCAO:
        case NO_PARAMETRO:
        case NO_CHAMADA:
        case NO_ID:
        case NO_STRING:
            return 1;
        default:
            return 0;
    }
}

static void imprime_valor(const No *no) {
    static const char *categorias[] = {
        "NO_PROGRAMA", "NO_BLOCO", "NO_DECL_VAR", "NO_ATRIBUICAO",
        "NO_IF", "NO_WHILE", "NO_FOR", "NO_FUNCAO", "NO_PARAMETRO",
        "NO_RETURN", "NO_CHAMADA", "NO_PRINT", "NO_BINARIO", "NO_UNARIO",
        "NO_ID", "NO_NUMERO", "NO_STRING", "NO_BOOLEANO", "NO_VAZIO"
    };
    static const char *operadores[] = {
        "", "+", "-", "*", "/", "==", "!=", "<", "<=", ">", ">=",
        "&&", "||", "-", "!"
    };
    static const char *declaradores[] = { "let", "var", "const" };

    printf("%s", categorias[no->categoria]);
    if (no->categoria == NO_DECL_VAR) {
        printf(" %s %s", declaradores[no->declarador], no->valor.texto);
    } else if (usa_texto(no->categoria)) {
        printf(" %s", no->valor.texto);
    } else if (no->categoria == NO_NUMERO) {
        printf(" %.15g", no->valor.numero);
    } else if (no->categoria == NO_BOOLEANO) {
        printf(" %s", no->valor.booleano ? "true" : "false");
    } else if (no->categoria == NO_BINARIO || no->categoria == NO_UNARIO) {
        printf(" %s", operadores[no->operador]);
    }
    putchar('\n');
}

/* Cada ancestral informa se ainda ha um irmao abaixo de seu ramo. */
typedef struct Prefixo {
    const struct Prefixo *anterior;
    int continua;
} Prefixo;

static void imprime_prefixo(const Prefixo *prefixo) {
    if (!prefixo) return;
    imprime_prefixo(prefixo->anterior);
    printf("%s", prefixo->continua ? "│   " : "    ");
}

static const char *papel_filho(TipoNo categoria, int indice) {
    static const char *filhos_if[] = { "condicao", "entao", "senao" };
    static const char *filhos_while[] = { "condicao", "corpo" };
    static const char *filhos_for[] = { "inicializacao", "condicao", "incremento", "corpo" };
    if (categoria == NO_IF && indice < 3) return filhos_if[indice];
    if (categoria == NO_WHILE && indice < 2) return filhos_while[indice];
    if (categoria == NO_FOR) return filhos_for[indice];
    return NULL;
}

static void imprime_arvore(const No *no, const Prefixo *prefixo,
                           int ultimo, const char *papel, int raiz) {
    if (!raiz) {
        imprime_prefixo(prefixo);
        printf("%s", ultimo ? "└── " : "├── ");
        if (papel) printf("%s: ", papel);
    }
    imprime_valor(no);

    int ultimo_filho = -1;
    for (int i = 0; i < NO_MAX_FILHOS; i++) {
        if (no->filhos[i]) ultimo_filho = i;
    }
    Prefixo proximo_prefixo = { prefixo, !ultimo };
    for (int i = 0; i <= ultimo_filho; i++) {
        for (const No *filho = no->filhos[i]; filho; filho = filho->proximo) {
            imprime_arvore(filho, raiz ? NULL : &proximo_prefixo,
                           i == ultimo_filho && !filho->proximo,
                           papel_filho(no->categoria, i), 0);
        }
    }
}

/* Filhos nulos sao omitidos; irmaos compartilham as conexoes do mesmo nivel. */
void ast_imprime(const No *no) {
    for (; no; no = no->proximo) {
        imprime_arvore(no, NULL, !no->proximo, NULL, 1);
    }
}

void ast_libera(No *no) {
    while (no) {
        No *proximo = no->proximo;
        for (int i = 0; i < NO_MAX_FILHOS; i++) {
            ast_libera(no->filhos[i]);
        }
        if (usa_texto(no->categoria)) {
            free(no->valor.texto);
        }
        free(no);
        no = proximo;
    }
}
