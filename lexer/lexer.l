%{
#include "parser.tab.h" /* Importa os tokens definidos pelo Bison */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

/* Evita a necessidade de linkar com a biblioteca do Flex (-lfl) no GCC */
%option noyywrap
/* Habilita o rastreio de numero de linha (yylineno) para mensagens de erro */
%option yylineno

%%

 /* 1. Palavras Reservadas */
"let"                   { return LET; }
"const"                 { return CONST; }
"var"                   { return VAR; }
"function"              { return FUNCTION; }
"if"                    { return IF; }
"else"                  { return ELSE; }
"while"                 { return WHILE; }
"for"                   { return FOR; }
"return"                { return RETURN; }
"console.log"           { return PRINT; }

 /* 2. Valores Literais */
"true"                          { yylval.booleano = 1; return BOOLEAN_VAL; }
"false"                         { yylval.booleano = 0; return BOOLEAN_VAL; }
[0-9]+(\.[0-9]+)?               { yylval.valor_num = atof(yytext); return NUM; }
\"[^\"\r\n]*\"|\'[^\'\r\n]*\' { yylval.texto = strdup(yytext); return STRING_VAL; }

 /* Detecta a quebra sem consumi-la, preservando a linha inicial da string. */
(\"[^\"\r\n]*|\'[^\'\r\n]*)/[\r\n] {
    fprintf(stderr, "Erro lexico: string com quebra de linha na linha %d\n", yylineno);
    return YYUNDEF;
}

 /* Palavras reservadas fora do escopo: antes da regra de identificadores */
"do"|"switch"|"case"|"break"|"continue"|"try"|"catch"|"throw"|"class"|"extends"|"import"|"from"|"export"|"new"|"in"|"of"|"delete"|"default"|"finally"|"this"|"typeof"|"instanceof"|"void"|"super"|"with"|"yield"|"await"|"enum"|"debugger" {
    fprintf(stderr, "palavra reservada '%s' fora do escopo na linha %d\n", yytext, yylineno);
    /* Token invalido para o Bison registrar o erro, mesmo com recuperacao. */
    return YYUNDEF;
}

 /* 3. Identificadores */
[a-zA-Z_][a-zA-Z0-9_]*          { yylval.texto = strdup(yytext); return ID; }

 /* 4. Operadores Relacionais e Logicos */
"=="                            { return EQ; }
"!="                            { return NEQ; }
"!"                             { return NOT; }
"<="                            { return LEQ; }
">="                            { return GEQ; }
"<"                             { return LT; }
">"                             { return GT; }
"&&"                            { return AND; }
"||"                            { return OR; }

 /* 5. Operadores de Atribuicao e Aritmeticos */
"="                             { return ASSIGN; }
"+"                             { return PLUS; }
"-"                             { return MINUS; }
"*"                             { return TIMES; }
"/"                             { return DIVIDE; }

 /* 6. Delimitadores */
";"                             { return SEMICOLON; }
"("                             { return LPAREN; }
")"                             { return RPAREN; }
"{"                             { return LBRACE; }
"}"                             { return RBRACE; }
","                             { return COMMA; }

 /* 7. Ignorar espacos e comentarios */
[ \t\n\r]+                      { /* Ignora espacos em branco, tabs e quebras de linha */ }
"//".*                          { /* Ignora comentarios de uma linha */ }
"/*"([^*]|\*+[^*/])*\*+"/"      { /* Ignora comentarios de bloco / multilinha */ }

 /* 8. Tratamento de Erro Lexico */
.                       { printf("Erro lexico: Caractere invalido '%s'\n", yytext); }

%%
