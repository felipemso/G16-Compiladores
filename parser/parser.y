%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* Definição dos tokens exportados para o Flex */
%token LET CONST VAR FUNCTION IF ELSE WHILE FOR DO SWITCH CASE
%token BREAK CONTINUE RETURN TRY CATCH THROW
%token CLASS EXTENDS IMPORT FROM EXPORT NEW IN OF DELETE
%token PRINT
%token ID NUM BOOLEAN_VAL STRING_VAL
%token ASSIGN SEMICOLON COMMA
%token PLUS MINUS TIMES DIVIDE
%token EQ NEQ LEQ GEQ LT GT AND OR
%token LPAREN RPAREN LBRACE RBRACE

/* Regras de precedência para evitar avisos Shift/Reduce */
%left PLUS MINUS
%left TIMES DIVIDE

%%

programa:
    comandos
    ;

comandos:
    comando
    | comandos comando
    ;

comando:
    LET ID ASSIGN expressao SEMICOLON { printf("AST: Declaracao de variavel reconhecida.\n"); }
    | PRINT LPAREN expressao RPAREN SEMICOLON { printf("AST: Comando de impressao reconhecido.\n"); }
    | error SEMICOLON { yyerrok; /* Permite que o parser se recupere de erros após um ponto e vírgula */ }
    ;

expressao:
    NUM
    | ID
    | expressao PLUS expressao
    | expressao MINUS expressao
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico: %s\n", s);
}