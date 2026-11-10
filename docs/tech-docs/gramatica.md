# Gramática do Mini-JS

Esta página descreve a gramática livre de contexto aceita pelo analisador sintático do compilador. Ela é fiel ao arquivo `parser/parser.y` (Bison), na versão posterior ao ajuste de escopo léxico e sintático (T09): as regras, os nomes dos não-terminais e a ordem das alternativas são os mesmos do código.

---

## 1. Notação

A gramática está escrita em BNF, com as seguintes convenções:

| Elemento | Significado | Exemplo |
|---|---|---|
| `::=` | "é definido como" | `bloco ::= '{' lista_comandos '}'` |
| `|` | alternativa | `'let' | 'var'` |
| `ε` | produção vazia (nenhum símbolo) | `params_opt ::= ε | lista_params` |
| `minusculas` | não-terminal (mesmo nome usado no `parser.y`) | `expressao` |
| `'texto'` | terminal com lexema fixo | `'while'`, `';'`, `'&&'` |
| `MAIUSCULAS` | terminal cujo lexema varia (identificadores e literais) | `ID`, `NUM` |
| `error` | token especial do Bison usado na recuperação de erros | `error ';'` |

Os terminais com lexema fixo aparecem pelo próprio texto para facilitar a leitura; no `parser.y`, cada um corresponde a um token nomeado (por exemplo, `'let'` é `LET` e `';'` é `SEMICOLON`), conforme a [tabela de tokens](#2-tokens). O único terminal fixo com mais de uma palavra é `'console.log'`, reconhecido pelo léxico como um token só (`PRINT`).

Assim como no `parser.y`, as listas são escritas com **recursão à esquerda** (`lista ::= item | lista ',' item`), forma preferida pelo Bison por não acumular símbolos na pilha.

---

## 2. Tokens

Os tokens são reconhecidos pelo analisador léxico (`lexer/lexer.l`, Flex) e declarados no `parser.y` com `%token`. As expressões regulares abaixo estão na sintaxe do Flex, exatamente como no `lexer.l`: texto entre aspas é literal, `[...]` é uma classe de caracteres, `*` é "zero ou mais", `+` é "um ou mais" e `?` é "opcional".

Quando mais de uma regra reconhece a entrada, o Flex escolhe a que consome **o maior lexema** e, em caso de empate, **a que aparece primeiro** no arquivo. Por isso as palavras reservadas vêm antes da regra de identificadores (`let` vira `LET`, mas `letra` vira `ID`), e `!=` é reconhecido como `NEQ`, e não como `NOT` seguido de `ASSIGN`.

### 2.1 Palavras reservadas

| Token | Expressão regular | Exemplo |
|---|---|---|
| `LET` | `"let"` | `let x = 1;` |
| `CONST` | `"const"` | `const PI = 3.14;` |
| `VAR` | `"var"` | `var total;` |
| `FUNCTION` | `"function"` | `function soma(a, b) { ... }` |
| `IF` | `"if"` | `if (x > 0) { ... }` |
| `ELSE` | `"else"` | `else { ... }` |
| `WHILE` | `"while"` | `while (i < 10) { ... }` |
| `FOR` | `"for"` | `for (let i = 0; i < 3; i = i + 1) { ... }` |
| `RETURN` | `"return"` | `return a + b;` |
| `PRINT` | `"console.log"` | `console.log("oi");` |

### 2.2 Literais e identificadores

| Token | Expressão regular | Exemplo | Valor enviado ao parser |
|---|---|---|---|
| `BOOLEAN_VAL` | `"true"` e `"false"` (duas regras) | `true`, `false` | `1` ou `0` (`booleano`) |
| `NUM` | `[0-9]+(\.[0-9]+)?` | `42`, `3.14` | o número convertido com `atof` (`valor_num`) |
| `STRING_VAL` | `\"[^\"\r\n]*\"|\'[^\'\r\n]*\'` | `"olá"`, `'mundo'` | o texto, com as aspas (`texto`) |
| `ID` | `[a-zA-Z_][a-zA-Z0-9_]*` | `soma`, `_total1` | o nome (`texto`) |

O valor de cada literal é passado ao parser pelo `yylval`, no campo da `%union` indicado entre parênteses.

### 2.3 Operadores

| Token | Expressão regular | Exemplo |
|---|---|---|
| `EQ` | `"=="` | `a == b` |
| `NEQ` | `"!="` | `a != b` |
| `NOT` | `"!"` | `!ativo` |
| `LEQ` | `"<="` | `a <= b` |
| `GEQ` | `">="` | `a >= b` |
| `LT` | `"<"` | `a < b` |
| `GT` | `">"` | `a > b` |
| `AND` | `"&&"` | `a && b` |
| `OR` | `"||"` | `a || b` |
| `ASSIGN` | `"="` | `x = 10;` |
| `PLUS` | `"+"` | `a + b` |
| `MINUS` | `"-"` | `a - b`, `-x` |
| `TIMES` | `"*"` | `a * b` |
| `DIVIDE` | `"/"` | `a / b` |

### 2.4 Delimitadores

| Token | Expressão regular | Exemplo |
|---|---|---|
| `SEMICOLON` | `";"` | `x = 1;` |
| `LPAREN` | `"("` | `soma(1, 2)` |
| `RPAREN` | `")"` | `soma(1, 2)` |
| `LBRACE` | `"{"` | `{ let x; }` |
| `RBRACE` | `"}"` | `{ let x; }` |
| `COMMA` | `","` | `let a, b;` |

### 2.5 Padrões descartados

Estes padrões são reconhecidos, mas não geram token: o léxico simplesmente os ignora.

| Padrão | Expressão regular | Exemplo |
|---|---|---|
| Espaços em branco | `[ \t\n\r]+` | espaços, tabulações e quebras de linha |
| Comentário de linha | `"//".*` | `// comentário` |
| Comentário de bloco | `"/*"([^*]|\*+[^*/])*\*+"/"` | `/* comentário */` |

### 2.6 Erros léxicos

Os padrões abaixo indicam erros. O léxico imprime uma mensagem com o número da linha e devolve o token especial `YYUNDEF`, que o parser trata como erro de sintaxe. Assim, os erros léxicos entram na contagem de erros e no mecanismo de recuperação de erros da análise sintática.

| Situação | Expressão regular | Exemplo | Mensagem |
|---|---|---|---|
| String com quebra de linha | `(\"[^\"\r\n]*|\'[^\'\r\n]*)/[\r\n]` | `"abc` + quebra de linha | `Erro lexico: string com quebra de linha na linha N` |
| Palavra reservada fora do escopo | `"do"|"switch"|"case"|...|"debugger"` | `switch (x) { ... }` | `palavra reservada 'switch' fora do escopo na linha N` |
| Caractere inválido | `.` (qualquer caractere não reconhecido pelas regras anteriores) | `@`, `%`, `[` | `Erro lexico: Caractere invalido '@' na linha N` |

As palavras reservadas fora do escopo são: `do`, `switch`, `case`, `break`, `continue`, `try`, `catch`, `throw`, `class`, `extends`, `import`, `from`, `export`, `new`, `in`, `of`, `delete`, `default`, `finally`, `this`, `typeof`, `instanceof`, `void`, `super`, `with`, `yield`, `await`, `enum` e `debugger`. A regra delas aparece antes da regra de identificadores, para que não sejam aceitas como nomes de variáveis.

Na regra de string com quebra de linha, o `/[\r\n]` é um *contexto à direita*: a quebra de linha é verificada, mas não é consumida, para que o erro seja relatado na linha em que a string começou.

---

## 3. Gramática

O símbolo inicial é `programa`.

### 3.1 Programa, blocos e comandos

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

### 3.2 Declarações de variáveis

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

### 3.3 Atribuição

```text
comando_atribuicao ::= ID '=' expressao ';'
```

### 3.4 Funções

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

### 3.5 Estruturas de controle

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

### 3.6 Expressões

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

## 4. Observações sobre a gramática

* **Ambiguidade resolvida fora das regras:** a regra `expressao` é ambígua de propósito (`a - b * c` pode ser agrupado de duas formas), e o mesmo vale para o `if` sem `else` (*dangling else*). O Bison resolve as duas ambiguidades com as declarações de precedência e associatividade do `parser.y`, sem que seja preciso reescrever a gramática em vários níveis.
* **Atribuição é comando, não expressão:** `x = 1` só aparece em `comando_atribuicao`, `for_init` e `for_incr`. Construções como `a = b = 0;` ou `if (x = 1)` são erros de sintaxe.
* **`const` exige valor inicial:** `lista_declaradores_const` só aceita `item_declarador_inicializado`, então `const x;` é rejeitado pela própria gramática.
* **`const` não pode iniciar um `for`:** `for_init` usa `tipo_declarador`, que só aceita `let` e `var`.
* **`console.log` é um comando:** a impressão não é uma expressão e não pode ser usada como valor, por exemplo em `let x = console.log(1);`.
* **Chamada de função:** pode ser usada como comando (`soma(1, 2);`) ou dentro de uma expressão, por meio de `atomo`.
* **`return` fora de função:** a gramática aceita `comando_retorno` em qualquer lugar onde cabe um `comando`, e a restrição é verificada por uma ação semântica do próprio parser, que relata o erro `'return' fora de funcao`.
* **Corpo das estruturas de controle:** o corpo de `if`, `else`, `while` e `for` é um `comando` qualquer, com ou sem chaves. Por isso, a gramática aceita uma declaração direto no corpo (`if (c) let x = 1;`), embora isso esteja fora do escopo do projeto ([Escopo e Arquitetura, seção 2.2](escopo-e-arq.md#22-o-que-nao-faz-parte-do-projeto)); essa restrição não é imposta pelo parser.
* **Ações intermediárias omitidas:** o `parser.y` tem ações no meio de algumas regras (em `programa`, para reiniciar o estado, e em `declaracao_funcao`, para controlar se o parser está dentro de uma função). Elas não alteram a linguagem aceita e foram omitidas da notação.
