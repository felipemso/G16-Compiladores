#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

typedef struct Escopo {
    int nivel;
    Simbolo *simbolos;
    struct Escopo *anterior;
} Escopo;

static Escopo escopo_global = {0, NULL, NULL};
static Escopo *escopo_atual = &escopo_global;

/* Todos os simbolos em ordem de insercao: sobrevivem ao fim do escopo para a impressao */
static Simbolo *primeiro_simbolo = NULL;
static Simbolo *ultimo_simbolo = NULL;

static Simbolo *busca_no_escopo(const Escopo *escopo, const char *nome) {
    for (Simbolo *simbolo = escopo->simbolos; simbolo; simbolo = simbolo->proximo_no_escopo) {
        if (strcmp(simbolo->nome, nome) == 0) {
            return simbolo;
        }
    }
    return NULL;
}

void escopo_entra(void) {
    Escopo *escopo = calloc(1, sizeof(Escopo));
    if (!escopo) {
        fprintf(stderr, "Erro: memoria insuficiente para a tabela de simbolos\n");
        exit(1);
    }
    escopo->nivel = escopo_atual->nivel + 1;
    escopo->anterior = escopo_atual;
    escopo_atual = escopo;
}

void escopo_sai(void) {
    if (escopo_atual == &escopo_global) {
        return;
    }
    Escopo *encerrado = escopo_atual;
    escopo_atual = encerrado->anterior;
    free(encerrado);
}

int escopo_nivel(void) {
    return escopo_atual->nivel;
}

Simbolo *tabela_insere(const char *nome, CategoriaSimbolo categoria, TipoDado tipo, int linha) {
    if (tabela_busca_local(nome)) {
        return NULL;
    }
    Simbolo *simbolo = calloc(1, sizeof(Simbolo));
    char *copia = strdup(nome);
    if (!simbolo || !copia) {
        fprintf(stderr, "Erro: memoria insuficiente para a tabela de simbolos\n");
        exit(1);
    }
    simbolo->nome = copia;
    simbolo->categoria = categoria;
    simbolo->tipo_inferido = tipo;
    simbolo->linha = linha;
    simbolo->nivel_escopo = escopo_atual->nivel;

    simbolo->proximo_no_escopo = escopo_atual->simbolos;
    escopo_atual->simbolos = simbolo;

    if (ultimo_simbolo) {
        ultimo_simbolo->proximo_na_tabela = simbolo;
    } else {
        primeiro_simbolo = simbolo;
    }
    ultimo_simbolo = simbolo;
    return simbolo;
}

Simbolo *tabela_busca_local(const char *nome) {
    return busca_no_escopo(escopo_atual, nome);
}

Simbolo *tabela_busca(const char *nome) {
    for (const Escopo *escopo = escopo_atual; escopo; escopo = escopo->anterior) {
        Simbolo *simbolo = busca_no_escopo(escopo, nome);
        if (simbolo) {
            return simbolo;
        }
    }
    return NULL;
}
