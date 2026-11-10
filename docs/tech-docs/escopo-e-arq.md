# Documento de Arquitetura e Escopo

## 1. Visão Geral
O projeto desenvolve um compilador que traduz código JavaScript básico (Mini-JS) diretamente para Python.

---

## 2. Escopo

O compilador traduz apenas comandos de JavaScript que possuem equivalente direto em Python.

### 2.1 O que o Compilador Suporta
* **Variáveis:** declarações com `let`, `const` e `var`, com ou sem valor inicial (`const` exige valor inicial). Várias variáveis podem ser declaradas em uma única instrução: `let a = 1, b = 2;`.
* **Atribuição:** comandos simples de atribuição (`x = 10;`).
* **Condicionais:** `if` e `else`.
* **Repetição:** laços `while` e laços `for` tradicionais (`for (let i = 0; i < n; i = i + 1) { ... }`).
* **Funções Básicas:**
  * Declaração de funções com nome e parâmetros: `function soma(a, b) { ... }`.
  * Retorno com `return;` ou `return expressao;` (apenas dentro de funções).
  * Chamada de funções, como comando ou dentro de expressões: `soma(2, 3)`.
* **Expressões e Operadores:**
  * Matemática: `+`, `-`, `*`, `/` e `-` unário.
  * Comparação: `==`, `!=`, `<`, `<=`, `>`, `>=`.
  * Lógica: `&&` (e), `||` (ou) e `!` (negação lógica unária, traduzida para `not` em Python).
  * Parênteses para prioridade: `(a + b) * c`.
* **Tipos de Dados:**
  * Números (inteiros e decimais).
  * Textos (strings com aspas simples ou duplas, em uma única linha).
  * Booleanos (`true` e `false`).
* **Saída:** comando `console.log(...)` com zero ou mais argumentos separados por vírgula (traduzido diretamente para `print(...)`).
* **Comentários:** de linha (`// ...`) e de bloco (`/* ... */`), descartados pelo analisador léxico.

### 2.2 O que NÃO Faz Parte do Projeto
* **Sem Orientação a Objetos:** o projeto é estritamente procedural. Não usamos classes (`class`), instanciação (`new`) nem `this`.
* **Sem Funções Complexas:** nada de *arrow functions* (`() => {}`), funções anônimas, funções dentro de variáveis ou *callbacks*. Apenas funções declaradas tradicionais são aceitas.
* **Ponto e Vírgula Obrigatório:** toda instrução deve terminar com `;` (sem inserção automática de `;`).

---

## 3. Arquitetura do Compilador

O fluxo de compilação acontece em 4 etapas lineares:

1. **Léxico (Flex):** lê o texto em JS, gera os tokens e descarta comentários e espaços.
2. **Sintático (Bison):** valida a estrutura do código e as regras gramaticais.
3. **Árvore Sintática (AST):** organiza o programa validado em uma estrutura de árvore na memória.
4. **Emissão de Código:** percorre a árvore e gera o arquivo `.py` correspondente, devidamente identado.

### Tecnologias
* **Linguagem:** C
* **Ferramentas:** Flex (`lexer.l`) e Bison (`parser.y`)
* **Comunicação:** arquivo `parser.tab.h` (tokens) e estrutura `%union` / `yylval` para troca de dados entre léxico e sintático.