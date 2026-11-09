#include <stdio.h>
#include <stdlib.h>
#include "tabela.h"

typedef struct Escopo {
    int nivel;
    Simbolo *simbolos;
    struct Escopo *anterior;
} Escopo;

static Escopo escopo_global = {0, NULL, NULL};
static Escopo *escopo_atual = &escopo_global;

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
