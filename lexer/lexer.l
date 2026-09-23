%{
#include "parser.tab.h" /* Importa os tokens definidos pelo Bison */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
%}

/* Evita a necessidade de linkar com a biblioteca do Flex (-lfl) no GCC */
%option noyywrap 

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
"do"                    { return DO; }
"switch"                { return SWITCH; }
"case"                  { return CASE; }
"break"                 { return BREAK; }
"continue"              { return CONTINUE; }
"return"                { return RETURN; }
"try"                   { return TRY; }
"catch"                 { return CATCH; }
"throw"                 { return THROW; }
"class"                 { return CLASS; }
"extends"               { return EXTENDS; }
"import"                { return IMPORT; }
"from"                  { return FROM; }
"export"                { return EXPORT; }
"new"                   { return NEW; }
"in"                    { return IN; }
"of"                    { return OF; }
"delete"                { return DELETE; }
"console.log"           { return PRINT; }

 /* 2. Valores Literais */
"true"|"false"                  { return BOOLEAN_VAL; }
[0-9]+(\.[0-9]+)?               { return NUM; }
\"[^\"]*\"                      { return STRING_VAL; } 
\'[^\']*\'                      { return STRING_VAL; } 

 /* 3. Identificadores */
[a-zA-Z_][a-zA-Z0-9_]*          { return ID; }

 /* 4. Operadores Relacionais e Logicos */
"=="                            { return EQ; }
"!="                            { return NEQ; }
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
