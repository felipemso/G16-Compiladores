#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declarações externas do Flex e Bison */
extern int yyparse(void);
extern int yynerrs;

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

static int run_test(const char *descricao, const char *codigo, int deve_passar) {
    yynerrs = 0;
    YY_BUFFER_STATE buffer = yy_scan_string(codigo);
    int parse_res = yyparse();
    yy_delete_buffer(buffer);

    int sucesso = (parse_res == 0 && yynerrs == 0);
    int ok = (sucesso == deve_passar);

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
    /* Desativa bufferizacao para sincronizar stdout e stderr */
    setvbuf(stdout, NULL, _IONBF, 0);

    int falhas = 0;
    int total = 0;

    printf("=========================================\n");
    printf(" Executando Testes Unitarios em Memoria \n");
    printf("        (via yy_scan_string)            \n");
    printf("=========================================\n\n");

    /* Casos válidos */
    printf("--- Casos Validos ---\n");
    total++; falhas += run_test("Declaracao simples de variavel", "let x = 10;", 1);
    total++; falhas += run_test("Declaracao sem inicializacao", "let x;", 1);
    total++; falhas += run_test("Declaracao com const", "const PI = 3.14;", 1);
    total++; falhas += run_test("Declaracao com var", "var contador = 0;", 1);
    total++; falhas += run_test("Declaracoes em cadeia com virgula", "let a = 1, b = 2, c;", 1);
    total++; falhas += run_test("Comando de reatribuicao simples", "x = y + 5;", 1);
    total++; falhas += run_test("Reatribuicao com operacao", "contador = contador + 1;", 1);
    total++; falhas += run_test("Expressao com precedencia de operadores", "let a = (10 + 20) * 3;", 1);
    total++; falhas += run_test("Expressao relacional e logica", "let ok = 5 > 2 && 10 <= 20;", 1);
    total++; falhas += run_test("Comando console.log com literal", "console.log(\"teste\");", 1);
    total++; falhas += run_test("Bloco de instrucoes delimitado", "{ let x = 1; console.log(x); }", 1);
    total++; falhas += run_test("Instrucao vazia e comentarios", "// comentario\n; { ; }\n", 1);

    /* Casos inválidos */
    printf("\n--- Casos Invalidos (Falha Sintatica Esperada) ---\n");
    total++; falhas += run_test("Falta de ponto e virgula ao final", "let x = 10", 0);
    total++; falhas += run_test("Expressao incompleta antes do ponto e virgula", "let x = 10 + ;", 0);
    total++; falhas += run_test("Parenteses desbalanceados", "let a = (10 + 20;", 0);
    total++; falhas += run_test("Chave de bloco nao fechada", "{ let x = 1;", 0);
    total++; falhas += run_test("Identificador invalido iniciando com digito", "let 123a = 456;", 0);
    total++; falhas += run_test("Declaracao sem identificador", "let ;", 0);
    total++; falhas += run_test("Declaracao em cadeia com virgula pendente", "let a = 1, ;", 0);
    total++; falhas += run_test("Atribuicao sem expressao", "x = ;", 0);

    int passaram = total - falhas;
    printf("\n-----------------------------------------\n");
    printf("Sumario dos Testes em Memoria:\n");
    printf("  Total: %d | Passaram: %d | Falharam: %d\n", total, passaram, falhas);
    printf("-----------------------------------------\n");

    return falhas > 0 ? 1 : 0;
}
