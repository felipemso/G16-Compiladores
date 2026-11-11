#include <stdarg.h>
#include <stdbool.h>
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
static TipoDado expressao(No *no);

/* Nivel do escopo da funcao sendo analisada (0 fora de funcoes) */
static int nivel_funcao = 0;

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
static Simbolo *declara(const char *nome, CategoriaSimbolo categoria, TipoDado tipo, int linha) {
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
    return tabela_insere(nome, categoria, tipo, linha);
}

static const char *nome_tipo(TipoDado tipo) {
    switch (tipo) {
        case TIPO_NUMERO: return "numero";
        case TIPO_STRING: return "string";
        case TIPO_BOOLEANO: return "booleano";
        case TIPO_DESCONHECIDO: break;
    }
    return "desconhecido";
}

static const char *nome_operador(Operador operador) {
    switch (operador) {
        case OP_SOMA: return "+";
        case OP_SUBTRACAO:
        case OP_NEGATIVO: return "-";
        case OP_MULTIPLICACAO: return "*";
        case OP_DIVISAO: return "/";
        case OP_IGUAL: return "==";
        case OP_DIFERENTE: return "!=";
        case OP_MENOR: return "<";
        case OP_MENOR_IGUAL: return "<=";
        case OP_MAIOR: return ">";
        case OP_MAIOR_IGUAL: return ">=";
        case OP_E: return "&&";
        case OP_OU: return "||";
        case OP_NAO: return "!";
        case OP_NENHUM: break;
    }
    return "?";
}

/* Tipo de uma variavel no ponto em que e lida.
 * Dentro de uma funcao, let/var de fora contam como desconhecido: a funcao pode ser chamada
 * depois de uma reatribuicao, e o tipo antigo geraria erro falso. const nunca muda de tipo. */
static TipoDado tipo_do_simbolo(const Simbolo *simbolo) {
    if (nivel_funcao > 0 && simbolo->nivel_escopo < nivel_funcao && simbolo->categoria != SIMBOLO_CONST) {
        return TIPO_DESCONHECIDO;
    }
    return simbolo->tipo_inferido;
}

static bool conhecido(TipoDado tipo) {
    return tipo != TIPO_DESCONHECIDO;
}

/* + : soma entre numeros/booleanos ou concatenacao entre strings.
 * Misturar string com numero ou booleano e erro (decisao de escopo). */
static TipoDado tipo_soma(No *no, TipoDado esquerda, TipoDado direita) {
    if (esquerda == TIPO_STRING || direita == TIPO_STRING) {
        TipoDado outro = esquerda == TIPO_STRING ? direita : esquerda;
        if (outro == TIPO_NUMERO || outro == TIPO_BOOLEANO) {
            erro_semantico(no->linha, "operador '+' entre %s e %s (string so pode ser concatenada com string)",
                           nome_tipo(esquerda), nome_tipo(direita));
            return TIPO_DESCONHECIDO;
        }
        /* Em JS, string + qualquer coisa e string */
        return TIPO_STRING;
    }
    if (conhecido(esquerda) && conhecido(direita)) {
        return TIPO_NUMERO;
    }
    return TIPO_DESCONHECIDO;
}

static TipoDado tipo_binario(No *no) {
    TipoDado esquerda = expressao(no->filhos[0]);
    TipoDado direita = expressao(no->filhos[1]);
    switch (no->operador) {
        case OP_SOMA:
            return tipo_soma(no, esquerda, direita);
        case OP_SUBTRACAO:
        case OP_MULTIPLICACAO:
        case OP_DIVISAO:
            if (esquerda == TIPO_STRING || direita == TIPO_STRING) {
                erro_semantico(no->linha, "operador '%s' nao pode ser usado com string", nome_operador(no->operador));
            }
            /* O resultado aritmetico e sempre numero (NaN tambem e numero em JS) */
            return TIPO_NUMERO;
        case OP_IGUAL:
        case OP_DIFERENTE:
        case OP_MENOR:
        case OP_MENOR_IGUAL:
        case OP_MAIOR:
        case OP_MAIOR_IGUAL:
            if (conhecido(esquerda) && conhecido(direita) && esquerda != direita) {
                erro_semantico(no->linha, "comparacao '%s' entre tipos diferentes (%s e %s)",
                               nome_operador(no->operador), nome_tipo(esquerda), nome_tipo(direita));
            }
            return TIPO_BOOLEANO;
        case OP_E:
        case OP_OU:
            /* && e || devolvem um dos operandos: o tipo so e conhecido se os dois forem iguais */
            return esquerda == direita ? esquerda : TIPO_DESCONHECIDO;
        default:
            return TIPO_DESCONHECIDO;
    }
}

static TipoDado tipo_unario(No *no) {
    TipoDado operando = expressao(no->filhos[0]);
    if (no->operador == OP_NAO) {
        return TIPO_BOOLEANO;
    }
    if (operando == TIPO_STRING) {
        erro_semantico(no->linha, "operador '%s' nao pode ser usado com string", nome_operador(no->operador));
    }
    return TIPO_NUMERO;
}

/* Visita uma expressao, guarda o tipo inferido no proprio no e devolve esse tipo */
static TipoDado expressao(No *no) {
    TipoDado tipo = TIPO_DESCONHECIDO;
    switch (no->categoria) {
        case NO_NUMERO:
            tipo = TIPO_NUMERO;
            break;
        case NO_STRING:
            tipo = TIPO_STRING;
            break;
        case NO_BOOLEANO:
            tipo = TIPO_BOOLEANO;
            break;
        case NO_ID: {
            Simbolo *simbolo = busca_uso(no->valor.texto, no->linha);
            tipo = simbolo ? tipo_do_simbolo(simbolo) : TIPO_DESCONHECIDO;
            break;
        }
        case NO_BINARIO:
            tipo = tipo_binario(no);
            break;
        case NO_UNARIO:
            tipo = tipo_unario(no);
            break;
        default:
            /* NO_CHAMADA: o retorno de funcao conta como desconhecido; os argumentos sao visitados */
            visita_filhos(no);
            break;
    }
    no->tipo_inferido = tipo;
    return tipo;
}

static void visita_atribuicao(No *no) {
    /* O lado direito vem antes, na ordem em que seria avaliado */
    TipoDado tipo = expressao(no->filhos[0]);
    no->tipo_inferido = tipo;
    Simbolo *alvo = busca_uso(no->valor.texto, no->linha);
    if (!alvo) {
        return;
    }
    if (alvo->categoria == SIMBOLO_CONST) {
        erro_semantico(no->linha, "atribuicao a constante '%s' (declarada na linha %d)", no->valor.texto, alvo->linha);
    } else if (alvo->tipo_inferido != tipo) {
        /* Reatribuida com outro tipo: daqui em diante o tipo deixa de ser conhecido */
        alvo->tipo_inferido = TIPO_DESCONHECIDO;
    }
}

static void visita_declaracao(No *no) {
    /* O inicializador vem antes da insercao: em "let x = x;" o x da direita ainda nao existe */
    TipoDado tipo = no->filhos[0] ? expressao(no->filhos[0]) : TIPO_DESCONHECIDO;
    no->tipo_inferido = tipo;
    declara(no->valor.texto, categoria_do_declarador(no->declarador), tipo, no->linha);
}

static void visita_funcao(No *no) {
    /* A funcao entra antes do corpo, o que permite recursao */
    Simbolo *funcao = declara(no->valor.texto, SIMBOLO_FUNCAO, TIPO_DESCONHECIDO, no->linha);

    escopo_entra();
    int nivel_anterior = nivel_funcao;
    nivel_funcao = escopo_nivel();
    int num_parametros = 0;
    for (No *parametro = no->filhos[0]; parametro; parametro = parametro->proximo) {
        /* Parametros contam como desconhecido: o tipo depende de quem chama */
        declara(parametro->valor.texto, SIMBOLO_PARAMETRO, TIPO_DESCONHECIDO, parametro->linha);
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
    nivel_funcao = nivel_anterior;
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
        case NO_NUMERO:
        case NO_STRING:
        case NO_BOOLEANO:
        case NO_BINARIO:
        case NO_UNARIO:
        case NO_CHAMADA:
            /* NO_CHAMADA: o nome da funcao e checado pela T20; aqui so os argumentos */
            expressao(no);
            break;
        default:
            visita_filhos(no);
            break;
    }
}

void analisa(No *raiz) {
    erros_semanticos = 0;
    nivel_funcao = 0;
    visita_lista(raiz);
}
