#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ast.h"
#include "tabela.h"
#include "semantico.h"

/* Declarações externas do Flex e Bison */
extern int yyparse(void);
extern int yynerrs;
extern int yylineno;
extern No *raiz_ast;

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char *str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);

static int parse_string(const char *codigo) {
    yynerrs = 0;
    yylineno = 1;
    YY_BUFFER_STATE buffer = yy_scan_string(codigo);
    int resultado = yyparse();
    yy_delete_buffer(buffer);
    return resultado;
}

static int run_test(const char *descricao, const char *codigo, bool deve_passar) {
    int parse_res = parse_string(codigo);

    ast_libera(raiz_ast);
    raiz_ast = NULL;

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

static int testa_ast_parser(const char *descricao, const char *codigo, const TipoNo *esperado, size_t qtd, bool erro) {
    int resultado = parse_string(codigo);
    bool ok = resultado == 0 && (yynerrs > 0) == erro
        && raiz_ast && raiz_ast->categoria == NO_PROGRAMA;
    No *no = raiz_ast ? raiz_ast->filhos[0] : NULL;
    for (size_t i = 0; i < qtd; i++) {
        ok = ok && no && no->categoria == esperado[i];
        if (no) no = no->proximo;
    }
    ok = ok && no == NULL;
    int falhas = verifica(descricao, ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
}

static int testa_ast_linhas(void) {
    const char *codigo =
        "let a =\n"
        "    10;\n"
        "{\n"
        "    x =\n"
        "        20;\n"
        "    ;\n"
        "}\n";
    int resultado = parse_string(codigo);
    No *a = raiz_ast ? raiz_ast->filhos[0] : NULL;
    No *bloco = a ? a->proximo : NULL;
    No *x = bloco ? bloco->filhos[0] : NULL;
    No *vazio = x ? x->proximo : NULL;
    bool ok = resultado == 0 && yynerrs == 0
        && a && a->linha == 1
        && bloco && bloco->linha == 3
        && x && x->linha == 4
        && vazio && vazio->linha == 6;
    int falhas = verifica("Nos guardam a linha inicial mesmo com quebras de linha", ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
}

static int testa_ast_declaracoes(void) {
    int resultado = parse_string("let a, b = 1 + 2 * 3; var c; const d = true, e = 'ok'; { a = -b; ; {} }");
    No *a = raiz_ast ? raiz_ast->filhos[0] : NULL;
    No *b = a ? a->proximo : NULL;
    No *c = b ? b->proximo : NULL;
    No *d = c ? c->proximo : NULL;
    No *e = d ? d->proximo : NULL;
    No *bloco = e ? e->proximo : NULL;
    No *atr = bloco ? bloco->filhos[0] : NULL;
    No *vazio = atr ? atr->proximo : NULL;
    No *interno = vazio ? vazio->proximo : NULL;
    bool ok = resultado == 0 && yynerrs == 0 && a && b && c && d && e
        && a->categoria == NO_DECL_VAR && !strcmp(a->valor.texto, "a") && !a->filhos[0]
        && a->declarador == DECLARADOR_LET && b->declarador == DECLARADOR_LET
        && b->filhos[0] && b->filhos[0]->operador == OP_SOMA
        && b->filhos[0]->filhos[1]->operador == OP_MULTIPLICACAO
        && c->declarador == DECLARADOR_VAR && !c->filhos[0]
        && d->declarador == DECLARADOR_CONST && e->declarador == DECLARADOR_CONST
        && d->filhos[0] && d->filhos[0]->valor.booleano == 1
        && e->filhos[0] && !strcmp(e->filhos[0]->valor.texto, "'ok'")
        && bloco && bloco->categoria == NO_BLOCO && !bloco->proximo
        && atr && atr->categoria == NO_ATRIBUICAO && !strcmp(atr->valor.texto, "a")
        && atr->filhos[0] && atr->filhos[0]->operador == OP_NEGATIVO
        && vazio && vazio->categoria == NO_VAZIO
        && interno && interno->categoria == NO_BLOCO && !interno->filhos[0] && !interno->proximo;
    int falhas = verifica("Parser preserva declaradores, inicializadores, precedencia e blocos aninhados", ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
}

static int testa_ast_funcao(void) {
    int resultado = parse_string("function soma(a,\n  b) {\n  let c = a + b;\n}\nfunction vazia() {}");
    No *soma = raiz_ast ? raiz_ast->filhos[0] : NULL;
    No *vazia = soma ? soma->proximo : NULL;
    No *a = soma ? soma->filhos[0] : NULL;
    No *b = a ? a->proximo : NULL;
    No *corpo = soma ? soma->filhos[1] : NULL;
    bool ok = resultado == 0 && yynerrs == 0
        && soma && soma->categoria == NO_FUNCAO && !strcmp(soma->valor.texto, "soma") && soma->linha == 1
        && a && a->categoria == NO_PARAMETRO && !strcmp(a->valor.texto, "a") && a->linha == 1
        && b && b->categoria == NO_PARAMETRO && !strcmp(b->valor.texto, "b") && b->linha == 2 && !b->proximo
        && corpo && corpo->categoria == NO_BLOCO
        && corpo->filhos[0] && corpo->filhos[0]->categoria == NO_DECL_VAR
        && vazia && vazia->categoria == NO_FUNCAO && !strcmp(vazia->valor.texto, "vazia") && vazia->linha == 5
        && !vazia->filhos[0] && vazia->filhos[1] && vazia->filhos[1]->categoria == NO_BLOCO
        && !vazia->filhos[1]->filhos[0] && !vazia->proximo;
    int falhas = verifica("Funcao guarda nome, parametros em ordem e corpo", ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
}

static int testa_ast_return(void) {
    int resultado = parse_string("function f() {\n  return;\n}\nfunction g(a) {\n  return a + 1;\n}\nreturn 2;\nlet x;");
    No *f = raiz_ast ? raiz_ast->filhos[0] : NULL;
    No *g = f ? f->proximo : NULL;
    No *x = g ? g->proximo : NULL;
    No *vazio = f && f->filhos[1] ? f->filhos[1]->filhos[0] : NULL;
    No *soma = g && g->filhos[1] ? g->filhos[1]->filhos[0] : NULL;
    bool ok = resultado == 0 && yynerrs == 1
        && vazio && vazio->categoria == NO_RETURN && vazio->linha == 2 && !vazio->filhos[0]
        && soma && soma->categoria == NO_RETURN && soma->linha == 5
        && soma->filhos[0] && soma->filhos[0]->operador == OP_SOMA
        && x && x->categoria == NO_DECL_VAR && !x->proximo;
    int falhas = verifica("Return guarda a expressao e return fora de funcao nao entra na arvore", ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
}

static int testa_ast_chamada(void) {
    int resultado = parse_string("function f(a, b) {\n  return f(a, 1 + 2);\n}\nf();\nlet x = f(1) * 2;");
    No *f = raiz_ast ? raiz_ast->filhos[0] : NULL;
    No *comando = f ? f->proximo : NULL;
    No *x = comando ? comando->proximo : NULL;
    No *ret = f && f->filhos[1] ? f->filhos[1]->filhos[0] : NULL;
    No *recursiva = ret ? ret->filhos[0] : NULL;
    No *arg1 = recursiva ? recursiva->filhos[0] : NULL;
    No *arg2 = arg1 ? arg1->proximo : NULL;
    No *mult = x ? x->filhos[0] : NULL;
    No *em_expressao = mult ? mult->filhos[0] : NULL;
    bool ok = resultado == 0 && yynerrs == 0
        && recursiva && recursiva->categoria == NO_CHAMADA && !strcmp(recursiva->valor.texto, "f")
        && recursiva->linha == 2
        && arg1 && arg1->categoria == NO_ID && !strcmp(arg1->valor.texto, "a")
        && arg2 && arg2->operador == OP_SOMA && !arg2->proximo
        && comando && comando->categoria == NO_CHAMADA && !comando->filhos[0] && comando->linha == 4
        && mult && mult->operador == OP_MULTIPLICACAO
        && em_expressao && em_expressao->categoria == NO_CHAMADA
        && em_expressao->filhos[0] && em_expressao->filhos[0]->valor.numero == 1
        && !em_expressao->filhos[0]->proximo && !x->proximo;
    int falhas = verifica("Chamada guarda nome e argumentos como comando e dentro de expressao", ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
}

static int testa_ast_print(void) {
    int resultado = parse_string("console.log();\nconsole.log(1, \"a\",\n  f(x) + 2);");
    No *vazio = raiz_ast ? raiz_ast->filhos[0] : NULL;
    No *print = vazio ? vazio->proximo : NULL;
    No *num = print ? print->filhos[0] : NULL;
    No *str = num ? num->proximo : NULL;
    No *soma = str ? str->proximo : NULL;
    bool ok = resultado == 0 && yynerrs == 0
        && vazio && vazio->categoria == NO_PRINT && !vazio->filhos[0] && vazio->linha == 1
        && print && print->categoria == NO_PRINT && print->linha == 2 && !print->proximo
        && num && num->categoria == NO_NUMERO && num->valor.numero == 1
        && str && str->categoria == NO_STRING && !strcmp(str->valor.texto, "\"a\"")
        && soma && soma->operador == OP_SOMA && soma->linha == 3 && !soma->proximo
        && soma->filhos[0] && soma->filhos[0]->categoria == NO_CHAMADA;
    int falhas = verifica("Console.log gera no de impressao com os argumentos em ordem", ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    return falhas;
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

static No *decl(Declarador declarador, const char *nome, int linha) {
    return no_decl_var(declarador, strdup(nome), NULL, linha);
}

static const Simbolo *simbolo_em(const char *nome, int nivel) {
    for (const Simbolo *s = tabela_primeiro(); s; s = s->proximo_na_tabela) {
        if (s->nivel_escopo == nivel && strcmp(s->nome, nome) == 0) {
            return s;
        }
    }
    return NULL;
}

static int termina_semantico_com_erros(const char *descricao, bool ok, No *raiz, int erros_esperados) {
    int resultado = verifica(descricao, ok && escopo_nivel() == 0 && erros_semanticos == erros_esperados);
    ast_libera(raiz);
    tabela_libera();
    erros_semanticos = 0;
    return resultado;
}

static int termina_semantico(const char *descricao, bool ok, No *raiz) {
    return termina_semantico_com_erros(descricao, ok, raiz, 0);
}

static int testa_semantico_variaveis(void) {
    No *comandos = no_lista_adiciona(decl(DECLARADOR_LET, "a", 1), decl(DECLARADOR_VAR, "b", 2));
    comandos = no_lista_adiciona(comandos, no_decl_var(DECLARADOR_CONST, strdup("c"), no_numero(1, 3), 3));
    No *raiz = no_programa(comandos, 1);
    analisa(raiz);
    const Simbolo *a = simbolo_em("a", 0);
    const Simbolo *b = simbolo_em("b", 0);
    const Simbolo *c = simbolo_em("c", 0);
    bool ok = a && a->categoria == SIMBOLO_LET && a->linha == 1
        && b && b->categoria == SIMBOLO_VAR && b->linha == 2
        && c && c->categoria == SIMBOLO_CONST && c->linha == 3;
    return termina_semantico("Registra let, var e const no escopo global", ok, raiz);
}

static int testa_semantico_blocos(void) {
    No *interno = no_bloco(decl(DECLARADOR_LET, "y", 2), 2);
    No *externo = no_bloco(no_lista_adiciona(decl(DECLARADOR_LET, "x", 1), interno), 1);
    No *raiz = no_programa(externo, 1);
    analisa(raiz);
    bool ok = simbolo_em("x", 1) && simbolo_em("y", 2) && tabela_busca("x") == NULL && tabela_busca("y") == NULL;
    return termina_semantico("Blocos aninhados abrem e fecham um escopo cada", ok, raiz);
}

static int testa_semantico_var_de_bloco(void) {
    No *raiz = no_programa(no_bloco(decl(DECLARADOR_VAR, "v", 1), 1), 1);
    analisa(raiz);
    bool ok = simbolo_em("v", 1) && simbolo_em("v", 0) == NULL && tabela_busca("v") == NULL;
    return termina_semantico("var tem escopo de bloco, como let", ok, raiz);
}

static int testa_semantico_controle(void) {
    No *cond_if = no_if(no_booleano(1, 1), no_bloco(decl(DECLARADOR_LET, "x", 1), 1),
                        no_bloco(decl(DECLARADOR_LET, "y", 2), 2), 1);
    No *cond_while = no_while(no_booleano(1, 3), no_bloco(decl(DECLARADOR_LET, "z", 3), 3), 3);
    No *raiz = no_programa(no_lista_adiciona(cond_if, cond_while), 1);
    analisa(raiz);
    bool ok = simbolo_em("x", 1) && simbolo_em("y", 1) && simbolo_em("z", 1);
    return termina_semantico("Blocos de if, else e while tem escopo proprio", ok, raiz);
}

static No *for_com_i(int linha) {
    return no_for(decl(DECLARADOR_LET, "i", linha), NULL, NULL, no_bloco(decl(DECLARADOR_LET, "j", linha), linha), linha);
}

static int testa_semantico_for(void) {
    No *raiz = no_programa(for_com_i(1), 1);
    analisa(raiz);
    bool ok = simbolo_em("i", 1) && simbolo_em("j", 2) && tabela_busca("i") == NULL;
    return termina_semantico("for abre escopo proprio para o let da inicializacao", ok, raiz);
}

static int testa_semantico_dois_for(void) {
    No *raiz = no_programa(no_lista_adiciona(for_com_i(1), for_com_i(2)), 1);
    analisa(raiz);
    const Simbolo *primeiro = simbolo_em("i", 1);
    bool ok = primeiro && primeiro->linha == 1 && primeiro->proximo_na_tabela
        && strcmp(primeiro->proximo_na_tabela->proximo_na_tabela->nome, "i") == 0
        && primeiro->proximo_na_tabela->proximo_na_tabela->linha == 2;
    return termina_semantico("Dois for seguidos registram cada i no seu escopo", ok, raiz);
}

static int testa_semantico_funcao(void) {
    No *parametros = no_lista_adiciona(no_parametro(strdup("a"), 1), no_parametro(strdup("b"), 1));
    No *corpo = no_bloco(decl(DECLARADOR_LET, "c", 2), 1);
    No *raiz = no_programa(no_funcao(strdup("f"), parametros, corpo, 1), 1);
    analisa(raiz);
    const Simbolo *f = simbolo_em("f", 0);
    const Simbolo *a = simbolo_em("a", 1);
    const Simbolo *c = simbolo_em("c", 1);
    bool ok = f && f->categoria == SIMBOLO_FUNCAO && f->num_parametros == 2
        && a && a->categoria == SIMBOLO_PARAMETRO && simbolo_em("b", 1)
        && c && c->categoria == SIMBOLO_LET && tabela_busca("a") == NULL && tabela_busca("f") == f;
    return termina_semantico("Funcao, parametros e corpo ficam nos escopos certos", ok, raiz);
}

static int testa_semantico_funcao_antes_do_corpo(void) {
    No *raiz = no_programa(no_funcao(strdup("f"), no_parametro(strdup("a"), 1), no_bloco(NULL, 1), 1), 1);
    analisa(raiz);
    const Simbolo *primeiro = tabela_primeiro();
    bool ok = primeiro && strcmp(primeiro->nome, "f") == 0 && primeiro->proximo_na_tabela
        && strcmp(primeiro->proximo_na_tabela->nome, "a") == 0;
    return termina_semantico("Funcao entra na tabela antes do corpo", ok, raiz);
}

static int testa_semantico_nome_repetido(void) {
    No *comandos = no_lista_adiciona(decl(DECLARADOR_LET, "x", 1), decl(DECLARADOR_LET, "x", 2));
    comandos = no_lista_adiciona(comandos, no_funcao(strdup("f"), NULL, no_bloco(NULL, 3), 3));
    comandos = no_lista_adiciona(comandos, no_funcao(strdup("f"), no_parametro(strdup("a"), 4), no_bloco(NULL, 4), 4));
    No *raiz = no_programa(comandos, 1);
    analisa(raiz);
    const Simbolo *x = simbolo_em("x", 0);
    const Simbolo *f = simbolo_em("f", 0);
    bool ok = x && x->linha == 1 && f && f->linha == 3 && f->num_parametros == 0;
    /* T19: as duas repeticoes (x e f) agora sao erros de redeclaracao */
    return termina_semantico_com_erros("Nome repetido gera erro e mantem o primeiro", ok, raiz, 2);
}

static int testa_semantico_com_parser(void) {
    int resultado_parse = parse_string("let a = 1, b;\nconst c = 2;\n{\n  var d;\n  { let e; }\n}\n");
    analisa(raiz_ast);
    const Simbolo *a = simbolo_em("a", 0);
    const Simbolo *c = simbolo_em("c", 0);
    const Simbolo *d = simbolo_em("d", 1);
    const Simbolo *e = simbolo_em("e", 2);
    bool ok = resultado_parse == 0 && yynerrs == 0 && a && a->linha == 1 && simbolo_em("b", 0)
        && c && c->categoria == SIMBOLO_CONST && c->linha == 2
        && d && d->categoria == SIMBOLO_VAR && d->linha == 4 && e && e->linha == 5;
    No *raiz = raiz_ast;
    raiz_ast = NULL;
    return termina_semantico("Popula a tabela a partir da AST gerada pelo parser", ok, raiz);
}

static int testa_semantico_erro(void) {
    analisa(NULL);
    bool zerado = erros_semanticos == 0;
    erro_semantico(7, "nome '%s' invalido", "x");
    erro_semantico(8, "outro erro");
    bool contou = erros_semanticos == 2;
    analisa(NULL);
    return verifica("erro_semantico conta os erros e analisa zera o contador",
                    zerado && contou && erros_semanticos == 0 && escopo_nivel() == 0);
}

/* Passa o codigo pelo parser e pela semantica e confere quantos erros semanticos surgiram */
static int testa_regra(const char *descricao, const char *codigo, int erros_esperados) {
    int resultado = parse_string(codigo);
    bool sintaxe_ok = resultado == 0 && yynerrs == 0;
    if (sintaxe_ok) {
        analisa(raiz_ast);
    }
    bool ok = sintaxe_ok && erros_semanticos == erros_esperados && escopo_nivel() == 0;
    int falhas = verifica(descricao, ok);
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    tabela_libera();
    erros_semanticos = 0;
    return falhas;
}

/* Analisa uma arvore montada a mao e confere a quantidade de erros.
 * Usado so para o for, que o parser ainda nao coloca na AST (T14) */
static int testa_arvore(const char *descricao, No *comandos, int erros_esperados) {
    No *raiz = no_programa(comandos, 1);
    analisa(raiz);
    return termina_semantico_com_erros(descricao, true, raiz, erros_esperados);
}

/* for (let i;;) { let i; } */
static No *arvore_for_com_i_no_corpo(void) {
    return no_for(decl(DECLARADOR_LET, "i", 1), NULL, NULL, no_bloco(decl(DECLARADOR_LET, "i", 2), 1), 1);
}

/* Passa o codigo pelo parser e pela semantica; quem chama confere o resultado e chama limpa_analise */
static bool analisa_codigo(const char *codigo) {
    int resultado = parse_string(codigo);
    bool sintaxe_ok = resultado == 0 && yynerrs == 0;
    if (sintaxe_ok) {
        analisa(raiz_ast);
    }
    return sintaxe_ok;
}

static void limpa_analise(void) {
    ast_libera(raiz_ast);
    raiz_ast = NULL;
    tabela_libera();
    erros_semanticos = 0;
}

/* Confere o tipo guardado na tabela para a variavel 'nome' no nivel de escopo dado */
static int testa_tipo_variavel(const char *descricao, const char *codigo, const char *nome, int nivel,
                               TipoDado esperado) {
    bool ok = analisa_codigo(codigo);
    const Simbolo *simbolo = simbolo_em(nome, nivel);
    ok = ok && simbolo && simbolo->tipo_inferido == esperado && erros_semanticos == 0 && escopo_nivel() == 0;
    int falhas = verifica(descricao, ok);
    limpa_analise();
    return falhas;
}

/* Comando de numero 'indice' (a partir de 0) no nivel global do programa */
static No *comando(int indice) {
    No *no = raiz_ast ? raiz_ast->filhos[0] : NULL;
    for (int i = 0; no && i < indice; i++) {
        no = no->proximo;
    }
    return no;
}

/* Confere o tipo anotado na expressao do comando (filhos[0]) e nos seus operandos, quando existem */
static int testa_anotacao(const char *descricao, const char *codigo, int indice,
                          TipoDado raiz, TipoDado esquerda, TipoDado direita) {
    bool ok = analisa_codigo(codigo) && erros_semanticos == 0;
    No *cmd = comando(indice);
    No *expr = cmd ? cmd->filhos[0] : NULL;
    ok = ok && expr && expr->tipo_inferido == raiz;
    if (ok && expr->filhos[0]) {
        ok = expr->filhos[0]->tipo_inferido == esquerda;
    }
    if (ok && expr->filhos[1]) {
        ok = expr->filhos[1]->tipo_inferido == direita;
    }
    int falhas = verifica(descricao, ok);
    limpa_analise();
    return falhas;
}

static int testa_anotacao_comandos(void) {
    bool ok = analisa_codigo("let x = true;\nx = \"a\";\n") && erros_semanticos == 0;
    No *declaracao = comando(0);
    No *atribuicao = comando(1);
    ok = ok && declaracao && declaracao->tipo_inferido == TIPO_BOOLEANO
        && atribuicao && atribuicao->tipo_inferido == TIPO_STRING;
    int falhas = verifica("Declaracao e atribuicao guardam o tipo do valor", ok);
    limpa_analise();
    return falhas;
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

    printf("\n--- AST: Integracao com o Parser ---\n");
    const TipoNo seq_comandos[] = { NO_DECL_VAR, NO_DECL_VAR, NO_ATRIBUICAO, NO_VAZIO, NO_BLOCO };
    const TipoNo seq_uma_decl[] = { NO_DECL_VAR };
    const TipoNo seq_duas_decl[] = { NO_DECL_VAR, NO_DECL_VAR };
    total++; falhas += testa_ast_linhas();
    total++; falhas += testa_ast_declaracoes();
    total++; falhas += testa_ast_funcao();
    total++; falhas += testa_ast_return();
    total++; falhas += testa_ast_chamada();
    total++; falhas += testa_ast_print();
    total++; falhas += testa_ast_parser("Programa vazio tem raiz sem comandos", "", NULL, 0, false);
    total++; falhas += testa_ast_parser("Lista preserva declaracoes, atribuicoes e comando vazio", "let a, b; a = 1; ; {}", seq_comandos, 5, false);
    total++; falhas += testa_ast_parser("Comandos sem AST sao ignorados", "if (true) ; else ; while (false) {} for (;;) ; let x;", seq_uma_decl, 1, false);
    total++; falhas += testa_ast_parser("Recuperacao ignora comando invalido e preserva o seguinte", "let a; x = ; let b;", seq_duas_decl, 2, true);
    total++; falhas += testa_ast_parser("Recuperacao por chave ignora NULL", "let a; x = } let b;", seq_duas_decl, 2, true);

    printf("\n--- Tabela de Simbolos ---\n");
    total++; falhas += testa_tabela_insere_busca();
    total++; falhas += testa_tabela_funcao();
    total++; falhas += testa_tabela_redeclaracao();
    total++; falhas += testa_tabela_niveis();
    total++; falhas += testa_tabela_busca_por_escopo();
    total++; falhas += testa_tabela_sair_escopo();
    total++; falhas += testa_tabela_mesmo_nome();
    total++; falhas += testa_tabela_libera();

    printf("\n--- Passada Semantica ---\n");
    total++; falhas += testa_semantico_variaveis();
    total++; falhas += testa_semantico_blocos();
    total++; falhas += testa_semantico_var_de_bloco();
    total++; falhas += testa_semantico_controle();
    total++; falhas += testa_semantico_for();
    total++; falhas += testa_semantico_dois_for();
    total++; falhas += testa_semantico_funcao();
    total++; falhas += testa_semantico_funcao_antes_do_corpo();
    total++; falhas += testa_semantico_nome_repetido();
    total++; falhas += testa_semantico_com_parser();
    total++; falhas += testa_semantico_erro();

    printf("\n--- Regras Semanticas: Declaracao e Uso ---\n");
    total++; falhas += testa_regra("Uso de variavel declarada nao gera erro", "let a = 1; let b = a + 1; b = a;", 0);
    total++; falhas += testa_regra("Uso de variavel nao declarada gera erro", "let a = b + 1;", 1);
    total++; falhas += testa_regra("Atribuicao a variavel nao declarada gera erro", "x = 1;", 1);
    total++; falhas += testa_regra("Cada uso nao declarado conta um erro", "x = y + z;", 3);
    total++; falhas += testa_regra("Inicializador nao enxerga a propria variavel", "let x = x + 1;", 1);
    total++; falhas += testa_regra("Variavel de bloco nao existe depois do bloco", "{ let a = 1; } let b = a;", 1);
    total++; falhas += testa_regra("Bloco interno enxerga variavel externa", "let a = 1; { { a = a + 1; } }", 0);
    total++; falhas += testa_regra("Uso nao declarado em console.log gera erro", "{ let a = 1; } console.log(a);", 1);
    total++; falhas += testa_regra("Funcao enxerga parametros e globais", "let g = 1; function f(a) { return a + g; }", 0);
    total++; falhas += testa_regra("Uso nao declarado dentro de funcao gera erro", "function f(a) { return a + b; }", 1);
    total++; falhas += testa_regra("Parametro nao existe fora da funcao", "function f(a) { return a; } let b = a;", 1);
    total++; falhas += testa_regra("Argumento nao declarado gera erro", "function f(a) { return a; } f(c);", 1);
    total++; falhas += testa_regra("Redeclaracao de let no mesmo escopo gera erro", "let x = 1; let x = 2;", 1);
    total++; falhas += testa_regra("Redeclaracao de var no mesmo escopo gera erro", "var x; var x;", 1);
    total++; falhas += testa_regra("Redeclaracao entre declaradores diferentes gera erro", "let x; const x = 1;", 1);
    total++; falhas += testa_regra("Redeclaracao na mesma lista gera erro", "let a = 1, a = 2;", 1);
    total++; falhas += testa_regra("Redeclaracao dentro de bloco gera erro", "{ let x; let x; }", 1);
    total++; falhas += testa_regra("Blocos irmaos podem repetir o nome", "{ let a = 1; } { let a = 2; }", 0);
    total++; falhas += testa_regra("Parametro repetido gera erro", "function f(a, a) { return a; }", 1);
    total++; falhas += testa_regra("Parametro redeclarado no corpo gera erro", "function f(a) { let a = 1; }", 1);
    total++; falhas += testa_regra("Funcao com nome de variavel gera erro", "let f = 1; function f() { return 1; }", 1);
    total++; falhas += testa_regra("Funcoes diferentes podem repetir parametros", "function f(a) { return a; } function g(a) { return a; }", 0);
    total++; falhas += testa_regra("Variavel em bloco com nome externo gera erro", "let x = 1; { let x = 2; }", 1);
    total++; falhas += testa_regra("Sombreamento em bloco aninhado gera erro", "{ let a; { { var a; } } }", 1);
    total++; falhas += testa_regra("Nome externo declarado depois do bloco nao e sombreamento", "{ let a = 1; } let a = 2;", 0);
    total++; falhas += testa_regra("Parametro pode ter o nome de uma global", "let x = 1; function f(x) { return x; }", 0);
    total++; falhas += testa_regra("Variavel local com nome de global gera erro", "let x = 1; function f() { let x = 2; return x; }", 1);
    total++; falhas += testa_regra("Bloco interno com nome de parametro gera erro", "function f(a) { { let a = 1; } }", 1);
    total++; falhas += testa_regra("Parametro com nome de parametro externo gera erro", "function f(a) { function g(a) { return a; } }", 1);
    total++; falhas += testa_arvore("Dois for seguidos podem repetir o let i", no_lista_adiciona(for_com_i(1), for_com_i(2)), 0);
    total++; falhas += testa_arvore("let no corpo do for com o nome do i gera erro", arvore_for_com_i_no_corpo(), 1);
    total++; falhas += testa_regra("Atribuicao a const gera erro", "const LIMITE = 10; LIMITE = 20;", 1);
    total++; falhas += testa_regra("Atribuicao a const dentro de bloco gera erro", "const c = 1; { c = c + 1; }", 1);
    total++; falhas += testa_regra("const pode ser lido normalmente", "const c = 1; let d = c * 2;", 0);
    total++; falhas += testa_regra("Atribuicao a let e var continua permitida", "let a = 1; var b = 2; a = b; b = a;", 0);

    printf("\n--- Verificacao de Tipos ---\n");
    total++; falhas += testa_tipo_variavel("Literal numerico da tipo numero", "let n = 1;", "n", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Literal string da tipo string", "let s = \"texto\";", "s", 0, TIPO_STRING);
    total++; falhas += testa_tipo_variavel("Literal booleano da tipo booleano", "let b = true;", "b", 0, TIPO_BOOLEANO);
    total++; falhas += testa_tipo_variavel("Variavel sem inicializacao e desconhecida", "let x;", "x", 0, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("Variavel copia o tipo de outra variavel", "let a = 1; let b = a;", "b", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Reatribuicao com o mesmo tipo mantem o tipo", "let a = 1; a = 2;", "a", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Reatribuicao com outro tipo vira desconhecido", "let a = 1; a = \"x\";", "a", 0, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("Depois de desconhecido nao volta a ser conhecido", "let a = 1; a = \"x\"; a = 2;", "a", 0, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("Parametro e desconhecido", "function f(p) { let c = p; }", "c", 1, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("Retorno de funcao e desconhecido", "function f() { return 1; } let r = f();", "r", 0, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("let externo lido dentro de funcao e desconhecido", "let g = 1; function f() { let c = g; }", "c", 1, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("const externo lido dentro de funcao mantem o tipo", "const k = 1; function f() { let c = k; }", "c", 1, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Variavel da propria funcao mantem o tipo", "function f() { let a = 1; let c = a; }", "c", 1, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Soma de numeros e numero", "let x = 1 + 2;", "x", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Operacoes aritmeticas dao numero", "let x = 1 - 2 * 3 / 4;", "x", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Concatenacao de strings e string", "let s = \"a\" + \"b\";", "s", 0, TIPO_STRING);
    total++; falhas += testa_tipo_variavel("Soma com booleano e numero", "let x = true + 1;", "x", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Soma com desconhecido e desconhecida", "function f(p) { let c = p + 1; }", "c", 1, TIPO_DESCONHECIDO);
    total++; falhas += testa_tipo_variavel("Aritmetica com desconhecido ainda e numero", "function f(p) { let c = p * 2; }", "c", 1, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("Comparacao da booleano", "let b = 1 < 2;", "b", 0, TIPO_BOOLEANO);
    total++; falhas += testa_tipo_variavel("Negacao logica da booleano", "let b = !1;", "b", 0, TIPO_BOOLEANO);
    total++; falhas += testa_tipo_variavel("Menos unario da numero", "let n = -1;", "n", 0, TIPO_NUMERO);
    total++; falhas += testa_tipo_variavel("&& entre booleanos e booleano", "let b = true && false;", "b", 0, TIPO_BOOLEANO);
    total++; falhas += testa_tipo_variavel("|| entre tipos diferentes e desconhecido", "let x = 1 || \"a\";", "x", 0, TIPO_DESCONHECIDO);
    total++; falhas += testa_anotacao("Expressao e operandos sao anotados", "let x = (1 + 2) < 4;", 0, TIPO_BOOLEANO, TIPO_NUMERO, TIPO_NUMERO);
    total++; falhas += testa_anotacao("Identificadores sao anotados com o tipo da variavel", "let s = \"a\"; let t = s + s;", 1, TIPO_STRING, TIPO_STRING, TIPO_STRING);
    total++; falhas += testa_anotacao("Argumento de console.log e anotado", "console.log(1 + 2);", 0, TIPO_NUMERO, TIPO_NUMERO, TIPO_NUMERO);
    total++; falhas += testa_anotacao("Argumento de chamada e anotado", "function f(a) { return a; } f(\"a\" + \"b\");", 1, TIPO_STRING, TIPO_STRING, TIPO_STRING);
    total++; falhas += testa_anotacao("Expressao de atribuicao e anotada", "let n = 1; n = n * 2;", 1, TIPO_NUMERO, TIPO_NUMERO, TIPO_NUMERO);
    total++; falhas += testa_anotacao("Chamada e anotada como desconhecida", "function f(a) { return a; } let r = f(1) + 1;", 1, TIPO_DESCONHECIDO, TIPO_DESCONHECIDO, TIPO_NUMERO);
    total++; falhas += testa_anotacao_comandos();

    int passaram = total - falhas;
    printf("\n-----------------------------------------\n");
    printf("Sumario dos Testes em Memoria:\n");
    printf("  Total: %d | Passaram: %d | Falharam: %d\n", total, passaram, falhas);
    printf("-----------------------------------------\n");

    return falhas > 0 ? 1 : 0;
}
