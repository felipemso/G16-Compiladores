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
