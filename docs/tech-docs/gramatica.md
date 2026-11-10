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

* **Ambiguidade resolvida fora das regras:** a regra `expressao` é ambígua de propósito (`a - b * c` pode ser agrupado de duas formas), e o mesmo vale para o `if` sem `else` (*dangling else*). O Bison resolve as duas ambiguidades com as declarações de [precedência e associatividade](#5-precedencia-e-associatividade) do `parser.y`, sem que seja preciso reescrever a gramática em vários níveis.
* **Atribuição é comando, não expressão:** `x = 1` só aparece em `comando_atribuicao`, `for_init` e `for_incr`. Construções como `a = b = 0;` ou `if (x = 1)` são erros de sintaxe.
* **`const` exige valor inicial:** `lista_declaradores_const` só aceita `item_declarador_inicializado`, então `const x;` é rejeitado pela própria gramática.
* **`const` não pode iniciar um `for`:** `for_init` usa `tipo_declarador`, que só aceita `let` e `var`.
* **`console.log` é um comando:** a impressão não é uma expressão e não pode ser usada como valor, por exemplo em `let x = console.log(1);`.
* **Chamada de função:** pode ser usada como comando (`soma(1, 2);`) ou dentro de uma expressão, por meio de `atomo`.
* **`return` fora de função:** a gramática aceita `comando_retorno` em qualquer lugar onde cabe um `comando`, e a restrição é verificada por uma ação semântica do próprio parser, que relata o erro `'return' fora de funcao`.
* **Corpo das estruturas de controle:** o corpo de `if`, `else`, `while` e `for` é um `comando` qualquer, com ou sem chaves. Por isso, a gramática aceita uma declaração direto no corpo (`if (c) let x = 1;`), embora isso esteja fora do escopo do projeto ([Escopo e Arquitetura, seção 2.2](escopo-e-arq.md#22-o-que-nao-faz-parte-do-projeto)); essa restrição não é imposta pelo parser.
* **Ações intermediárias omitidas:** o `parser.y` tem ações no meio de algumas regras (em `programa`, para reiniciar o estado, e em `declaracao_funcao`, para controlar se o parser está dentro de uma função). Elas não alteram a linguagem aceita e foram omitidas da notação.

---

## 5. Precedência e associatividade

A regra `expressao` é ambígua: `a + b * c` poderia ser lida como `(a + b) * c` ou como `a + (b * c)`. Em vez de reescrever a gramática com um não-terminal para cada nível de prioridade, o `parser.y` declara a precedência e a associatividade dos operadores, e o Bison usa essas declarações para resolver os conflitos da tabela de análise:

```text
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left OR
%left AND
%left EQ NEQ
%left LT LEQ GT GEQ
%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS NOT
```

No Bison, as declarações estão em ordem **crescente** de precedência: cada linha tem prioridade maior que a anterior, e os tokens da mesma linha têm a mesma prioridade. A tabela abaixo lista os operadores na ordem inversa, do que agrupa primeiro para o que agrupa por último.

| Nível | Operadores | Tokens | Associatividade | Exemplo | Agrupamento |
|---|---|---|---|---|---|
| 1 (maior) | `-` unário, `!` | `UMINUS`, `NOT` | direita | `-a * b`<br>`!!ok` | `(-a) * b`<br>`!(!ok)` |
| 2 | `*`, `/` | `TIMES`, `DIVIDE` | esquerda | `a / b * c` | `(a / b) * c` |
| 3 | `+`, `-` | `PLUS`, `MINUS` | esquerda | `a - b - c`<br>`a + b * c` | `(a - b) - c`<br>`a + (b * c)` |
| 4 | `<`, `<=`, `>`, `>=` | `LT`, `LEQ`, `GT`, `GEQ` | esquerda | `a + 1 < b` | `(a + 1) < b` |
| 5 | `==`, `!=` | `EQ`, `NEQ` | esquerda | `a < b == c < d` | `(a < b) == (c < d)` |
| 6 | `&&` | `AND` | esquerda | `a == b && c` | `(a == b) && c` |
| 7 (menor) | `||` | `OR` | esquerda | `a || b && c` | `a || (b && c)` |

Os parênteses (`'(' expressao ')'`) sempre têm prioridade sobre a tabela: `(a + b) * c` é agrupado como está escrito. A ordem dos níveis é a mesma do JavaScript.

### 5.1 Como o Bison aplica a tabela

Considere `a + b * c`. Depois de ler `a + b`, o parser poderia **reduzir** `expressao '+' expressao` ou **empilhar** o `*` que vem a seguir. O Bison compara a precedência da regra (a do seu último terminal, `PLUS`) com a do token seguinte (`TIMES`):

* se o token seguinte tem precedência **maior**, ele é empilhado: `a + (b * c)`;
* se tem precedência **menor**, a regra é reduzida: em `a * b + c`, o resultado é `(a * b) + c`;
* se a precedência é **igual**, decide a associatividade: `%left` reduz, gerando `(a - b) - c`, e `%right` empilha.

### 5.2 Menos unário (`UMINUS`)

O token `MINUS` é o mesmo para a subtração e para o menos unário, mas os dois precisam de prioridades diferentes. Por isso, o `parser.y` declara `UMINUS`, um **pseudo-token** que o léxico nunca produz e que existe apenas para carregar uma precedência. A regra do menos unário o utiliza com `%prec`:

```text
expressao ::= '-' expressao   %prec UMINUS
```

Assim, a regra assume a precedência de `UMINUS` (nível 1), e não a de `MINUS` (nível 3): `-a * b` é agrupado como `(-a) * b`. O `!` (`NOT`) não precisa desse recurso, porque é usado apenas como operador unário.

### 5.3 `LOWER_THAN_ELSE` e `ELSE`

As duas primeiras linhas da declaração não envolvem operadores: elas resolvem o *dangling else*, a ambiguidade do `if` sem `else`. `LOWER_THAN_ELSE` também é um pseudo-token, usado na regra do `if` sem `else`. O funcionamento é detalhado na [seção 6](#6-dangling-else).

### 5.4 Implicações para a geração de Python

A ordem dos operadores em Python não é idêntica à do JavaScript. Por isso, o gerador de código deverá emitir parênteses a partir da estrutura da AST, em vez de copiar a expressão como foi escrita:

* **`!` × `not`:** em Python, `not` tem precedência **menor** que as comparações. `!a == b` é `(!a) == b` em JS, mas `not a == b` é `not (a == b)` em Python; a tradução correta é `(not a) == b`.
* **Comparações encadeadas:** em JS, `a < b < c` é `(a < b) < c`, comparando o booleano `a < b` com `c`. Em Python, `a < b < c` significa `a < b and b < c`. Quando `c` é um número, a expressão já fica fora do escopo por comparar tipos diferentes ([Escopo e Arquitetura, seção 2.2](escopo-e-arq.md#22-o-que-nao-faz-parte-do-projeto)); mas o mesmo vale para `==` e `!=`, e `a == b == c` com três booleanos é válido no Mini-JS. Nesse caso, a tradução correta é `(a == b) == c`, e não `a == b == c`.

---

## 6. Dangling else

### 6.1 O problema

Como o corpo de um `if` pode ser outro `if`, e o `else` é opcional, um `else` depois de dois `if` aninhados pode pertencer a qualquer um deles:

```javascript
if (a)
    if (b)
        console.log("x");
    else
        console.log("y");
```

As duas leituras são válidas pela gramática:

* **`else` do `if` interno:** `if (a) { if (b) ... else ... }`
* **`else` do `if` externo:** `if (a) { if (b) ... } else ...`

Para o parser, a dúvida surge quando ele já leu `if (a) if (b) comando` e o próximo token é `else`. Ele pode **reduzir** `if (b) comando` como um `if` sem `else` (e o `else` ficaria com o `if (a)`) ou **empilhar** o `else` (e ele ficaria com o `if (b)`). É um conflito de empilhar/reduzir (*shift/reduce*).

### 6.2 A solução no `parser.y`

A regra do `if` sem `else` recebe a precedência do pseudo-token `LOWER_THAN_ELSE`, declarado com prioridade menor que a de `ELSE`:

```text
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

comando_if ::= 'if' '(' expressao ')' comando                    %prec LOWER_THAN_ELSE
             | 'if' '(' expressao ')' comando 'else' comando
```

No momento do conflito, o Bison compara a precedência da regra que poderia ser reduzida (`LOWER_THAN_ELSE`) com a do token seguinte (`ELSE`). Como `ELSE` tem precedência maior, o parser **empilha** o `else`, que fica associado ao **`if` mais próximo**, como no JavaScript (e no C e no Java). No exemplo acima, `console.log("y")` só executa quando `a` é verdadeiro e `b` é falso.

Para associar o `else` ao `if` externo, basta usar chaves:

```javascript
if (a) {
    if (b)
        console.log("x");
} else
    console.log("y");
```

### 6.3 Verificação

Com as declarações do `parser.y`, o Bison gera o parser sem nenhum conflito. Removendo o `%prec LOWER_THAN_ELSE` e as duas linhas `%nonassoc`, ele passa a relatar exatamente um conflito, no token `ELSE`, com este contraexemplo (`bison -Wcounterexamples`):

```text
warning: shift/reduce conflict on token ELSE [-Wcounterexamples]
  Example: IF LPAREN expressao RPAREN IF LPAREN expressao RPAREN comando • ELSE comando
```

Sem as declarações, o Bison também escolheria empilhar (é o seu comportamento padrão em conflitos de empilhar/reduzir), mas emitiria o aviso a cada compilação. Com elas, a escolha fica explícita na gramática.

---

## 7. Recuperação de erros

Sem recuperação, o parser pararia no primeiro erro de sintaxe. Para relatar vários erros em uma única execução, a regra `comando` tem duas produções com o token especial `error` do Bison:

```text
comando ::= ...
          | error ';'
          | error '}'
```

```c
| error SEMICOLON {
    yyerrok;
    fprintf(stderr, "[RECUPERACAO] Erro sintatico descartado ate ';'\n");
}
| error RBRACE {
    yyerrok;
    fprintf(stderr, "[RECUPERACAO] Erro em bloco descartado ate '}'\n");
}
```

### 7.1 Como funciona

Quando o parser encontra um token que não se encaixa em nenhuma regra:

1. **Relata o erro:** chama `yyerror`, que imprime `Erro sintatico na linha N: syntax error`, e incrementa o contador de erros (`yynerrs`).
2. **Desempilha estados** até encontrar um em que o token `error` possa ser empilhado, ou seja, o início de um `comando`. Como `comando` aparece dentro de `lista_comandos`, isso acontece no nível do programa ou dentro do bloco `{ ... }` mais interno.
3. **Descarta tokens da entrada** até encontrar um que possa vir depois de `error`: `;` ou `}`.
4. **Reduz** `error ';'` ou `error '}'` a um `comando` e continua a análise normalmente a partir do próximo token.

O trecho com erro é tratado como um comando descartado. Se o fim do arquivo for alcançado antes de um `;` ou `}`, a recuperação não é possível e a análise termina.

Depois de um erro, o Bison normalmente omite as mensagens de novos erros até que três tokens tenham sido lidos com sucesso, para evitar uma cascata de mensagens sobre o mesmo problema. A macro `yyerrok`, chamada nas duas regras, encerra esse modo imediatamente, para que erros em comandos consecutivos sejam todos relatados.

Os erros léxicos entram no mesmo mecanismo: o léxico devolve o token `YYUNDEF`, que não se encaixa em nenhuma regra e dispara a recuperação como qualquer erro de sintaxe.

Mesmo quando todos os erros são recuperados, a compilação falha: se `yynerrs` for maior que zero, o `main.c` informa a fase e a quantidade de erros, e a análise semântica não é executada.

### 7.2 Exemplos

As saídas abaixo foram obtidas com os arquivos de teste de `tests/invalidos/`.

**Erro em um comando (`08_erro_recuperado_comando.js`):** o `=` sem expressão gera um erro; o parser descarta até o `;` e analisa normalmente as linhas seguintes.

```javascript
let a = 1;
let b = ;
let c = 2;
console.log(c);
```

```text
Erro sintatico na linha 5: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Compilacao falhou na fase lexica/sintatica (1 erro(s))
```

**Erros em série (`10_multiplos_erros_em_serie.js`):** graças ao `yyerrok`, cada comando com erro é relatado separadamente, e o parser chega ao fim do arquivo.

```javascript
let a = ;
let b = ;
let c = ;
let d = 4;
console.log(d);
```

```text
Erro sintatico na linha 3: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Erro sintatico na linha 4: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Erro sintatico na linha 5: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Compilacao falhou na fase lexica/sintatica (3 erro(s))
```

Os números de linha incluem as linhas de comentário do início dos arquivos de teste, omitidas acima.

### 7.3 Limitação conhecida: erro sem `;` dentro de bloco

Quando um comando dentro de um bloco termina sem `;`, o próximo token aceito pela recuperação é o `}` que fecharia o bloco. A regra `error '}'` consome esse `}` como fim do comando com erro, e o bloco continua aberto. Os comandos seguintes passam a ser lidos como parte do bloco e, no fim do arquivo, falta um `}`, o que gera um segundo erro. É o que acontece em `09_erro_recuperado_bloco.js`:

```javascript
let x = 1;
{
    let y = 5
}
console.log(x);
```

```text
Erro sintatico na linha 6: syntax error
[RECUPERACAO] Erro em bloco descartado ate '}'
Erro sintatico na linha 8: syntax error
Compilacao falhou na fase lexica/sintatica (2 erro(s))
```

O erro da linha 8 (o fim do arquivo) é consequência do primeiro, e não um erro do programa. A compilação falha corretamente, mas a contagem de erros fica maior que a real.
