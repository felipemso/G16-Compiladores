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

/* Retorna NULL se o nome ja existe no escopo atual. O nome e copiado. */
Simbolo *tabela_insere(const char *nome, CategoriaSimbolo categoria, TipoDado tipo, int linha);
Simbolo *tabela_busca_local(const char *nome);
Simbolo *tabela_busca(const char *nome);

/* Primeiro simbolo inserido; os demais seguem por proximo_na_tabela. */
const Simbolo *tabela_primeiro(void);

void tabela_imprime(void);
void tabela_libera(void);

#endif
