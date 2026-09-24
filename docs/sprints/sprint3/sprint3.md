# Documento de Planejamento - Sprint 3

## Visão Geral da Sprint
**Objetivo Principal:** Implementar o Analisador Sintático (Parser) utilizando GNU Bison, definir a gramática formal para o subconjunto Mini-JS, integrar os valores semânticos com o Analisador Léxico (Flex) e estruturar a suíte de testes automatizados com mecanismos de recuperação de erros.
**Período:** 16/09/2026 a 30/09/2026

---

## 1. Infraestrutura e Automação de Testes
Estabelecer um ambiente de compilação contínua e uma suíte de testes padronizada para validação sintática do compilador.

*   **Automação com Makefile:** Configurar scripts de compilação automática integrando `flex`, `bison` e `gcc`.
*   **Suíte de Casos de Teste:** Criar arquivos de teste em JavaScript cobrindo as construções do Mini-JS (expressões, declarações, controle de fluxo, funções e recuperação de erros).
*   **Pipeline de Validação:** Executar bateria de testes automatizados para verificar a aceitação de sintaxes válidas e a rejeição de sintaxes inválidas.

## 2. Implementação da Gramática Livre de Contexto (Bison)
Formalizar e codificar as regras de produção gramatical no arquivo `parser.y`.

*   **Precedência e Associatividade de Operadores:** Declarar regras de precedência para operadores aritméticos (`+`, `-`, `*`, `/`), relacionais (`==`, `!=`, `<`, `<=`, `>`, `>=`) e lógicos (`&&`, `||`, `!`).
*   **Desambiguação de Condicionais:** Resolver formalmente a ambiguidade do *dangling else* utilizando diretivas de precedência (`%prec LOWER_THAN_ELSE` e `ELSE`).
*   **Comandos e Declarações:** Implementar produções para declaração de variáveis (`let`, `const`, `var`), comandos de atribuição e laços de repetição (`while`, `for`).
*   **Estrutura do Programa:** Definir regras para comandos isolados, blocos delimitados por chaves `{ ... }` e a raiz do programa.

## 3. Declaração e Chamada de Funções Procedurais
Adicionar suporte à sintaxe de funções básicas compatíveis com a tradução para Python.

*   **Declarações de Funções:** Reconhecer funções nomeadas com lista de parâmetros formais (`function nome(a, b) { ... }`).
*   **Instruções de Retorno:** Suportar retorno de valores ou término antecipado de execução (`return;` e `return expressao;`).
*   **Chamadas de Funções:** Permitir invocações de funções por identificador dentro de expressões e comandos (`nome(arg1, arg2)`).
*   **Saída Padrão Embutida:** Tratar a chamada `console.log(...)` como instrução de impressão integrada.

## 4. Integração Semântica e Tratamento de Erros
Garantir a comunicação consistente entre Léxico e Sintático e a resiliência do Parser diante de falhas de sintaxe.

*   **Integração Léxico-Sintático:** Definir a união de tipos semânticos (`%union`) para números, literais de texto e identificadores, comunicados através da variável global `yylval` e do cabeçalho `parser.tab.h`.
*   **Recuperação e Sincronização de Erros:** Implementar regras com o token especial `error` sincronizado no ponto e vírgula (`error SEMICOLON`), permitindo ao parser reportar erros sem abortar prematuramente a leitura do arquivo.
*   **Rastreamento de Posição:** Utilizar contadores de linha (`yylineno`) para exibir mensagens informativas de erro sintático via rotina `yyerror()`.

---
