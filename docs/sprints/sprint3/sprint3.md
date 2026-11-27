# Documento de Planejamento - Sprint 3

## Visão Geral da Sprint
**Objetivo Principal:** Implementar o Analisador Sintático (Parser) utilizando GNU Bison, definir a gramática formal para o subconjunto de JavaScript adotado pelo projeto, integrar os valores semânticos com o Analisador Léxico (Flex) e estruturar a suíte de testes automatizados com mecanismos de recuperação de erros.
**Período:** 16/09/2026 a 30/09/2026

## Resultado Esperado
Ao final da sprint, o compilador deverá reconhecer a estrutura sintática das construções planejadas da linguagem de entrada e reportar violações da gramática. Um programa como `function soma(a, b) { return a + b; } console.log(soma(2, 3));` deverá ser aceito sintaticamente. Entradas com instruções incompletas deverão gerar diagnósticos e permitir a recuperação nos pontos de sincronização previstos.

## Escopo da Sprint
*   Integração entre Flex e Bison por tokens e valores semânticos.
*   Gramática de programas, blocos, declarações, atribuições, expressões e controle de fluxo.
*   Declarações e chamadas de funções, retornos e impressão com `console.log`.
*   Precedência, associatividade, resolução de ambiguidades e recuperação de erros sintáticos.
*   Automação de compilação e testes de aceitação e rejeição sintática.

## Exclusões e Continuidade
*   A construção de uma AST persistente e sua utilização nas fases seguintes não constituem entrega desta sprint; mensagens de reconhecimento não equivalem a uma árvore construída.
*   A tabela de símbolos e as verificações gerais de nomes, escopos e tipos ficam para a etapa semântica. Verificações pontuais no parser não substituem essa etapa.
*   A geração de Python permanece para uma etapa posterior.
*   A negação lógica `!`, mencionada no planejamento, recebe ajustes de implementação na Sprint 6; sua presença neste documento não comprova conclusão nesta sprint.

---

## Resultados da Sprint
O analisador sintático foi concluído dentro do escopo previsto para esta sprint, incluindo sua integração com o analisador léxico. Essa conclusão se refere à etapa de reconhecimento sintático, sem representar a entrega da AST ou da análise semântica previstas para etapas posteriores.

## Organização e Responsáveis
As atividades foram realizadas em conjunto pela equipe, com Felipe Melo de Sousa à frente do trabalho no analisador sintático.

## Problemas Encontrados e Soluções
A principal dificuldade foi integrar os analisadores léxico e sintático, compreendendo como estabelecer a comunicação entre os dois componentes para que os tokens reconhecidos pelo lexer fossem utilizados pelo parser.

A integração foi alcançada combinando os exemplos apresentados em aula com experimentação prática na configuração das ferramentas. O ponto decisivo foi compartilhar as definições dos tokens pelo cabeçalho `parser.tab.h`, gerado pelo Bison e incluído no arquivo do Flex, e estabelecer a passagem de valores semânticos por meio de `%union` e `yylval`.

A automação da compilação pelo Makefile e a execução conjunta de testes com casos válidos e inválidos consolidaram a integração, permitindo verificar o fluxo completo entre os analisadores léxico e sintático.

## 1. Infraestrutura e Automação de Testes
Estabelecer um ambiente de compilação contínua e uma suíte de testes padronizada para validação sintática do compilador.

*   **Automação com Makefile:** Configurar scripts de compilação automática integrando `flex`, `bison` e `gcc`.
*   **Suíte de Casos de Teste:** Criar arquivos de teste em JavaScript cobrindo as construções da linguagem de entrada (expressões, declarações, controle de fluxo, funções e recuperação de erros).
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

## 5. Problemas Encontrados e Soluções
Registro das dificuldades que surgiram durante a sprint e de como foram resolvidas.

### 5.1 Conflito do *Dangling Else*
*   **Problema:** Como o `else` é opcional, um `else` depois de dois `if` aninhados (`if (a) if (b) x; else y;`) pode pertencer a qualquer um deles. Para o Bison, isso aparece como um conflito de empilhar/reduzir (*shift/reduce*) no token `ELSE`.
*   **Solução:** Declarar o pseudo-token `LOWER_THAN_ELSE` com precedência menor que a de `ELSE` (`%nonassoc LOWER_THAN_ELSE` e `%nonassoc ELSE`) e aplicá-lo à regra do `if` sem `else` com `%prec LOWER_THAN_ELSE`. Assim o `else` fica associado ao `if` mais próximo, como no JavaScript. A solução entrou junto com as estruturas de controle (T06), e o conflito nunca chegou à branch `dev`. A explicação completa está na página de Gramática.

### 5.2 Conflito do `console.log`
*   **Problema:** Após o merge da tarefa de funções (T07), o Bison passou a relatar um conflito de empilhar/reduzir no token `)`. Havia duas regras para o mesmo comando: a original, `console.log(expressao)`, e a nova, `console.log(args_opt)`, que aceita zero ou mais argumentos. Com um único argumento, as duas regras se aplicam, e o parser não sabia se fechava a impressão pela primeira regra ou se reduzia o argumento a uma lista para a segunda.
*   **Causa:** A T07 substituiu a regra original pela nova, mas, na resolução do conflito de merge com a `dev`, as duas versões foram mantidas no `parser.y`.
*   **Solução:** Remover a regra antiga, mantendo só `console.log(args_opt)`. A correção foi feita na branch da T07 em 30/09, mas depois que o Pull Request já tinha sido aceito; por isso ela nunca chegou à `dev`, e o conflito permaneceu até ser corrigido de vez na Sprint 4, na tarefa de ajuste de escopo (T09). Desde então, o Bison gera o parser sem nenhum conflito.

### 5.3 Merge de Branches
*   **Problema:** As tarefas da sprint (estrutura do programa, declarações, controle de fluxo, funções e recuperação de erros) foram desenvolvidas em paralelo, em branches separadas, e quase todas alteravam os mesmos arquivos: o `parser/parser.y` e o `tests/unit_tests.c`. Os merges com a `dev` geraram conflitos nesses dois arquivos, e a resolução manual de um deles foi a origem do conflito do `console.log`.
*   **Solução:** Na sprint, os conflitos foram resolvidos trazendo a `dev` para dentro de cada branch antes do merge final. Para a Sprint 4, a equipe adotou regras para evitar o problema: uma branch por issue, Pull Requests pequenos, *rebase* na `dev` antes de abrir o PR (já que o `parser.y` é compartilhado), revisão por alguém de outra frente e a exigência de que todo commit compile e passe no `make test`.

---
