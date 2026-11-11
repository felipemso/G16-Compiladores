# Árvore Sintática Abstrata (AST)

A AST é a representação intermediária do compilador: uma árvore em memória que guarda a estrutura do programa sem os detalhes da sintaxe concreta (parênteses, `;`, chaves). Ela é construída pelas ações semânticas do Bison durante a própria análise sintática, percorrida pela análise semântica e, na próxima etapa, servirá de base para a geração de Python.

A definição está em `src/ast.h` e os construtores, a impressão e a liberação da árvore estão em `src/ast.c`.

---

## 1. Estrutura do Nó

Todos os nós da árvore usam a mesma estrutura, `struct No`:

```c
#define NO_MAX_FILHOS 4

typedef struct No {
    TipoNo categoria;
    TipoDado tipo_inferido;
    int linha;
    Operador operador;
    Declarador declarador;
    union {
        double numero;
        char *texto;
        int booleano;
    } valor;
    struct No *filhos[NO_MAX_FILHOS];
    struct No *proximo;
} No;
```

| Campo | Conteúdo |
|---|---|
| `categoria` | O tipo do nó (`NO_IF`, `NO_BINARIO`, etc.), conforme a [seção 2](#2-tipos-de-no). |
| `tipo_inferido` | O tipo de dado da expressão, preenchido pela análise semântica. Vale `TIPO_DESCONHECIDO` até lá. |
| `linha` | A linha do código-fonte em que a construção começa, usada nas mensagens de erro semântico. |
| `operador` | O operador de um nó `NO_BINARIO` ou `NO_UNARIO`. Nos demais nós vale `OP_NENHUM`. |
| `declarador` | Em `NO_DECL_VAR`, indica se a variável foi declarada com `let`, `var` ou `const`. |
| `valor` | O dado do nó, quando existe: número, texto (nomes e strings) ou booleano. |
| `filhos` | Até 4 filhos, cada um em uma posição com papel fixo (por exemplo, no `if`: condição, então, senão). |
| `proximo` | O próximo irmão, para formar listas (comandos, parâmetros, argumentos). |

### 1.1 Filhos e Listas

A árvore combina duas formas de ligação:

*   **Filhos em posições fixas (`filhos[0..3]`):** cada tipo de nó define o papel de cada posição. Um filho opcional ausente fica como `NULL`, como o `else` de um `if` sem `else` ou as partes vazias de um `for`.
*   **Listas de irmãos (`proximo`):** sequências de tamanho variável são listas encadeadas. O filho aponta para o primeiro elemento, e os demais seguem pelo `proximo`. É assim que são representados os comandos de um programa ou bloco, os parâmetros de uma função e os argumentos de uma chamada ou de um `console.log`.

As listas são montadas por `no_lista_adiciona`, que ignora elementos `NULL`. Assim, as construções que não geram nó, como um comando descartado pela recuperação de erros, simplesmente não aparecem na lista.

Uma declaração com várias variáveis (`let a = 1, b = 2;`) gera **um nó `NO_DECL_VAR` por variável**, encadeados como comandos irmãos.

### 1.2 Campo `valor`

| Campo da `union` | Usado por |
|---|---|
| `texto` | `NO_DECL_VAR`, `NO_ATRIBUICAO`, `NO_FUNCAO`, `NO_PARAMETRO`, `NO_CHAMADA` e `NO_ID` (o nome); `NO_STRING` (o texto, com as aspas) |
| `numero` | `NO_NUMERO` (sempre `double`, inteiro ou decimal) |
| `booleano` | `NO_BOOLEANO` (`1` para `true`, `0` para `false`) |

O texto é alocado pelo léxico (`strdup`) e passa a pertencer ao nó: ele é liberado junto com a árvore, por `ast_libera`, que percorre os filhos e as listas recursivamente.

### 1.3 Enumerações Auxiliares

**`TipoDado`**, o tipo inferido pela análise semântica:

| Valor | Significado |
|---|---|
| `TIPO_DESCONHECIDO` | Tipo que não dá para saber em tempo de compilação (parâmetros, retorno de funções, variáveis reatribuídas com outro tipo) |
| `TIPO_NUMERO` | Número |
| `TIPO_STRING` | Texto |
| `TIPO_BOOLEANO` | `true` ou `false` |

**`Operador`**, usado por `NO_BINARIO` e `NO_UNARIO`:

| Valor | Operador | Valor | Operador |
|---|---|---|---|
| `OP_SOMA` | `+` | `OP_MENOR` | `<` |
| `OP_SUBTRACAO` | `-` | `OP_MENOR_IGUAL` | `<=` |
| `OP_MULTIPLICACAO` | `*` | `OP_MAIOR` | `>` |
| `OP_DIVISAO` | `/` | `OP_MAIOR_IGUAL` | `>=` |
| `OP_IGUAL` | `==` | `OP_E` | `&&` |
| `OP_DIFERENTE` | `!=` | `OP_OU` | `||` |
| `OP_NEGATIVO` | `-` unário | `OP_NAO` | `!` |

O valor `OP_NENHUM` indica um nó sem operador.

**`Declarador`**: `DECLARADOR_LET`, `DECLARADOR_VAR` e `DECLARADOR_CONST`.

---

## 2. Tipos de Nó

O `enum TipoNo` define 19 categorias. A tabela mostra a construção do Mini-JS que gera cada uma, os campos que ela usa e o papel de cada filho.

### 2.1 Estrutura do Programa

| Categoria | Gerado por | Campos | Filhos |
|---|---|---|---|
| `NO_PROGRAMA` | o programa inteiro (raiz da árvore) | — | `[0]` lista de comandos |
| `NO_BLOCO` | `{ ... }` | — | `[0]` lista de comandos |
| `NO_VAZIO` | `;` sozinho | — | — |

### 2.2 Declarações e Atribuição

| Categoria | Gerado por | Campos | Filhos |
|---|---|---|---|
| `NO_DECL_VAR` | `let x = 1;`, `var y;`, `const z = 2;` (um nó por variável) | `declarador`, `valor.texto` (nome) | `[0]` inicializador (opcional) |
| `NO_ATRIBUICAO` | `x = 10;` e as atribuições na inicialização e no incremento do `for` | `valor.texto` (nome) | `[0]` expressão |

### 2.3 Controle de Fluxo

| Categoria | Gerado por | Campos | Filhos |
|---|---|---|---|
| `NO_IF` | `if (c) ...` e `if (c) ... else ...` | — | `[0]` condição, `[1]` então, `[2]` senão (opcional) |
| `NO_WHILE` | `while (c) ...` | — | `[0]` condição, `[1]` corpo |
| `NO_FOR` | `for (init; cond; incr) ...` | — | `[0]` inicialização, `[1]` condição, `[2]` incremento, `[3]` corpo (os três primeiros são opcionais) |

No `for`, a inicialização é uma lista de `NO_DECL_VAR` (`let i = 0`) ou um `NO_ATRIBUICAO` (`i = 0`), e o incremento é um `NO_ATRIBUICAO` (`i = i + 1`) ou uma expressão qualquer.

### 2.4 Funções e Saída

| Categoria | Gerado por | Campos | Filhos |
|---|---|---|---|
| `NO_FUNCAO` | `function soma(a, b) { ... }` | `valor.texto` (nome) | `[0]` lista de parâmetros (opcional), `[1]` corpo (`NO_BLOCO`) |
| `NO_PARAMETRO` | cada parâmetro da função | `valor.texto` (nome) | — |
| `NO_RETURN` | `return;` e `return expressao;` | — | `[0]` expressão (opcional) |
| `NO_CHAMADA` | `soma(1, 2)`, como comando ou dentro de uma expressão | `valor.texto` (nome da função) | `[0]` lista de argumentos (opcional) |
| `NO_PRINT` | `console.log(...)` | — | `[0]` lista de argumentos (opcional) |

Um `return` fora de função não gera nó: o parser relata o erro e descarta o comando.

### 2.5 Expressões

| Categoria | Gerado por | Campos | Filhos |
|---|---|---|---|
| `NO_BINARIO` | `a + b`, `a < b`, `a && b`, etc. | `operador` | `[0]` operando esquerdo, `[1]` operando direito |
| `NO_UNARIO` | `-x` e `!x` | `operador` (`OP_NEGATIVO` ou `OP_NAO`) | `[0]` operando |
| `NO_ID` | uso de uma variável | `valor.texto` (nome) | — |
| `NO_NUMERO` | `42`, `3.14` | `valor.numero` | — |
| `NO_STRING` | `"texto"`, `'texto'` | `valor.texto` | — |
| `NO_BOOLEANO` | `true`, `false` | `valor.booleano` | — |

Os parênteses não geram nó: eles só determinam a forma da árvore. Por isso, `(a + b) * c` e `a + b * c` geram árvores diferentes, e a precedência dos operadores fica registrada na própria estrutura.

### 2.6 Tipo Inferido

A análise semântica percorre a AST e preenche o campo `tipo_inferido` dos nós de expressão (`NO_NUMERO`, `NO_STRING`, `NO_BOOLEANO`, `NO_ID`, `NO_BINARIO`, `NO_UNARIO` e `NO_CHAMADA`), além de `NO_DECL_VAR` e `NO_ATRIBUICAO`, que recebem o tipo do valor atribuído. O retorno de uma chamada de função conta sempre como `TIPO_DESCONHECIDO`.
