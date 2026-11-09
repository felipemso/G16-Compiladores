#ifndef TABELA_H
#define TABELA_H

#include "ast.h"

typedef enum {
    SIMBOLO_LET,
    SIMBOLO_VAR,
    SIMBOLO_CONST,
    SIMBOLO_FUNCAO,
    SIMBOLO_PARAMETRO
} CategoriaSimbolo;

typedef struct Simbolo {
    char *nome;
    CategoriaSimbolo categoria;
    TipoDado tipo_inferido;
    int linha;
    int num_parametros;
    int nivel_escopo;
    struct Simbolo *proximo_no_escopo;
    struct Simbolo *proximo_na_tabela;
} Simbolo;

void escopo_entra(void);
void escopo_sai(void);
int escopo_nivel(void);

#endif
