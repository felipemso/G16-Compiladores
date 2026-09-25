%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yylex(void);
void yyerror(const char *s);
%}

/* Tipos semânticos suportados pelo analisador sintático */
%union {
    double valor_num;
    char *texto;
    int booleano;
}

/* Definição dos tokens exportados para o Flex */
%token LET CONST VAR FUNCTION IF ELSE WHILE FOR DO SWITCH CASE
%token BREAK CONTINUE RETURN TRY CATCH THROW
%token CLASS EXTENDS IMPORT FROM EXPORT NEW IN OF DELETE
%token PRINT
%token <texto> ID STRING_VAL
%token <valor_num> NUM
%token <booleano> BOOLEAN_VAL
%token ASSIGN SEMICOLON COMMA
%token PLUS MINUS TIMES DIVIDE
%token EQ NEQ LEQ GEQ LT GT AND OR
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
%right UMINUS

%%

programa:
    lista_comandos
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
  | comando_if
  | comando_while
  | comando_for
  | PRINT LPAREN expressao RPAREN SEMICOLON { printf("AST: Comando de impressao reconhecido.\n"); }
  | bloco
  | SEMICOLON
  | error SEMICOLON { yyerrok; /* Permite que o parser se recupere de erros após um ponto e vírgula */ }
  ;

declaracao_var:
    tipo_declarador lista_declaradores SEMICOLON
  ;

tipo_declarador:
    LET
  | CONST
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
  | ID ASSIGN expressao {
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
  | expressao EQ expressao
  | expressao NEQ expressao
  | expressao LT expressao
  | expressao LEQ expressao
  | expressao GT expressao
  | expressao GEQ expressao
  | expressao PLUS expressao
  | expressao MINUS expressao
  | expressao TIMES expressao
  | expressao DIVIDE expressao
  | MINUS expressao %prec UMINUS
  | LPAREN expressao RPAREN
  | atomo
  ;

atomo:
    ID { free($1); }
  | NUM
  | STRING_VAL { free($1); }
  | BOOLEAN_VAL
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico: %s\n", s);
}