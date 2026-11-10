# Documento de Planejamento - Sprint 4

## Visão Geral da Sprint
**Objetivo Principal:** Aplicar no código as decisões de redução de escopo, construir a Árvore Sintática Abstrata (AST) a partir do parser, implementar a tabela de símbolos e a análise semântica sobre a AST e atualizar a documentação do projeto para a nova arquitetura.
**Período:** 04/11/2026 a 18/11/2026

---

## 1. Ajuste de Escopo no Léxico e no Sintático
Aplicar as decisões de redução de escopo antes do início da AST, para que a árvore seja construída sobre a gramática final.

**Responsável:** José Eduardo (@jevprado)

*   **Palavras Reservadas Fora do Escopo:** Remover os tokens sem uso do `lexer.l` e do `parser.y` e reconhecer as palavras reservadas do JS fora do escopo (`switch`, `class`, `new`, etc.) em uma única regra, que relata o erro com a linha e faz a compilação falhar.
*   **Negação Lógica:** Adicionar o operador `!` ao léxico e a regra de negação unária ao parser.
*   **Correções no Parser:** Remover a regra duplicada do `console.log`, para que o Bison rode sem conflitos.
*   **Erros Léxicos:** Fazer os erros léxicos (caractere inválido, string com quebra de linha) falharem a compilação e informarem a linha.

## 2. Árvore Sintática Abstrata (AST)
Fazer o parser construir a AST durante a análise sintática, em vez de apenas validar a entrada. A árvore é a representação intermediária usada pela análise semântica e, depois, pela geração de Python.

**Responsáveis:** Augusto (@augustogmedeiros) e Felipe (@felipemso)

*   **Estrutura dos Nós (`ast.h`):** Definir, em dupla, os tipos de nó, os tipos de dado inferidos e a estrutura do nó, que serve de contrato entre a frente da AST e a da semântica.
*   **Construtores e Liberação (`ast.c`):** Implementar as funções de criação de cada tipo de nó, guardando a linha de origem, e a liberação recursiva da árvore, com testes unitários. *(Augusto)*
*   **AST de Expressões e Literais:** Habilitar o rastreio de linhas (`%locations`) e construir os nós de expressões, preservando a precedência dos operadores. *(Augusto)*
*   **AST de Declarações, Atribuição e Blocos:** Construir o nó raiz do programa e os nós de declaração, atribuição e bloco. *(Felipe)*
*   **AST de Controle de Fluxo:** Construir os nós de `if`/`else`, `while` e `for`, com partes opcionais. *(Felipe)*
*   **AST de Funções e `console.log`:** Construir os nós de declaração de função, parâmetros, `return`, chamada e impressão. *(Augusto)*
*   **Impressão da AST:** Implementar a impressão indentada da árvore com a opção `--ast` e os testes de saída esperada. *(Felipe)*

## 3. Tabela de Símbolos e Análise Semântica
Criar a fase semântica do compilador: percorrer a AST, registrar os símbolos com seus escopos e aplicar as regras que decorrem das decisões de escopo.

**Responsáveis:** Carlos Eduardo (@cadumotta) e Pedro (@pedrogrocha13)

*   **Tabela de Símbolos (`tabela.c`):** Implementar a estrutura de símbolo, a pilha de escopos e as operações de inserção, busca local e busca global, com testes unitários. *(Carlos Eduardo)*
*   **Passada Semântica (`semantico.c`):** Percorrer a AST abrindo e fechando escopos de função, bloco e `for`, registrar variáveis, funções e parâmetros e padronizar as mensagens de erro semântico; opção `--tabela` para imprimir a tabela. *(Carlos Eduardo)*
*   **Integração no `main.c` e nos Testes:** Executar a semântica após o parser, informar em qual fase a compilação falhou e incluir os testes semânticos nos scripts `run_tests.sh` e `run_tests.ps1`. *(Pedro)*
*   **Regras de Declaração e Uso de Variáveis:** Uso de variável não declarada, redeclaração, sombreamento proibido e atribuição a `const`. *(Pedro)*
*   **Regras de Funções:** Funções aninhadas, declaração direto no corpo de estruturas de controle, chamada antes da declaração, número de argumentos e atribuição a variável global dentro de função. *(Carlos Eduardo)*
*   **Verificação de Tipos Simples:** Inferir o tipo das expressões (`numero`, `string`, `booleano` ou `desconhecido`) e rejeitar as combinações fora do escopo, como string com número no `+` e comparação entre tipos diferentes. *(Pedro)*

## 4. Documentação
Atualizar a documentação do site e do repositório para refletir o escopo reduzido e a nova arquitetura.

**Responsável:** José Eduardo (@jevprado)

*   **Escopo e Arquitetura:** Atualizar o que o compilador suporta, detalhar cada recurso fora do escopo com a justificativa e incluir a fase semântica na arquitetura e no diagrama de fluxo.
*   **Gramática Formal:** Documentar a gramática em BNF, a tabela de tokens, a precedência dos operadores, o *dangling else* e a recuperação de erros.
*   **Sprints e Retrospectiva:** Registrar o planejamento da Sprint 4, os problemas encontrados nas sprints anteriores e a decisão de reduzir o escopo.
*   **AST e Semântica:** Documentar os tipos de nó da AST, a tabela de símbolos e as regras semânticas, com revisão das duplas.
*   **README:** Atualizar a estrutura de pastas, as opções de linha de comando e as instruções de teste.

## 5. Organização do Trabalho
Com quatro frentes em paralelo, a sprint adota regras comuns para reduzir conflitos de merge.

*   **Uma Branch por Issue:** Cada tarefa é desenvolvida em sua própria branch (ex.: `feat/T10-ast-estrutura`), com Pull Request para `dev`; as tarefas de documentação vão para a branch `docs`.
*   **Commits Pequenos e Padronizados:** Um commit por mudança lógica, no padrão `tipo(escopo): descrição`, e todo commit de código compila e passa no `make test`.
*   **Testes Junto com o Código:** O teste de cada funcionalidade entra no mesmo commit ou na mesma issue.
*   **Ordem das Dependências:** O ajuste de escopo e a estrutura dos nós da AST vêm primeiro; a tabela de símbolos é independente da AST e começa em paralelo; a integração semântica no `main.c` vem antes das regras semânticas, para que os testes delas já rodem.

## 6. Divisão de Tarefas

| Issue | Tarefa | Frente | Responsável |
|---|---|---|---|
| [T09](https://github.com/felipemso/G16-Compiladores/issues/15) | Ajustes de escopo no léxico e sintático | Escopo | José Eduardo |
| [T10](https://github.com/felipemso/G16-Compiladores/issues/16) | Estrutura de nós da AST (`ast.h`) | AST | Augusto e Felipe |
| [T11](https://github.com/felipemso/G16-Compiladores/issues/17) | Construtores e liberação da AST (`ast.c`) | AST | Augusto |
| [T12](https://github.com/felipemso/G16-Compiladores/issues/18) | AST de expressões e literais | AST | Augusto |
| [T13](https://github.com/felipemso/G16-Compiladores/issues/19) | AST de declarações, atribuição e blocos | AST | Felipe |
| [T14](https://github.com/felipemso/G16-Compiladores/issues/20) | AST de controle de fluxo | AST | Felipe |
| [T15](https://github.com/felipemso/G16-Compiladores/issues/21) | AST de funções e `console.log` | AST | Augusto |
| [T16](https://github.com/felipemso/G16-Compiladores/issues/22) | Impressão da AST e testes de saída | AST | Felipe |
| [T17](https://github.com/felipemso/G16-Compiladores/issues/23) | Módulo da tabela de símbolos | Semântica | Carlos Eduardo |
| [T18](https://github.com/felipemso/G16-Compiladores/issues/24) | Passada semântica | Semântica | Carlos Eduardo |
| [T19](https://github.com/felipemso/G16-Compiladores/issues/25) | Regras de declaração e uso de variáveis | Semântica | Pedro |
| [T20](https://github.com/felipemso/G16-Compiladores/issues/26) | Regras semânticas de funções | Semântica | Carlos Eduardo |
| [T21](https://github.com/felipemso/G16-Compiladores/issues/27) | Verificação de tipos simples | Semântica | Pedro |
| [T22](https://github.com/felipemso/G16-Compiladores/issues/28) | Integração da semântica no `main.c` e nos testes | Semântica | Pedro |
| [T23](https://github.com/felipemso/G16-Compiladores/issues/29) | Documento de Escopo e Arquitetura | Documentação | José Eduardo |
| [T24](https://github.com/felipemso/G16-Compiladores/issues/30) | Gramática formal e tokens | Documentação | José Eduardo |
| [T25](https://github.com/felipemso/G16-Compiladores/issues/31) | Sprints e retrospectiva | Documentação | José Eduardo |
| [T26](https://github.com/felipemso/G16-Compiladores/issues/32) | AST, tabela de símbolos e regras semânticas | Documentação | José Eduardo |
| [T27](https://github.com/felipemso/G16-Compiladores/issues/33) | README | Documentação | José Eduardo |

---
