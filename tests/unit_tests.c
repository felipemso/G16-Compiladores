#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"

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

static int verifica(const char *descricao, bool ok) {
    if (ok) {
        printf("  [PASSOU] %s\n", descricao);
        return 0;
    }
    printf("  [FALHOU] %s\n", descricao);
    return 1;
}

static int testa_literais(void) {
    char *texto = strdup("\"ola\"");
    char *nome = strdup("x");
    No *num = no_numero(3.5, 2);
    No *str = no_string(texto, 3);
    No *boo = no_booleano(5, 4);
    No *id = no_id(nome, 5);
    bool ok = num->categoria == NO_NUMERO && num->valor.numero == 3.5 && num->linha == 2
        && num->tipo_inferido == TIPO_DESCONHECIDO && num->operador == OP_NENHUM
        && num->filhos[0] == NULL && num->filhos[3] == NULL && num->proximo == NULL
        && str->categoria == NO_STRING && str->valor.texto == texto && str->linha == 3
        && boo->categoria == NO_BOOLEANO && boo->valor.booleano == 1
        && id->categoria == NO_ID && id->valor.texto == nome && id->linha == 5;
    int resultado = verifica("Literais e identificador guardam valor, linha e tipo desconhecido", ok);
    ast_libera(num);
    ast_libera(str);
    ast_libera(boo);
    ast_libera(id);
    return resultado;
}

static int testa_precedencia(void) {
    No *mult = no_binario(OP_MULTIPLICACAO, no_numero(2, 1), no_numero(3, 1), 1);
    No *soma = no_binario(OP_SOMA, no_numero(1, 1), mult, 1);
    bool ok = soma->categoria == NO_BINARIO && soma->operador == OP_SOMA
        && soma->filhos[0]->valor.numero == 1 && soma->filhos[1] == mult
        && mult->operador == OP_MULTIPLICACAO
        && mult->filhos[0]->valor.numero == 2 && mult->filhos[1]->valor.numero == 3;
    int resultado = verifica("1 + 2 * 3 monta +(1, *(2, 3))", ok);
    ast_libera(soma);
    return resultado;
}

static int testa_unario(void) {
    No *nao = no_unario(OP_NAO, no_booleano(1, 7), 7);
    No *neg = no_unario(OP_NEGATIVO, no_numero(4, 8), 8);
    bool ok = nao->categoria == NO_UNARIO && nao->operador == OP_NAO
        && nao->filhos[0]->categoria == NO_BOOLEANO && nao->filhos[1] == NULL
        && neg->operador == OP_NEGATIVO && neg->filhos[0]->valor.numero == 4;
    int resultado = verifica("Operadores unarios ! e - guardam o operando", ok);
    ast_libera(nao);
    ast_libera(neg);
    return resultado;
}

static int testa_lista(void) {
    No *a = no_numero(1, 1);
    No *b = no_numero(2, 1);
    No *c = no_numero(3, 1);
    No *lista = no_lista_adiciona(NULL, a);
    lista = no_lista_adiciona(lista, NULL);
    lista = no_lista_adiciona(lista, b);
    lista = no_lista_adiciona(lista, c);
    bool ok = lista == a && a->proximo == b && b->proximo == c && c->proximo == NULL
        && no_lista_adiciona(NULL, NULL) == NULL;
    int resultado = verifica("Lista mantem a ordem e ignora NULL", ok);
    ast_libera(lista);
    return resultado;
}

static int testa_comandos(void) {
    No *decl = no_decl_var(DECLARADOR_CONST, strdup("PI"), no_numero(3.14, 1), 1);
    No *se = no_if(no_id(strdup("x"), 2), no_bloco(NULL, 2), NULL, 2);
    No *laco = no_for(NULL, NULL, NULL, no_vazio(3), 3);
    No *params = no_lista_adiciona(no_parametro(strdup("a"), 4), no_parametro(strdup("b"), 4));
    No *corpo = no_bloco(no_return(no_binario(OP_SOMA, no_id(strdup("a"), 5), no_id(strdup("b"), 5), 5), 5), 4);
    No *func = no_funcao(strdup("soma"), params, corpo, 4);
    No *args = no_lista_adiciona(no_numero(1, 6), no_numero(2, 6));
    No *print = no_print(no_chamada(strdup("soma"), args, 6), 6);
    bool ok = decl->categoria == NO_DECL_VAR && decl->declarador == DECLARADOR_CONST
        && strcmp(decl->valor.texto, "PI") == 0 && decl->filhos[0]->valor.numero == 3.14
        && se->categoria == NO_IF && se->filhos[1]->categoria == NO_BLOCO && se->filhos[2] == NULL
        && laco->categoria == NO_FOR && laco->filhos[0] == NULL && laco->filhos[3]->categoria == NO_VAZIO
        && func->categoria == NO_FUNCAO && strcmp(func->valor.texto, "soma") == 0 && func->linha == 4
        && func->filhos[0] == params && strcmp(params->proximo->valor.texto, "b") == 0
        && func->filhos[1]->filhos[0]->categoria == NO_RETURN
        && print->filhos[0]->categoria == NO_CHAMADA && print->filhos[0]->filhos[0]->proximo->valor.numero == 2;
    No *programa = no_programa(NULL, 1);
    programa->filhos[0] = no_lista_adiciona(no_lista_adiciona(no_lista_adiciona(decl, se), laco), no_lista_adiciona(func, print));
    ok = ok && programa->categoria == NO_PROGRAMA && programa->filhos[0] == decl
        && decl->proximo == se && se->proximo == laco && laco->proximo == func && func->proximo == print;
    int resultado = verifica("Comandos montam filhos nas posicoes do contrato e a arvore e liberada", ok);
    ast_libera(programa);
    ast_libera(NULL);
    return resultado;
}

static int testa_tabela_insere_busca(void) {
    char nome[] = "x";
    Simbolo *s = tabela_insere(nome, SIMBOLO_LET, TIPO_NUMERO, 3);
    nome[0] = 'y';
    bool ok = s && strcmp(s->nome, "x") == 0 && s->categoria == SIMBOLO_LET
        && s->tipo_inferido == TIPO_NUMERO && s->linha == 3 && s->num_parametros == 0
        && s->nivel_escopo == 0 && tabela_busca_local("x") == s && tabela_busca("x") == s
        && tabela_busca("y") == NULL && tabela_busca_local("y") == NULL;
    int resultado = verifica("Insere copiando o nome e encontra pela busca local e global", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_funcao(void) {
    Simbolo *f = tabela_insere("soma", SIMBOLO_FUNCAO, TIPO_DESCONHECIDO, 1);
    f->num_parametros = 2;
    Simbolo *achado = tabela_busca("soma");
    bool ok = achado == f && achado->categoria == SIMBOLO_FUNCAO && achado->num_parametros == 2;
    int resultado = verifica("Funcao guarda o numero de parametros", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_redeclaracao(void) {
    Simbolo *primeiro = tabela_insere("x", SIMBOLO_VAR, TIPO_NUMERO, 1);
    Simbolo *segundo = tabela_insere("x", SIMBOLO_CONST, TIPO_STRING, 2);
    bool ok = primeiro && segundo == NULL && tabela_busca("x") == primeiro
        && primeiro->categoria == SIMBOLO_VAR && primeiro->linha == 1;
    int resultado = verifica("Nome repetido no mesmo escopo e recusado e mantem o original", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_niveis(void) {
    bool ok = escopo_nivel() == 0;
    escopo_entra();
    ok = ok && escopo_nivel() == 1;
    escopo_entra();
    Simbolo *s = tabela_insere("x", SIMBOLO_LET, TIPO_DESCONHECIDO, 1);
    ok = ok && escopo_nivel() == 2 && s->nivel_escopo == 2;
    escopo_sai();
    ok = ok && escopo_nivel() == 1;
    escopo_sai();
    ok = ok && escopo_nivel() == 0;
    escopo_sai();
    ok = ok && escopo_nivel() == 0;
    int resultado = verifica("Pilha de escopos controla o nivel e nao sai do global", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_busca_por_escopo(void) {
    Simbolo *global = tabela_insere("g", SIMBOLO_LET, TIPO_NUMERO, 1);
    escopo_entra();
    Simbolo *local = tabela_insere("l", SIMBOLO_PARAMETRO, TIPO_DESCONHECIDO, 2);
    escopo_entra();
    bool ok = tabela_busca_local("g") == NULL && tabela_busca_local("l") == NULL
        && tabela_busca("g") == global && tabela_busca("l") == local;
    int resultado = verifica("Busca local ve so o escopo atual e a busca sobe ate o global", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_sair_escopo(void) {
    escopo_entra();
    tabela_insere("x", SIMBOLO_LET, TIPO_NUMERO, 1);
    escopo_sai();
    bool ok = tabela_busca("x") == NULL && tabela_busca_local("x") == NULL;
    escopo_entra();
    ok = ok && tabela_busca("x") == NULL && tabela_insere("x", SIMBOLO_LET, TIPO_NUMERO, 2) != NULL;
    int resultado = verifica("Simbolo some ao sair do escopo e nao volta em escopo novo", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_mesmo_nome(void) {
    Simbolo *externo = tabela_insere("x", SIMBOLO_LET, TIPO_NUMERO, 1);
    escopo_entra();
    Simbolo *interno = tabela_insere("x", SIMBOLO_PARAMETRO, TIPO_DESCONHECIDO, 2);
    bool ok = interno && interno != externo && tabela_busca("x") == interno;
    escopo_sai();
    ok = ok && tabela_busca("x") == externo;
    int resultado = verifica("Mesmo nome em escopo interno tem prioridade ate o escopo fechar", ok);
    tabela_libera();
    return resultado;
}

static int testa_tabela_libera(void) {
    tabela_insere("g", SIMBOLO_LET, TIPO_NUMERO, 1);
    escopo_entra();
    escopo_entra();
    tabela_insere("x", SIMBOLO_LET, TIPO_NUMERO, 2);
    tabela_libera();
    bool ok = escopo_nivel() == 0 && tabela_busca("g") == NULL && tabela_busca("x") == NULL
        && tabela_insere("g", SIMBOLO_CONST, TIPO_STRING, 3) != NULL;
    int resultado = verifica("Liberar fecha os escopos abertos e deixa a tabela reutilizavel", ok);
    tabela_libera();
    tabela_libera();
    return resultado;
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
    total++; falhas += run_test("Reatribuicao da propria variavel", "x = x + 1;", true);
    total++; falhas += run_test("Declaracao em cadeia mista", "let a = 1, b, c = 3;", true);
    total++; falhas += run_test("Declaracao em cadeia com var", "var i = 0, j;", true);
    total++; falhas += run_test("Declaracao em cadeia com const", "const A = 1, B = A * 2;", true);
    total++; falhas += run_test("Declaracao com string e booleano", "let s = 'ok', f = false;", true);
    total++; falhas += run_test("Expressao com precedencia de operadores", "let a = (10 + 20) * 3;", true);
    total++; falhas += run_test("Expressao relacional e logica", "let ok = 5 > 2 && 10 <= 20;", true);
    total++; falhas += run_test("Comando console.log com literal", "console.log(\"teste\");", true);
    total++; falhas += run_test("Bloco de instrucoes delimitado", "{ let x = 1; console.log(x); }", true);
    total++; falhas += run_test("Instrucao vazia e comentarios", "// comentario\n; { ; }\n", true);
    total++; falhas += run_test("If-else aninhado associa else ao if interno", "if (a) if (b) x = 1; else x = 2;", true);
    total++; falhas += run_test("While com bloco", "while (contador < 10) { contador = contador + 1; }", true);
    total++; falhas += run_test("For com declaracao e incremento", "for (let i = 0; i < 10; i = i + 1) { total = total + i; }", true);
    total++; falhas += run_test("Programa vazio", "", true);
    total++; falhas += run_test("Programa apenas com comentarios", "// linha\n/* bloco */\n", true);
    total++; falhas += run_test("Blocos aninhados", "{ { let a = 1; } }", true);
    total++; falhas += run_test("Blocos vazios aninhados", "{ { { } } }", true);
    total++; falhas += run_test("Blocos intercalados no escopo global", "let a = 1; { a = 2; } console.log(a); { }", true);

    /* Casos inválidos */
    printf("\n--- Casos Invalidos (Falha Sintatica Esperada) ---\n");
    total++; falhas += run_test("Falta de ponto e virgula ao final", "let x = 10", false);
    total++; falhas += run_test("Expressao incompleta antes do ponto e virgula", "let x = 10 + ;", false);
    total++; falhas += run_test("Parenteses desbalanceados", "let a = (10 + 20;", false);
    total++; falhas += run_test("Chave de bloco nao fechada", "{ let x = 1;", false);
    total++; falhas += run_test("Chave de fechamento extra", "{ let x = 1; } }", false);
    total++; falhas += run_test("Bloco aninhado nao fechado", "{ { let x = 1; }", false);
    total++; falhas += run_test("Identificador invalido iniciando com digito", "let 123a = 456;", false);
    total++; falhas += run_test("Declaracao sem identificador", "let ;", false);
    total++; falhas += run_test("Declaracao em cadeia com virgula pendente", "let a = 1, ;", false);
    total++; falhas += run_test("Atribuicao sem expressao", "x = ;", false);
    total++; falhas += run_test("Const sem inicializacao", "const PI;", false);
    total++; falhas += run_test("Const em cadeia com item sem inicializacao", "const A = 1, B;", false);
    total++; falhas += run_test("Declarador sem palavra-chave em cadeia", "a = 1, b = 2;", false);
    total++; falhas += run_test("Atribuicao a literal", "10 = x;", false);
    total++; falhas += run_test("Palavra-chave duplicada", "let let x = 1;", false);

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

    /* Recuperacao de erros sintaticos (T08) */
    printf("\n--- Recuperacao de Erros Sintaticos ---\n");
    total++; falhas += run_test(
        "Recuperacao em nivel de comando (sincroniza no ';')",
        "let a = 1; let b = ; let c = 2; console.log(c);",
        false);
    total++; falhas += run_test(
        "Recuperacao em nivel de bloco (sincroniza no '}')",
        "let x = 1; { let y = 5 } console.log(x);",
        false);
    total++; falhas += run_test(
        "Multiplos erros consecutivos nao travam o parser",
        "let a = ; let b = ; let c = ; let d = 4; console.log(d);",
        false);

    printf("\n--- AST: Construtores e Liberacao ---\n");
    total++; falhas += testa_literais();
    total++; falhas += testa_precedencia();
    total++; falhas += testa_unario();
    total++; falhas += testa_lista();
    total++; falhas += testa_comandos();

    printf("\n--- Tabela de Simbolos ---\n");
    total++; falhas += testa_tabela_insere_busca();
    total++; falhas += testa_tabela_funcao();
    total++; falhas += testa_tabela_redeclaracao();
    total++; falhas += testa_tabela_niveis();
    total++; falhas += testa_tabela_busca_por_escopo();
    total++; falhas += testa_tabela_sair_escopo();
    total++; falhas += testa_tabela_mesmo_nome();
    total++; falhas += testa_tabela_libera();

    int passaram = total - falhas;
    printf("\n-----------------------------------------\n");
    printf("Sumario dos Testes em Memoria:\n");
    printf("  Total: %d | Passaram: %d | Falharam: %d\n", total, passaram, falhas);
    printf("-----------------------------------------\n");

    return falhas > 0 ? 1 : 0;
}
