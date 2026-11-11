# Análise Semântica

A análise semântica verifica o que a gramática não consegue expressar: se cada nome usado foi declarado, se as declarações respeitam as regras de escopo, se as chamadas de função batem com as declarações e se os tipos das operações são compatíveis.

Ela é feita em **uma única passada sobre a AST** (`src/semantico.c`), apoiada na tabela de símbolos (`src/tabela.c`), e só é executada quando as análises léxica e sintática terminam sem erros. A passada não para no primeiro erro: cada erro é relatado no formato `Erro semantico na linha N: ...` e contado, e ao final, se houver algum, a compilação falha com a mensagem `Compilacao falhou na fase semantica (N erro(s))`.

---

## 1. Tabela de Símbolos

A tabela de símbolos guarda as informações de cada nome declarado no programa: variáveis, funções e parâmetros.

### 1.1 Estrutura do Símbolo

```c
typedef struct Simbolo {
    char *nome;
    CategoriaSimbolo categoria;
    TipoDado tipo_inferido;
    int linha;
    int num_parametros;
    int nivel_escopo;
    struct Simbolo *proximo_no_escopo;
    struct Simbolo *proximo_na_tabela;
} Simbolo;
```

| Campo | Conteúdo |
|---|---|
| `nome` | O nome declarado (uma cópia, que pertence à tabela). |
| `categoria` | O que o nome representa: `let`, `var`, `const`, função ou parâmetro. |
| `tipo_inferido` | O tipo conhecido do valor (`numero`, `string`, `booleano` ou `desconhecido`), atualizado durante a análise. |
| `linha` | A linha da declaração, usada nas mensagens de erro (por exemplo, "declarado na linha 3"). |
| `num_parametros` | Para funções, a quantidade de parâmetros, usada para conferir as chamadas. |
| `nivel_escopo` | A profundidade do escopo em que o nome foi declarado (`0` = global). |
| `proximo_no_escopo` | O próximo símbolo do mesmo escopo, usado nas buscas. |
| `proximo_na_tabela` | O próximo símbolo em ordem de declaração, usado na impressão e na liberação. |

### 1.2 Categorias

| Categoria | Origem |
|---|---|
| `SIMBOLO_LET` | `let x;` |
| `SIMBOLO_VAR` | `var x;` (com as mesmas regras de `let`, conforme a decisão de escopo) |
| `SIMBOLO_CONST` | `const x = 1;` |
| `SIMBOLO_FUNCAO` | `function f(...) { ... }` |
| `SIMBOLO_PARAMETRO` | cada parâmetro de uma função |

### 1.3 Operações

| Função | O que faz |
|---|---|
| `escopo_entra()` | Abre um escopo novo, um nível abaixo do atual. |
| `escopo_sai()` | Fecha o escopo atual e volta ao anterior. O escopo global nunca é fechado. |
| `escopo_nivel()` | Devolve o nível do escopo atual (`0` = global). |
| `tabela_insere(nome, categoria, tipo, linha)` | Insere um símbolo no escopo atual. Devolve `NULL` se o nome já existe nesse escopo. |
| `tabela_busca_local(nome)` | Procura o nome **apenas no escopo atual**. |
| `tabela_busca(nome)` | Procura o nome do escopo atual até o global, devolvendo a declaração mais próxima. |
| `tabela_imprime()` | Imprime todos os símbolos, em ordem de declaração (opção `--tabela`). |
| `tabela_libera()` | Libera todos os escopos e símbolos. |

### 1.4 Organização em Memória

Os escopos formam uma **pilha**: cada escopo aponta para o anterior, até chegar ao global. Cada escopo tem a sua própria lista de símbolos, encadeada por `proximo_no_escopo`, e as buscas percorrem essas listas.

Quando um escopo é fechado, ele sai da pilha e os seus nomes deixam de ser encontrados pelas buscas, mas os símbolos **não são apagados**: eles continuam numa segunda lista, global e em ordem de declaração, encadeada por `proximo_na_tabela`. É por isso que a opção `--tabela`, executada no fim da análise, consegue mostrar também os símbolos de funções e blocos que já foram fechados.

---

## 2. Escopos

### 2.1 Níveis de Escopo

O escopo global tem nível `0`, e cada escopo aberto dentro de outro tem o nível do anterior mais um. A análise abre um escopo novo em três situações:

| Construção | Escopo |
|---|---|
| **Função** | Abre um escopo com os parâmetros e o corpo. O bloco do corpo **não** abre um segundo escopo: parâmetros e variáveis locais ficam no mesmo nível. |
| **Bloco `{ ... }`** | Abre um escopo para as declarações do bloco, incluindo os blocos usados como corpo de `if`, `else` e `while`. |
| **`for`** | Abre um escopo para a inicialização (`let i = 0`). O bloco do corpo abre outro escopo, um nível abaixo. Assim, o `i` só existe dentro do laço, e dois `for (let i ...)` seguidos não entram em conflito. |

O `if` e o `while` não abrem escopo por si só: apenas os blocos do corpo deles. Como a declaração direto no corpo dessas estruturas é proibida (veja as regras), toda declaração interna fica dentro de um bloco.

Por decisão de escopo, **`var` é tratado como `let`**: a variável é registrada com a categoria `var`, mas tem escopo de bloco e segue as mesmas regras. Não há o escopo de função nem o *hoisting* do `var` do JavaScript.

### 2.2 Declaração e Uso de Nomes

*   **Declaração:** antes de inserir um nome, a análise procura o mesmo nome no escopo atual (`tabela_busca_local`), para detectar redeclaração, e nos escopos externos (`tabela_busca`), para detectar sombreamento. Se a declaração for inválida, o nome não é inserido e a declaração anterior continua valendo.
*   **Uso:** cada uso de um nome (leitura de variável, atribuição ou chamada de função) é resolvido por `tabela_busca`, do escopo atual até o global. Se o nome não for encontrado, o uso é um erro.
*   **Ordem:** a tabela é preenchida na ordem em que a AST é percorrida, que é a ordem do código. Por isso, um nome só pode ser usado depois da sua declaração. Em `let x = x;`, o inicializador é analisado antes da inserção, então o `x` da direita ainda não existe.
*   **Recursão:** a função é inserida na tabela antes da análise do seu corpo, o que permite que ela chame a si mesma.

### 2.3 Exemplo com `--tabela`

A opção `--tabela` imprime a tabela de símbolos ao final da análise:

```bash
./minijs --tabela programa.js
```

```javascript
let total = 0;
var nome = "Mini-JS";

function soma(a, b) {
    let resultado = a + b;
    return resultado;
}

for (let i = 0; i < 3; i = i + 1) {
    let dobro = i * 2;
    total = total + dobro;
}

for (let i = 0; i < 2; i = i + 1) {
    total = soma(total, i);
}

{
    const ativo = true;
    console.log(nome, ativo);
}
```

```text
Nivel  Nome                 Categoria  Tipo          Linha  Parametros
0      total                let        desconhecido  1      -
0      nome                 var        string        2      -
0      soma                 funcao     desconhecido  4      2
1      a                    parametro  desconhecido  4      -
1      b                    parametro  desconhecido  4      -
1      resultado            let        desconhecido  5      -
1      i                    let        numero        9      -
2      dobro                let        numero        10     -
1      i                    let        numero        14     -
1      ativo                const      booleano      19     -
```

O que a tabela mostra:

*   **Função:** `soma` fica no escopo global e registra `2` parâmetros; `a`, `b` e `resultado` ficam no mesmo nível (`1`), porque o corpo da função não abre um segundo escopo.
*   **`for`:** o `i` de cada laço fica no escopo do `for` (nível `1`), e `dobro` fica no bloco do corpo (nível `2`). Os dois `i` são símbolos diferentes, em escopos diferentes.
*   **Bloco solto:** `ativo` fica no escopo do bloco `{ ... }` (nível `1`).
*   **Tipos:** a coluna `Tipo` mostra o estado no **fim** da análise. `total` começou como `numero`, mas recebeu o retorno de `soma(...)`, que é desconhecido, e por isso terminou como `desconhecido`. Parâmetros e funções são sempre `desconhecido`, e `resultado` também, por ser a soma de dois parâmetros.

---

## 3. Regras Semânticas

As regras abaixo são todas as verificações feitas pela análise semântica. A maioria decorre das decisões de escopo do projeto, cuja justificativa (a diferença entre JavaScript e Python) está em [Escopo e Arquitetura, seção 2.2](escopo-e-arq.md#22-o-que-nao-faz-parte-do-projeto).

Os exemplos foram executados com o compilador; a linha indicada na mensagem é a linha do próprio exemplo. A pasta `tests/invalidos_semanticos/` tem casos de teste para as regras, com exceção da regra 6.

### 3.1 Resumo

| # | Regra | Mensagem |
|---|---|---|
| 1 | Uso de nome não declarado | `identificador 'a' nao declarado` |
| 2 | Redeclaração no mesmo escopo | `'x' ja foi declarado neste escopo (linha N)` |
| 3 | Sombreamento de nome de escopo externo | `'total' sombreia a declaracao da linha N em escopo externo` |
| 4 | Atribuição a `const` | `atribuicao a constante 'LIMITE' (declarada na linha N)` |
| 5 | Função fora do escopo global | `funcao 'interna' declarada fora do escopo global` |
| 6 | Declaração direto no corpo de `if`/`else`/`while`/`for` | `declaracao de 'x' direto no corpo do if, sem bloco` |
| 7 | Chamada de função não declarada ou antes da declaração | `funcao 'dobro' nao declarada antes da chamada` |
| 8 | Chamada de algo que não é função | `'x' nao e uma funcao (declarado na linha N)` |
| 9 | Número de argumentos diferente do de parâmetros | `funcao 'soma' espera 2 argumento(s), mas recebeu 1` |
| 10 | Atribuição a variável global dentro de função | `atribuicao a variavel global 'contador' dentro de funcao` |
| 11 | String com número ou booleano no `+` | `operador '+' entre string e numero (...)` |
| 12 | String em operação aritmética | `operador '*' nao pode ser usado com string` |
| 13 | Comparação entre tipos diferentes | `comparacao '>=' entre tipos diferentes (numero e string)` |

### 3.2 Declaração e Uso de Variáveis

#### Regra 1: Uso de nome não declarado

Todo nome usado (em uma expressão, em uma atribuição ou como argumento) precisa ter sido declarado antes, no escopo atual ou em um escopo externo. Isso inclui usar uma variável fora do bloco em que ela foi declarada e usar a própria variável no seu inicializador (`let x = x + 1;`).

```javascript
{
    let a = 1;
}
console.log(a);
```

```text
Erro semantico na linha 4: identificador 'a' nao declarado
```

#### Regra 2: Redeclaração no mesmo escopo

Um nome não pode ser declarado duas vezes no mesmo escopo, qualquer que seja a forma (`let`, `var`, `const`, função ou parâmetro). Isso vale também para `var x; var x;`, que o JavaScript aceitaria, para parâmetros repetidos (`function f(a, a)`) e para uma variável local com o nome de um parâmetro, já que os dois ficam no mesmo escopo.

```javascript
let x = 1;
let x = 2;
```

```text
Erro semantico na linha 2: 'x' ja foi declarado neste escopo (linha 1)
```

#### Regra 3: Sombreamento de nome de escopo externo

Não é permitido declarar, em um escopo interno, um nome que já existe em um escopo externo. Isso vale para blocos, laços e para variáveis locais de função com o nome de uma global.

```javascript
let total = 0;
{
    let total = 1;
}
```

```text
Erro semantico na linha 3: 'total' sombreia a declaracao da linha 1 em escopo externo
```

**Exceção:** um **parâmetro** pode ter o nome de uma variável global, porque parâmetros são locais à função tanto em JS quanto em Python. O programa abaixo é válido:

```javascript
let x = 1;
function f(x) {
    return x * 2;
}
```

#### Regra 4: Atribuição a `const`

Uma variável declarada com `const` não pode receber outro valor.

```javascript
const LIMITE = 100;
LIMITE = 200;
```

```text
Erro semantico na linha 2: atribuicao a constante 'LIMITE' (declarada na linha 1)
```

### 3.3 Funções

#### Regra 5: Função fora do escopo global

Funções só podem ser declaradas no escopo global. Uma função dentro de outra função ou dentro de um bloco é um erro.

```javascript
function externa() {
    function interna() {
        return 1;
    }
    return 2;
}
```

```text
Erro semantico na linha 2: funcao 'interna' declarada fora do escopo global
```

#### Regra 6: Declaração direto no corpo de `if`/`else`/`while`/`for`

Uma declaração de variável ou de função não pode ser o corpo inteiro de uma estrutura de controle: ela precisa estar dentro de um bloco `{ }`. A gramática aceita essa forma, e a verificação é feita aqui.

```javascript
let c = true;
if (c)
    let x = 1;
```

```text
Erro semantico na linha 3: declaracao de 'x' direto no corpo do if, sem bloco
```

#### Regra 7: Chamada de função não declarada ou antes da declaração

A função precisa ser declarada antes de ser chamada. Como a função entra na tabela antes da análise do seu corpo, a recursão é permitida.

```javascript
let r = dobro(2);
function dobro(x) {
    return x * 2;
}
```

```text
Erro semantico na linha 1: funcao 'dobro' nao declarada antes da chamada
```

#### Regra 8: Chamada de algo que não é função

Só é possível chamar nomes declarados como função.

```javascript
let x = 10;
x();
```

```text
Erro semantico na linha 2: 'x' nao e uma funcao (declarado na linha 1)
```

#### Regra 9: Número de argumentos diferente do de parâmetros

A chamada precisa ter exatamente tantos argumentos quanto a função tem parâmetros, nem a menos nem a mais.

```javascript
function soma(a, b) {
    return a + b;
}
let r = soma(1);
```

```text
Erro semantico na linha 4: funcao 'soma' espera 2 argumento(s), mas recebeu 1
```

#### Regra 10: Atribuição a variável global dentro de função

Uma função pode **ler** variáveis globais, mas não **atribuir** a elas. Atribuir a um parâmetro, mesmo que ele tenha o nome de uma global, é permitido.

```javascript
let contador = 0;
function incrementa() {
    contador = contador + 1;
}
```

```text
Erro semantico na linha 3: atribuicao a variavel global 'contador' dentro de funcao
```

### 3.4 Tipos

A análise infere o tipo de cada expressão (`numero`, `string`, `booleano` ou `desconhecido`) e guarda o resultado no campo `tipo_inferido` do nó da AST. O tipo de uma variável vem do seu inicializador. As regras de tipo só geram erro quando **os dois tipos envolvidos são conhecidos**. Para não gerar erros falsos, ficam como `desconhecido`:

*   parâmetros e o retorno de funções, cujo tipo depende de quem chama;
*   variáveis reatribuídas com um valor de outro tipo, a partir da reatribuição;
*   variáveis `let` e `var` externas lidas dentro de uma função, já que a função pode ser chamada depois de uma reatribuição (as `const` mantêm o tipo).

Por exemplo, o programa abaixo é válido: o retorno de `dobro` é desconhecido, e `b` deixa de ter tipo conhecido ao ser reatribuída com uma string.

```javascript
function dobro(x) {
    return x * 2;
}
let a = dobro(2) + "!";
let b = 1;
b = "um";
let c = b * 2;
```

#### Regra 11: String com número ou booleano no `+`

O `+` só aceita string com string (concatenação) ou números e booleanos entre si (soma). Misturar string com número ou booleano, em qualquer ordem, é um erro.

```javascript
let nome = "Ana";
let idade = 20;
let texto = nome + idade;
```

```text
Erro semantico na linha 3: operador '+' entre string e numero (string so pode ser concatenada com string)
```

#### Regra 12: String em operação aritmética

Os operadores `-`, `*` e `/` e o menos unário não podem ser usados com strings.

```javascript
let preco = "10";
let total = preco * 2;
let n = -preco;
```

```text
Erro semantico na linha 2: operador '*' nao pode ser usado com string
Erro semantico na linha 3: operador '-' nao pode ser usado com string
```

#### Regra 13: Comparação entre tipos diferentes

Os operadores `==`, `!=`, `<`, `<=`, `>` e `>=` só podem comparar valores do mesmo tipo.

```javascript
let idade = 18;
let maior = idade >= "18";
```

```text
Erro semantico na linha 2: comparacao '>=' entre tipos diferentes (numero e string)
```
