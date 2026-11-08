%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

int yylex(void);
void yyerror(const char *s);
extern int yylineno;

/* Quantas declaracoes de funcao envolvem o ponto atual (0 = escopo global) */
static int profundidade_funcao = 0;
%}

/* Tipos semânticos suportados pelo analisador sintático */
%union {
    double valor_num;
    char *texto;
    int booleano;
    struct No *no;
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

%type <no> expressao atomo

%%

programa:
    { profundidade_funcao = 0; } lista_comandos
    ;

lista_comandos:
    /* vazio: programa pode iniciar vazio */
  | lista_comandos comando
  ;

bloco:
    LBRACE lista_comandos RBRACE
  ;

comando:
    declaracao_var
  | comando_atribuicao
  | declaracao_funcao
  | comando_retorno
  | chamada_funcao SEMICOLON
  | PRINT LPAREN args_opt RPAREN SEMICOLON { printf("AST: Comando de impressao reconhecido.\n"); }
  | comando_if
  | comando_while
  | comando_for
  | bloco
  | SEMICOLON
  | error SEMICOLON {
        yyerrok;
        yyclearin;
        fprintf(stderr, "[RECUPERACAO] Erro sintatico descartado ate ';'\n");
    }
  | error RBRACE {
        yyerrok;
        yyclearin;
        fprintf(stderr, "[RECUPERACAO] Erro em bloco descartado ate '}'\n");
    }
  ;

declaracao_var:
    tipo_declarador lista_declaradores SEMICOLON
  | CONST lista_declaradores_const SEMICOLON
  ;

tipo_declarador:
    LET
  | VAR
  ;

lista_declaradores:
    item_declarador
  | lista_declaradores COMMA item_declarador
  ;

item_declarador:
    ID {
        printf("AST: Declaracao de variavel '%s' reconhecida.\n", $1);
        free($1);
    }
  | item_declarador_inicializado
  ;

/* const exige inicializacao em todos os itens (const x; e erro de sintaxe no JS) */
lista_declaradores_const:
    item_declarador_inicializado
  | lista_declaradores_const COMMA item_declarador_inicializado
  ;

item_declarador_inicializado:
    ID ASSIGN expressao {
        printf("AST: Declaracao de variavel '%s' reconhecida.\n", $1);
        free($1);
    }
  ;

comando_atribuicao:
    ID ASSIGN expressao SEMICOLON {
        printf("AST: Atribuicao a variavel '%s' reconhecida.\n", $1);
        free($1);
    }
  ;

declaracao_funcao:
    FUNCTION ID LPAREN params_opt RPAREN { profundidade_funcao++; } bloco {
        profundidade_funcao--;
        printf("AST: Declaracao de funcao '%s' reconhecida.\n", $2);
        free($2);
    }
  ;

params_opt:
    /* vazio: funcao sem parametros */
  | lista_params
  ;

lista_params:
    ID { free($1); }
  | lista_params COMMA ID { free($3); }
  ;

comando_retorno:
    RETURN SEMICOLON {
        if (profundidade_funcao == 0) { yyerror("'return' fora de funcao"); yynerrs++; }
        else printf("AST: Comando de retorno reconhecido.\n");
    }
  | RETURN expressao SEMICOLON {
        if (profundidade_funcao == 0) { yyerror("'return' fora de funcao"); yynerrs++; }
        else printf("AST: Comando de retorno reconhecido.\n");
    }
  ;

chamada_funcao:
    ID LPAREN args_opt RPAREN {
        printf("AST: Chamada de funcao '%s' reconhecida.\n", $1);
        free($1);
    }
  ;

args_opt:
    /* vazio: chamada sem argumentos */
  | lista_args
  ;

lista_args:
    expressao
  | lista_args COMMA expressao
  ;

comando_if:
    IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE {
        printf("AST: Comando if reconhecido.\n");
    }
  | IF LPAREN expressao RPAREN comando ELSE comando {
        printf("AST: Comando if-else reconhecido.\n");
    }
  ;

comando_while:
    WHILE LPAREN expressao RPAREN comando {
        printf("AST: Comando while reconhecido.\n");
    }
  ;

comando_for:
    FOR LPAREN for_init SEMICOLON for_cond SEMICOLON for_incr RPAREN comando {
        printf("AST: Comando for reconhecido.\n");
    }
  ;

for_init:
    /* vazio */
  | tipo_declarador lista_declaradores
  | ID ASSIGN expressao { free($1); }
  ;

for_cond:
    /* vazio */
  | expressao
  ;

for_incr:
    /* vazio */
  | ID ASSIGN expressao { free($1); }
  | expressao
  ;

expressao:
    expressao OR expressao
  | expressao AND expressao
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
  | MINUS expressao %prec UMINUS { ast_libera($2); $$ = NULL; }
  | NOT expressao { ast_libera($2); $$ = NULL; }
  | LPAREN expressao RPAREN { $$ = $2; }
  | atomo { $$ = $1; }
  ;

atomo:
    ID { $$ = no_id($1, @1.first_line); }
  | chamada_funcao { $$ = NULL; }
  | NUM { $$ = no_numero($1, @1.first_line); }
  | STRING_VAL { $$ = no_string($1, @1.first_line); }
  | BOOLEAN_VAL { $$ = no_booleano($1, @1.first_line); }
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", yylineno, s);
}
