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
