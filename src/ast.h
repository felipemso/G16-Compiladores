#ifndef AST_H
#define AST_H

typedef enum {
    NO_PROGRAMA,
    NO_BLOCO,
    NO_DECL_VAR,
    NO_ATRIBUICAO,
    NO_IF,
    NO_WHILE,
    NO_FOR,
    NO_FUNCAO,
    NO_PARAMETRO,
    NO_RETURN,
    NO_CHAMADA,
    NO_PRINT,
    NO_BINARIO,
    NO_UNARIO,
    NO_ID,
    NO_NUMERO,
    NO_STRING,
    NO_BOOLEANO,
    NO_VAZIO
} TipoNo;

typedef enum {
    TIPO_DESCONHECIDO,
    TIPO_NUMERO,
    TIPO_STRING,
    TIPO_BOOLEANO
} TipoDado;

typedef enum {
    OP_NENHUM,
    OP_SOMA,
    OP_SUBTRACAO,
    OP_MULTIPLICACAO,
    OP_DIVISAO,
    OP_IGUAL,
    OP_DIFERENTE,
    OP_MENOR,
    OP_MENOR_IGUAL,
    OP_MAIOR,
    OP_MAIOR_IGUAL,
    OP_E,
    OP_OU,
    OP_NEGATIVO,
    OP_NAO
} Operador;

typedef enum {
    DECLARADOR_LET,
    DECLARADOR_VAR,
    DECLARADOR_CONST
} Declarador;

#define NO_MAX_FILHOS 4

typedef struct No {
    TipoNo categoria;
    TipoDado tipo_inferido;
    int linha;
    Operador operador;
    Declarador declarador;
    union {
        double numero;
        char *texto;
        int booleano;
    } valor;
    struct No *filhos[NO_MAX_FILHOS];
    struct No *proximo;
} No;

#endif
