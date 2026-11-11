%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;

/* Quantas declaracoes de funcao envolvem o ponto atual (0 = escopo global) */
static int profundidade_funcao = 0;
%}

%code requires {
#include "ast.h"
}

%code {
No *raiz_ast = NULL;

static No *aplica_declarador(No *lista, Declarador declarador) {
    for (No *no = lista; no; no = no->proximo) {
        no->declarador = declarador;
    }
    return lista;
}
}

/* Tipos semânticos suportados pelo analisador sintático */
%union {
    double valor_num;
    char *texto;
    int booleano;
    No *no;
    Declarador declarador;
}

/* Definição dos tokens exportados para o Flex */
%token LET CONST VAR FUNCTION IF ELSE WHILE FOR
%token RETURN
%token PRINT
%token <texto> ID STRING_VAL
%token <valor_num> NUM
%token <booleano> BOOLEAN_VAL
%token ASSIGN SEMICOLON COMMA
%token PLUS MINUS TIMES DIVIDE
%token EQ NEQ LEQ GEQ LT GT AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE

%type <no> lista_comandos bloco comando declaracao_var lista_declaradores
%type <no> item_declarador lista_declaradores_const item_declarador_inicializado
%type <no> comando_atribuicao declaracao_funcao comando_retorno chamada_funcao
%type <no> args_opt lista_args comando_if comando_while comando_for
%type <no> for_init for_cond for_incr expressao atomo
%type <no> params_opt lista_params
%type <declarador> tipo_declarador
%destructor { free($$); } <texto>
%destructor { ast_libera($$); } <no>

/* Precedência de Operadores (da menor para a maior) */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left OR
%left AND
%left EQ NEQ
%left LT LEQ GT GEQ
%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS NOT

%locations

%%

programa:
    { profundidade_funcao = 0; ast_libera(raiz_ast); raiz_ast = NULL; } lista_comandos
    { raiz_ast = no_programa($2, 1); }
    ;

lista_comandos:
    /* vazio: programa pode iniciar vazio */ { $$ = NULL; }
  | lista_comandos comando { $$ = no_lista_adiciona($1, $2); }
  ;

bloco:
    LBRACE lista_comandos RBRACE { $$ = no_bloco($2, @1.first_line); }
  ;

comando:
    declaracao_var
  | comando_atribuicao
  | declaracao_funcao
  | comando_retorno
  | chamada_funcao SEMICOLON
  | PRINT LPAREN args_opt RPAREN SEMICOLON { $$ = no_print($3, @1.first_line); }
  | comando_if
  | comando_while
  | comando_for
  | bloco
  | SEMICOLON { $$ = no_vazio(@1.first_line); }
  | error SEMICOLON {
        $$ = NULL;
        yyerrok;
        fprintf(stderr, "[RECUPERACAO] Erro sintatico descartado ate ';'\n");
    }
  | error RBRACE {
        $$ = NULL;
        yyerrok;
        fprintf(stderr, "[RECUPERACAO] Erro em bloco descartado ate '}'\n");
    }
  ;

declaracao_var:
    tipo_declarador lista_declaradores SEMICOLON {
        $$ = aplica_declarador($2, $1);
    }
  | CONST lista_declaradores_const SEMICOLON {
        $$ = aplica_declarador($2, DECLARADOR_CONST);
    }
  ;

tipo_declarador:
    LET { $$ = DECLARADOR_LET; }
  | VAR { $$ = DECLARADOR_VAR; }
  ;

lista_declaradores:
    item_declarador
  | lista_declaradores COMMA item_declarador { $$ = no_lista_adiciona($1, $3); }
  ;

/* DECLARADOR_LET e provisorio; a regra pai define o declarador real via aplica_declarador */
item_declarador:
    ID {
        $$ = no_decl_var(DECLARADOR_LET, $1, NULL, @1.first_line);
    }
  | item_declarador_inicializado
  ;

/* const exige inicializacao em todos os itens (const x; e erro de sintaxe no JS) */
lista_declaradores_const:
    item_declarador_inicializado
  | lista_declaradores_const COMMA item_declarador_inicializado { $$ = no_lista_adiciona($1, $3); }
  ;

item_declarador_inicializado:
    ID ASSIGN expressao {
        $$ = no_decl_var(DECLARADOR_LET, $1, $3, @1.first_line);
    }
  ;

comando_atribuicao:
    ID ASSIGN expressao SEMICOLON {
        $$ = no_atribuicao($1, $3, @1.first_line);
    }
  ;

declaracao_funcao:
    FUNCTION ID LPAREN params_opt RPAREN { profundidade_funcao++; } bloco {
        profundidade_funcao--;
        $$ = no_funcao($2, $4, $7, @1.first_line);
    }
  ;

params_opt:
    /* vazio: funcao sem parametros */ { $$ = NULL; }
  | lista_params
  ;

lista_params:
    ID { $$ = no_parametro($1, @1.first_line); }
  | lista_params COMMA ID { $$ = no_lista_adiciona($1, no_parametro($3, @3.first_line)); }
  ;

comando_retorno:
    RETURN SEMICOLON {
        if (profundidade_funcao == 0) {
            yyerror("'return' fora de funcao");
            yynerrs++;
            $$ = NULL;
        } else {
            $$ = no_return(NULL, @1.first_line);
        }
    }
  | RETURN expressao SEMICOLON {
        if (profundidade_funcao == 0) {
            yyerror("'return' fora de funcao");
            yynerrs++;
            ast_libera($2);
            $$ = NULL;
        } else {
            $$ = no_return($2, @1.first_line);
        }
    }
  ;

chamada_funcao:
    ID LPAREN args_opt RPAREN {
        $$ = no_chamada($1, $3, @1.first_line);
    }
  ;

args_opt:
    /* vazio: chamada sem argumentos */ { $$ = NULL; }
  | lista_args
  ;

lista_args:
    expressao
  | lista_args COMMA expressao { $$ = no_lista_adiciona($1, $3); }
  ;

comando_if:
    IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE {
        $$ = no_if($3, $5, NULL, @1.first_line);
    }
  | IF LPAREN expressao RPAREN comando ELSE comando {
        $$ = no_if($3, $5, $7, @1.first_line);
    }
  ;

comando_while:
    WHILE LPAREN expressao RPAREN comando {
        ast_libera($3);
        ast_libera($5);
        $$ = NULL;
    }
  ;

comando_for:
    FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_incr RPAREN comando {
        ast_libera($3);
        ast_libera($5);
        ast_libera($7);
        ast_libera($9);
        $$ = NULL;
    }
  ;

for_init:
    /* vazio */ { $$ = NULL; }
  | tipo_declarador lista_declaradores {
        $$ = aplica_declarador($2, $1);
    }
  | ID ASSIGN expressao { free($1); ast_libera($3); $$ = NULL; }
  ;

for_cond:
    /* vazio */ { $$ = NULL; }
  | expressao
  ;

for_incr:
    /* vazio */ { $$ = NULL; }
  | ID ASSIGN expressao { free($1); ast_libera($3); $$ = NULL; }
  | expressao
  ;

expressao:
    expressao OR expressao { $$ = no_binario(OP_OU, $1, $3, @1.first_line); }
  | expressao AND expressao { $$ = no_binario(OP_E, $1, $3, @1.first_line); }
  | expressao EQ expressao { $$ = no_binario(OP_IGUAL, $1, $3, @1.first_line); }
  | expressao NEQ expressao { $$ = no_binario(OP_DIFERENTE, $1, $3, @1.first_line); }
  | expressao LT expressao { $$ = no_binario(OP_MENOR, $1, $3, @1.first_line); }
  | expressao LEQ expressao { $$ = no_binario(OP_MENOR_IGUAL, $1, $3, @1.first_line); }
  | expressao GT expressao { $$ = no_binario(OP_MAIOR, $1, $3, @1.first_line); }
  | expressao GEQ expressao { $$ = no_binario(OP_MAIOR_IGUAL, $1, $3, @1.first_line); }
  | expressao PLUS expressao { $$ = no_binario(OP_SOMA, $1, $3, @1.first_line); }
  | expressao MINUS expressao { $$ = no_binario(OP_SUBTRACAO, $1, $3, @1.first_line); }
  | expressao TIMES expressao { $$ = no_binario(OP_MULTIPLICACAO, $1, $3, @1.first_line); }
  | expressao DIVIDE expressao { $$ = no_binario(OP_DIVISAO, $1, $3, @1.first_line); }
  | MINUS expressao %prec UMINUS { $$ = no_unario(OP_NEGATIVO, $2, @1.first_line); }
  | NOT expressao { $$ = no_unario(OP_NAO, $2, @1.first_line); }
  | LPAREN expressao RPAREN { $$ = $2; }
  | atomo { $$ = $1; }
  ;

atomo:
    ID { $$ = no_id($1, @1.first_line); }
  | chamada_funcao
  | NUM { $$ = no_numero($1, @1.first_line); }
  | STRING_VAL { $$ = no_string($1, @1.first_line); }
  | BOOLEAN_VAL { $$ = no_booleano($1, @1.first_line); }
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", yylineno, s);
}
