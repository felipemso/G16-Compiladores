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

O critério de corte é a tradução direta: quando um recurso se comporta de forma diferente em JavaScript e em Python, traduzi-lo exigiria gerar código extra para imitar o comportamento do JS. Nesses casos, o recurso fica fora do escopo e o programa é rejeitado, em vez de gerar um Python que rode com resultado diferente do JS original.

#### Paradigma e sintaxe geral
* **Sem Orientação a Objetos:** o projeto é estritamente procedural. Não usamos classes (`class`), instanciação (`new`) nem `this`.
* **Sem Funções Complexas:** nada de *arrow functions* (`() => {}`), funções anônimas, funções dentro de variáveis ou *callbacks*. Apenas funções declaradas tradicionais são aceitas.
* **Ponto e Vírgula Obrigatório:** toda instrução deve terminar com `;` (sem inserção automática de `;`).

#### Estruturas de dados
* **Arrays:** `[1, 2, 3]` e `lista[0]` não são aceitos. Arrays do JS e listas do Python divergem em pontos básicos: em JS, ler um índice inexistente devolve `undefined` e atribuir além do fim cria "buracos" no array; em Python, as duas operações geram `IndexError`. O tamanho também é obtido de formas diferentes (`lista.length` × `len(lista)`).

#### Funções e escopo
* **Funções aninhadas:** funções só podem ser declaradas no nível global. Em JS, uma função interna altera livremente as variáveis da função externa; em Python, isso exige declarar `nonlocal`, senão a atribuição cria uma variável local nova.
* **Atribuição a variável global dentro de função:** uma função pode **ler** uma variável global, mas não **atribuir** a ela. Em JS, `x = 1;` dentro da função altera o `x` global; em Python, a mesma atribuição cria um `x` local (e uma leitura anterior dele gera `UnboundLocalError`), a menos que se declare `global x`.
* **Sombreamento de nomes:** não é permitido declarar, em um bloco interno, uma variável com o mesmo nome de outra já visível. Em JS, `let` tem escopo de bloco, então o `x` interno é uma variável nova e o externo continua intacto; em Python, `if`, `while` e `for` não criam escopo, então o `x` interno sobrescreveria o externo. **Exceção:** um parâmetro pode ter o mesmo nome de uma variável global, porque parâmetros são locais à função nas duas linguagens e o comportamento é o mesmo.
* **`var` tratado como `let`:** `var` é aceito, mas segue as mesmas regras de `let`. O `var` original do JS tem escopo de função e sofre *hoisting* (pode ser usado antes da linha da declaração, valendo `undefined`); Python não tem nada equivalente.
* **Declaração direto no corpo de `if`/`else`/`while`/`for`:** uma declaração não pode ser o corpo inteiro de uma estrutura de controle, como em `if (c) let x = 1;`; ela precisa estar dentro de um bloco com `{ }`. O próprio JS proíbe isso para `let` e `const` (é erro de sintaxe), e, como `var` é tratado como `let`, a regra vale para os três.
* **Número de argumentos diferente do número de parâmetros:** `soma(1)` ou `soma(1, 2, 3)` para `function soma(a, b)` são rejeitados. Em JS, argumentos faltantes viram `undefined` e os excedentes são ignorados; em Python, os dois casos geram `TypeError`.
* **Chamada de função antes da declaração:** a função deve ser declarada antes de ser chamada (recursão é permitida, pois a função já existe dentro do próprio corpo). Em JS, declarações de função sofrem *hoisting* e podem ser chamadas antes da linha onde aparecem; em Python, o `def` só passa a existir quando é executado, e a chamada antecipada gera `NameError`.

#### Tipos e operações
* **String com número ou booleano no `+`:** `"total: " + 10` é rejeitado. Em JS, o número é convertido para texto (`"total: 10"`); em Python, somar `str` com `int`/`bool` gera `TypeError`.
* **String em operação aritmética:** `"3" * 2` e `"a" - 1` são rejeitados. Em JS, a string é convertida para número (`6` e `NaN`); em Python, `"3" * 2` repete o texto (`"33"`) e `"a" - 1` gera `TypeError`.
* **Comparação entre tipos diferentes:** `1 == "1"` e `1 < "2"` são rejeitados. Em JS, os operandos são convertidos antes da comparação (`true` nos dois casos); em Python, `1 == "1"` é `False` e `1 < "2"` gera `TypeError`.

#### Recursos léxicos
* **Palavras reservadas fora do escopo:** `do`, `switch`, `case`, `break`, `continue`, `try`, `catch`, `throw`, `class`, `new`, `this`, `import`, `export`, `typeof`, entre outras. Elas pertencem a construções não suportadas e são rejeitadas com uma mensagem própria, em vez de serem aceitas como identificadores comuns.
* **`++`, `+=` e `%`:** apenas a atribuição simples é aceita (`i = i + 1;`). Python não tem `++` (e `++x` é válido em Python, mas significa `+(+x)`, sem incrementar nada). O `%` tem resultado diferente para números negativos: `-7 % 3` vale `-1` em JS e `2` em Python. O `+=` ficou fora para manter uma única forma de atribuição na gramática.
* **Sequências de escape em strings:** `\n`, `\t`, `\"`, `\u{...}` etc. não são tratadas. Os conjuntos de escapes do JS e do Python não são idênticos (por exemplo, `\u{1F600}` só existe em JS, e escapes desconhecidos como `\d` são tratados de forma diferente).
* **Notação científica:** `1e3` e `2.5E-4` não são reconhecidos; os números aceitos são inteiros (`42`) e decimais (`3.14`). É uma simplificação do analisador léxico.

#### Etapas do compilador
* **Otimização e código intermediário:** o compilador não gera código intermediário (como código de três endereços) nem aplica otimizações. A própria AST serve como representação intermediária, e o Python é gerado diretamente a partir dela. Como Python é uma linguagem de alto nível, com estruturas equivalentes às do Mini-JS, uma representação de nível mais baixo não traria ganho para a tradução.

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