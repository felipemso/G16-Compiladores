# Gramática do Mini-JS

Esta página descreve a gramática livre de contexto aceita pelo analisador sintático do compilador. Ela é fiel ao arquivo `parser/parser.y` (Bison), na versão posterior ao ajuste de escopo léxico e sintático (T09): as regras, os nomes dos não-terminais e a ordem das alternativas são os mesmos do código.

---

## 1. Notação

A gramática está escrita em BNF, com as seguintes convenções:

| Elemento | Significado | Exemplo |
|---|---|---|
| `::=` | "é definido como" | `bloco ::= '{' lista_comandos '}'` |
| <code>&#124;</code> | alternativa | <code>'let' &#124; 'var'</code> |
| `ε` | produção vazia (nenhum símbolo) | <code>params_opt ::= ε &#124; lista_params</code> |
| `minusculas` | não-terminal (mesmo nome usado no `parser.y`) | `expressao` |
| `'texto'` | terminal com lexema fixo | `'while'`, `';'`, `'&&'` |
| `MAIUSCULAS` | terminal cujo lexema varia (identificadores e literais) | `ID`, `NUM` |
| `error` | token especial do Bison usado na recuperação de erros | `error ';'` |

Os terminais com lexema fixo aparecem pelo próprio texto para facilitar a leitura; no `parser.y`, cada um corresponde a um token nomeado (por exemplo, `'let'` é `LET` e `';'` é `SEMICOLON`). O único terminal fixo com mais de uma palavra é `'console.log'`, reconhecido pelo léxico como um token só (`PRINT`).

Assim como no `parser.y`, as listas são escritas com **recursão à esquerda** (`lista ::= item | lista ',' item`), forma preferida pelo Bison por não acumular símbolos na pilha.

---

## 2. Gramática

O símbolo inicial é `programa`.

### 2.1 Programa, blocos e comandos

```text
programa        ::= lista_comandos

lista_comandos  ::= ε
                  | lista_comandos comando

bloco           ::= '{' lista_comandos '}'

comando         ::= declaracao_var
                  | comando_atribuicao
                  | declaracao_funcao
                  | comando_retorno
                  | chamada_funcao ';'
                  | 'console.log' '(' args_opt ')' ';'
                  | comando_if
                  | comando_while
                  | comando_for
                  | bloco
                  | ';'
                  | error ';'
                  | error '}'
```

### 2.2 Declarações de variáveis

```text
declaracao_var               ::= tipo_declarador lista_declaradores ';'
                               | 'const' lista_declaradores_const ';'

tipo_declarador              ::= 'let'
                               | 'var'

lista_declaradores           ::= item_declarador
                               | lista_declaradores ',' item_declarador

item_declarador              ::= ID
                               | item_declarador_inicializado

lista_declaradores_const     ::= item_declarador_inicializado
                               | lista_declaradores_const ',' item_declarador_inicializado

item_declarador_inicializado ::= ID '=' expressao
```

### 2.3 Atribuição

```text
comando_atribuicao ::= ID '=' expressao ';'
```

### 2.4 Funções

```text
declaracao_funcao ::= 'function' ID '(' params_opt ')' bloco

params_opt        ::= ε
                    | lista_params

lista_params      ::= ID
                    | lista_params ',' ID

comando_retorno   ::= 'return' ';'
                    | 'return' expressao ';'

chamada_funcao    ::= ID '(' args_opt ')'

args_opt          ::= ε
                    | lista_args

lista_args        ::= expressao
                    | lista_args ',' expressao
```

### 2.5 Estruturas de controle

```text
comando_if    ::= 'if' '(' expressao ')' comando
                | 'if' '(' expressao ')' comando 'else' comando

comando_while ::= 'while' '(' expressao ')' comando

comando_for   ::= 'for' '(' for_init ';' for_cond ';' for_incr ')' comando

for_init      ::= ε
                | tipo_declarador lista_declaradores
                | ID '=' expressao

for_cond      ::= ε
                | expressao

for_incr      ::= ε
                | ID '=' expressao
                | expressao
```

### 2.6 Expressões

```text
expressao ::= expressao '||' expressao
            | expressao '&&' expressao
            | expressao '==' expressao
            | expressao '!=' expressao
            | expressao '<'  expressao
            | expressao '<=' expressao
            | expressao '>'  expressao
            | expressao '>=' expressao
            | expressao '+'  expressao
            | expressao '-'  expressao
            | expressao '*'  expressao
            | expressao '/'  expressao
            | '-' expressao
            | '!' expressao
            | '(' expressao ')'
            | atomo

atomo     ::= ID
            | chamada_funcao
            | NUM
            | STRING_VAL
            | BOOLEAN_VAL
```

---

## 3. Observações sobre a gramática

* **Ambiguidade resolvida fora das regras:** a regra `expressao` é ambígua de propósito (`a - b * c` pode ser agrupado de duas formas), e o mesmo vale para o `if` sem `else` (*dangling else*). O Bison resolve as duas ambiguidades com as declarações de precedência e associatividade do `parser.y`, sem que seja preciso reescrever a gramática em vários níveis.
* **Atribuição é comando, não expressão:** `x = 1` só aparece em `comando_atribuicao`, `for_init` e `for_incr`. Construções como `a = b = 0;` ou `if (x = 1)` são erros de sintaxe.
* **`const` exige valor inicial:** `lista_declaradores_const` só aceita `item_declarador_inicializado`, então `const x;` é rejeitado pela própria gramática.
* **`const` não pode iniciar um `for`:** `for_init` usa `tipo_declarador`, que só aceita `let` e `var`.
* **`console.log` é um comando:** a impressão não é uma expressão e não pode ser usada como valor, por exemplo em `let x = console.log(1);`.
* **Chamada de função:** pode ser usada como comando (`soma(1, 2);`) ou dentro de uma expressão, por meio de `atomo`.
* **`return` fora de função:** a gramática aceita `comando_retorno` em qualquer lugar onde cabe um `comando`, e a restrição é verificada por uma ação semântica do próprio parser, que relata o erro `'return' fora de funcao`.
* **Corpo das estruturas de controle:** o corpo de `if`, `else`, `while` e `for` é um `comando` qualquer, com ou sem chaves. Por isso, a gramática aceita uma declaração direto no corpo (`if (c) let x = 1;`), embora isso esteja fora do escopo do projeto ([Escopo e Arquitetura, seção 2.2](escopo-e-arq.md#22-o-que-nao-faz-parte-do-projeto)); essa restrição não é imposta pelo parser.
* **Ações intermediárias omitidas:** o `parser.y` tem ações no meio de algumas regras (em `programa`, para reiniciar o estado, e em `declaracao_funcao`, para controlar se o parser está dentro de uma função). Elas não alteram a linguagem aceita e foram omitidas da notação.
