#include <stdarg.h>
#include <stdio.h>
#include "semantico.h"
#include "tabela.h"

int erros_semanticos = 0;

void erro_semantico(int linha, const char *formato, ...) {
    va_list argumentos;
    fprintf(stderr, "Erro semantico na linha %d: ", linha);
    va_start(argumentos, formato);
    vfprintf(stderr, formato, argumentos);
    va_end(argumentos);
    fprintf(stderr, "\n");
    erros_semanticos++;
}

static void visita(No *no);

static void visita_lista(No *lista) {
    for (No *no = lista; no; no = no->proximo) {
        visita(no);
    }
}

static void visita_filhos(No *no) {
    for (int i = 0; i < NO_MAX_FILHOS; i++) {
        visita_lista(no->filhos[i]);
    }
}

static CategoriaSimbolo categoria_do_declarador(Declarador declarador) {
    switch (declarador) {
        case DECLARADOR_VAR: return SIMBOLO_VAR;
        case DECLARADOR_CONST: return SIMBOLO_CONST;
        case DECLARADOR_LET: break;
    }
    return SIMBOLO_LET;
}

/* Uso de um nome: precisa existir no escopo atual ou em algum escopo externo */
static Simbolo *busca_uso(const char *nome, int linha) {
    Simbolo *simbolo = tabela_busca(nome);
    if (!simbolo) {
        erro_semantico(linha, "identificador '%s' nao declarado", nome);
    }
    return simbolo;
}

/* Ponto unico de entrada de nomes na tabela: variaveis, funcoes e parametros.
 * Retorna NULL quando a declaracao e invalida; o nome nao entra e o primeiro continua valendo. */
static Simbolo *declara(const char *nome, CategoriaSimbolo categoria, int linha) {
    Simbolo *anterior = tabela_busca_local(nome);
    if (anterior) {
        erro_semantico(linha, "'%s' ja foi declarado neste escopo (linha %d)", nome, anterior->linha);
        return NULL;
    }
    /* Sombreamento proibido (decisao de escopo). Excecao: parametro pode ter o nome de uma global */
    Simbolo *externo = tabela_busca(nome);
    if (externo && !(categoria == SIMBOLO_PARAMETRO && externo->nivel_escopo == 0)) {
        erro_semantico(linha, "'%s' sombreia a declaracao da linha %d em escopo externo", nome, externo->linha);
        return NULL;
    }
    return tabela_insere(nome, categoria, TIPO_DESCONHECIDO, linha);
}

static void visita_atribuicao(No *no) {
    /* O lado direito vem antes, na ordem em que seria avaliado */
    visita_filhos(no);
    busca_uso(no->valor.texto, no->linha);
}

static void visita_declaracao(No *no) {
    /* O inicializador vem antes da insercao: em "let x = x;" o x da direita ainda nao existe */
    visita_filhos(no);
    declara(no->valor.texto, categoria_do_declarador(no->declarador), no->linha);
}

static void visita_funcao(No *no) {
    /* A funcao entra antes do corpo, o que permite recursao */
    Simbolo *funcao = declara(no->valor.texto, SIMBOLO_FUNCAO, no->linha);

    escopo_entra();
    int num_parametros = 0;
    for (No *parametro = no->filhos[0]; parametro; parametro = parametro->proximo) {
        declara(parametro->valor.texto, SIMBOLO_PARAMETRO, parametro->linha);
        num_parametros++;
    }
    if (funcao) {
        funcao->num_parametros = num_parametros;
    }

    /* Parametros e corpo dividem o mesmo escopo: o bloco do corpo nao abre outro */
    No *corpo = no->filhos[1];
    if (corpo) {
        visita_lista(corpo->filhos[0]);
    }
    escopo_sai();
}

static void visita(No *no) {
    switch (no->categoria) {
        case NO_BLOCO:
        case NO_FOR:
            /* No for, o let da inicializacao fica neste escopo, por fora do bloco do corpo */
            escopo_entra();
            visita_filhos(no);
            escopo_sai();
            break;
        case NO_FUNCAO:
            visita_funcao(no);
            break;
        case NO_DECL_VAR:
            visita_declaracao(no);
            break;
        case NO_ATRIBUICAO:
            visita_atribuicao(no);
            break;
        case NO_ID:
            busca_uso(no->valor.texto, no->linha);
            break;
        default:
            /* NO_CHAMADA: o nome da funcao e checado pela T20; aqui so os argumentos */
            visita_filhos(no);
            break;
    }
}

void analisa(No *raiz) {
    erros_semanticos = 0;
    visita_lista(raiz);
}
