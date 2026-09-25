#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Declarações externas do Flex e Bison */
extern int yyparse(void);
extern int yynerrs;

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

static int run_test(const char *descricao, const char *codigo, bool deve_passar) {
    yynerrs = 0;
    YY_BUFFER_STATE buffer = yy_scan_string(codigo);
    int parse_res = yyparse();
    yy_delete_buffer(buffer);

    bool sucesso = (parse_res == 0 && yynerrs == 0);
    bool ok = (sucesso == deve_passar);

    if (ok) {
        printf("  [PASSOU] %s\n", descricao);
        return 0;
    } else {
        printf("  [FALHOU] %s (esperava: %s, obtido: %s)\n",
               descricao,
               deve_passar ? "SUCESSO" : "ERRO",
               sucesso ? "SUCESSO" : "ERRO");
        return 1;
    }
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    int falhas = 0;
    int total = 0;

    printf("=========================================\n");
    printf(" Executando Testes Unitarios em Memoria \n");
    printf("        (via yy_scan_string)            \n");
    printf("=========================================\n\n");

    /* Casos válidos */
    printf("--- Casos Validos ---\n");
    total++; falhas += run_test("Declaracao simples de variavel", "let x = 10;", true);
    total++; falhas += run_test("Declaracao sem inicializacao", "let x;", true);
    total++; falhas += run_test("Declaracao com const", "const PI = 3.14;", true);
    total++; falhas += run_test("Declaracao com var", "var contador = 0;", true);
    total++; falhas += run_test("Declaracoes em cadeia com virgula", "let a = 1, b = 2, c;", true);
    total++; falhas += run_test("Comando de reatribuicao simples", "x = y + 5;", true);
    total++; falhas += run_test("Reatribuicao com operacao", "contador = contador + 1;", true);
    total++; falhas += run_test("Expressao com precedencia de operadores", "let a = (10 + 20) * 3;", true);
    total++; falhas += run_test("Expressao relacional e logica", "let ok = 5 > 2 && 10 <= 20;", true);
    total++; falhas += run_test("Comando console.log com literal", "console.log(\"teste\");", true);
    total++; falhas += run_test("Bloco de instrucoes delimitado", "{ let x = 1; console.log(x); }", true);
    total++; falhas += run_test("Instrucao vazia e comentarios", "// comentario\n; { ; }\n", true);

    /* Casos inválidos */
    printf("\n--- Casos Invalidos (Falha Sintatica Esperada) ---\n");
    total++; falhas += run_test("Falta de ponto e virgula ao final", "let x = 10", false);
    total++; falhas += run_test("Expressao incompleta antes do ponto e virgula", "let x = 10 + ;", false);
    total++; falhas += run_test("Parenteses desbalanceados", "let a = (10 + 20;", false);
    total++; falhas += run_test("Chave de bloco nao fechada", "{ let x = 1;", false);
    total++; falhas += run_test("Identificador invalido iniciando com digito", "let 123a = 456;", false);
    total++; falhas += run_test("Declaracao sem identificador", "let ;", false);
    total++; falhas += run_test("Declaracao em cadeia com virgula pendente", "let a = 1, ;", false);
    total++; falhas += run_test("Atribuicao sem expressao", "x = ;", false);

    /* Funcoes (T07) */
    printf("\n--- Funcoes: Casos Validos ---\n");
    total++; falhas += run_test("Funcao sem parametros", "function run() { console.log(1); }", true);
    total++; falhas += run_test("Funcao com multiplos parametros", "function soma(a, b) { return a + b; }", true);
    total++; falhas += run_test("Funcao com corpo vazio", "function nada() { }", true);
    total++; falhas += run_test("Retorno vazio", "function sair() { return; }", true);
    total++; falhas += run_test("Retorno dentro de bloco aninhado", "function f(x) { { return x * 2; } }", true);
    total++; falhas += run_test("Funcao aninhada", "function ext() { function int() { return 1; } return int(); }", true);
    total++; falhas += run_test("Chamada como comando", "soma(1, 2);", true);
    total++; falhas += run_test("Chamada sem argumentos", "run();", true);
    total++; falhas += run_test("Chamada encadeada em expressao", "let r = somar(x, 10) * 2;", true);
    total++; falhas += run_test("Chamadas aninhadas como argumento", "let r = f(g(1), h(2, 3) + 4);", true);
    total++; falhas += run_test("Chamada em condicao logica", "let ok = valido(x) && limite() > 0;", true);
    total++; falhas += run_test("console.log com chamada", "console.log(soma(1, 2));", true);
    total++; falhas += run_test("console.log com multiplos argumentos", "console.log(\"total:\", t, 1);", true);
    total++; falhas += run_test("console.log sem argumentos", "console.log();", true);
    total++; falhas += run_test("Funcoes intercaladas no escopo global", "let a = 1; function f() { return a; } a = f(); function g() { }", true);

    printf("\n--- Funcoes: Casos Invalidos (Falha Sintatica Esperada) ---\n");
    total++; falhas += run_test("Funcao sem nome", "function (a) { }", false);
    total++; falhas += run_test("Funcao sem corpo", "function f(a);", false);
    total++; falhas += run_test("Funcao com corpo sem chaves", "function f() return 1;", false);
    total++; falhas += run_test("Parametro com virgula pendente", "function f(a, ) { }", false);
    total++; falhas += run_test("Parametro que nao e identificador", "function f(1) { }", false);
    total++; falhas += run_test("Argumento com virgula pendente", "f(1, );", false);
    total++; falhas += run_test("Chamada sem ponto e virgula", "f(1)", false);
    total++; falhas += run_test("Return sem ponto e virgula", "function f() { return 1 }", false);
    total++; falhas += run_test("Return no escopo global", "return 1;", false);
    total++; falhas += run_test("Return em bloco fora de funcao", "{ return; }", false);
    total++; falhas += run_test("Return apos o fim da funcao", "function f() { } return;", false);

    int passaram = total - falhas;
    printf("\n-----------------------------------------\n");
    printf("Sumario dos Testes em Memoria:\n");
    printf("  Total: %d | Passaram: %d | Falharam: %d\n", total, passaram, falhas);
    printf("-----------------------------------------\n");

    return falhas > 0 ? 1 : 0;
}
