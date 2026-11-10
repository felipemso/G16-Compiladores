#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"

extern int erros_semanticos;

/* Percorre a AST uma vez, populando a tabela. Zera erros_semanticos no inicio. */
void analisa(No *raiz);

/* Imprime "Erro semantico na linha N: ..." e incrementa erros_semanticos. */
void erro_semantico(int linha, const char *formato, ...);

#endif
