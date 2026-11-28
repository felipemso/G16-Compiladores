# Geração de Código Python

A geração de código é a última fase do compilador: ela percorre a AST e escreve o programa equivalente em Python. Fica em `src/gerador.c`, e a interface pública é uma única função:

```c
void gera_python(const No *raiz, FILE *saida);
```

A geração só é executada quando as análises léxica, sintática e semântica terminam **sem nenhum erro**. Assim, o gerador sempre recebe uma AST completa e válida, e nunca precisa tratar nós faltando ou nomes não declarados. Duas opções de linha de comando acionam a geração:

| Opção | Efeito |
|---|---|
| `--python` | Imprime o Python gerado na saída padrão |
| `-o saida.py` | Grava o Python gerado no arquivo `saida.py` |

```bash
./minijs --python programa.js
./minijs programa.js -o programa.py
python3 programa.py
```

Se a compilação falhar, nenhum arquivo é criado.

---

## 1. Como o Gerador Percorre a AST

### 1.1 Estado do Gerador

Durante a geração, o gerador guarda três informações:

```c
typedef struct {
    FILE *saida;
    int nivel;
    int linhas;
} Gerador;
```

| Campo | Para que serve |
|---|---|
| `saida` | Para onde o Python é escrito: o terminal (`--python`) ou o arquivo de `-o` |
| `nivel` | Nível de indentação atual; cada nível equivale a 4 espaços |
| `linhas` | Quantas linhas já foram escritas; serve para saber se um corpo ficou vazio |

### 1.2 Comandos e Expressões

O percurso é feito por duas funções recursivas, uma para cada tipo de nó:

*   **`gera_comando`** trata os nós de comando (declaração, atribuição, `if`, `while`, `for`, função, `return`, chamada e `console.log`). Cada comando ocupa uma ou mais **linhas inteiras**: a linha começa com a indentação do nível atual e termina com uma quebra de linha.
*   **`gera_expressao`** trata os nós de expressão (literais, identificadores, operações e chamadas). Uma expressão é escrita **no meio de uma linha**, sem indentação e sem quebra de linha.

Uma declaração `let x = a + 1;`, por exemplo, é escrita por `gera_comando`, que abre a linha, escreve `x = ` e chama `gera_expressao` para o inicializador. Esta, por sua vez, chama a si mesma para cada operando da soma.

Listas de comandos, parâmetros e argumentos são percorridas pelo ponteiro `proximo`, do mesmo jeito que na AST. Parâmetros e argumentos são escritos separados por `, `.

### 1.3 Indentação e `pass`

Python marca blocos pela indentação, e não por chaves. Sempre que um comando tem corpo (`if`, `else`, `while`, `for` e `function`), o gerador:

1. escreve a linha de abertura terminada em `:` (por exemplo, `if x > 0:`);
2. aumenta o `nivel` e gera os comandos do corpo, que saem com 4 espaços a mais;
3. se nenhuma linha foi escrita no corpo, escreve `pass`;
4. volta ao `nivel` anterior.

O passo 3 existe porque Python não aceita um bloco vazio. Corpos como `{}`, `;` ou `{ ; {} }` não geram nenhuma linha, e o gerador percebe isso comparando o contador `linhas` antes e depois do corpo.

```javascript
if (x > 0) {
}
function nada() {
}
```

```python
if x > 0:
    pass
def nada():
    pass
```

---

## 2. Tradução de Cada Construção

### 2.1 Declarações e Valores

| Construção | Mini-JS | Python |
|---|---|---|
| Declaração com valor (`let`, `var` ou `const`) | `let x = 10;` | `x = 10` |
| Declaração sem valor | `let y;` | `y = None` |
| Declaração múltipla | `let a = 1, b;` | `a = 1` e `b = None`, uma por linha |
| Atribuição | `x = x + 1;` | `x = x + 1` |
| Número inteiro e decimal | `10`, `3.14`, `10.0` | `10`, `3.14`, `10` |
| String | `"ola"`, `'oi'` | `"ola"`, `'oi'` (mesmas aspas) |
| Booleano | `true`, `false` | `True`, `False` |

O tipo de declaração (`let`, `var` ou `const`) não aparece no Python, que não tem declaração de variável. A diferença entre eles já foi verificada pela análise semântica (por exemplo, a atribuição a `const`).

### 2.2 Operadores

| Construção | Mini-JS | Python |
|---|---|---|
| Aritméticos | `+`, `-`, `*`, `/` | `+`, `-`, `*`, `/` |
| Relacionais | `==`, `!=`, `<`, `<=`, `>`, `>=` | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| Lógicos | `a && b`, `a || b`, `!a` | `a and b`, `a or b`, `not a` |
| Menos unário | `-a` | `-a` |
| Parênteses | `(a + 1) * 2` | `(a + 1) * 2` |

A divisão `/` do Python 3 sempre devolve um número real (`7 / 2` é `3.5`), como no JavaScript. Os parênteses não são copiados do código original: o gerador recalcula onde eles são necessários (veja a seção 3.3).

### 2.3 Controle de Fluxo

| Construção | Mini-JS | Python |
|---|---|---|
| Bloco solto | `{ let g = 1; }` | `g = 1`, no mesmo nível |
| Comando vazio | `;` | nada |
| `if` / `else` | `if (a > 0) { ... } else { ... }` | `if a > 0:` / `else:` |
| `else if` | `else if (a < 0) { ... }` | `elif a < 0:` |
| `while` | `while (a < 5) { ... }` | `while a < 5:` |
| `for` | `for (let i = 0; i < 3; i = i + 1) { ... }` | `i = 0`, `while i < 3:` e, no fim do corpo, `i = i + 1` |
| `for` sem condição | `for (;;) { }` | `while True:` |

Exemplo:

```javascript
if (a > 0) { a = 1; } else if (a < 0) { a = 2; } else { a = 3; }
for (let i = 0; i < 3; i = i + 1) { console.log(i); }
```

```python
if a > 0:
    a = 1
elif a < 0:
    a = 2
else:
    a = 3
i = 0
while i < 3:
    _js_print(i)
    i = i + 1
```

### 2.4 Funções e Saída

| Construção | Mini-JS | Python |
|---|---|---|
| Declaração de função | `function soma(x, y) { ... }` | `def soma(x, y):` |
| `return` com valor | `return x + y;` | `return x + y` |
| `return` sem valor | `return;` | `return` |
| Chamada como comando | `nada();` | `nada()` |
| Chamada em expressão | `let r = soma(1, 2) * 2;` | `r = soma(1, 2) * 2` |
| Impressão | `console.log(a, "b");` | `_js_print(a, "b")` |

O `console.log` vira uma chamada a uma função auxiliar, escrita no início do arquivo gerado (veja a seção 3.4).

---

## 3. Decisões de Projeto

### 3.1 Geração Direta da AST

O Python é gerado diretamente da AST, sem código intermediário. Como Python é uma linguagem de alto nível, quase todas as construções do Mini-JS têm uma equivalente direta, e uma representação de nível mais baixo não traria ganho para a tradução. Além disso, a análise semântica já rejeitou os programas cuja tradução direta teria comportamento diferente (veja o [Escopo e Arquitetura](escopo-e-arq.md)). Por isso, o gerador também não precisa emitir `global` nem `nonlocal`: atribuir a uma variável global dentro de função e declarar uma função dentro de outra são erros semânticos.

### 3.2 Blocos Não Viram Nada

Um bloco `{ ... }` solto não gera nenhuma linha: seus comandos saem no mesmo nível de indentação de quem está em volta. Em Python, só funções criam escopo, e a análise semântica proíbe o sombreamento de nomes. Assim, nenhuma variável de um bloco interno pode sobrescrever uma variável externa por acidente ao "perder" o bloco.

```javascript
let a = 1;
{
    let b = 2;
    a = b;
}
```

```python
a = 1
b = 2
a = b
```

### 3.3 Parênteses por Precedência

A AST não guarda os parênteses do código original: a ordem de avaliação já está na forma da árvore. Ao escrever uma operação, o gerador compara a precedência do operador pai com a do filho e só coloca parênteses quando eles são necessários. Os níveis seguem a precedência do Python:

| Nível | Operadores |
|---|---|
| 1 (menor) | `or` |
| 2 | `and` |
| 3 | `not` |
| 4 | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| 5 | `+`, `-` |
| 6 | `*`, `/` |
| 7 | `-` unário |
| 8 (maior) | literais, nomes e chamadas |

Um operando recebe parênteses quando:

1. o nível do filho é **menor** que o do pai, como em `(a + b) * c`;
2. o filho está **à direita** e tem o **mesmo** nível do pai, como em `a - (b - c)`. Todos os operadores binários são associativos à esquerda, então `a - b - c` já significa `(a - b) - c` e não precisa de parênteses;
3. o pai e o filho são **comparações**, como em `(a < b) == c`.

A regra 3 e a posição do `not` no nível 3 corrigem duas diferenças entre as linguagens:

| Mini-JS | Significado no JS | Python sem parênteses | Significado no Python | Python gerado |
|---|---|---|---|---|
| `a < b == c` | `(a < b) == c` | `a < b == c` | `a < b and b == c` (comparação encadeada) | `(a < b) == c` |
| `!a == b` | `(!a) == b` | `not a == b` | `not (a == b)` | `(not a) == b` |

### 3.4 Função Auxiliar do `console.log`

O `print` do Python não imprime os valores do mesmo jeito que o `console.log`: ele escreve `True`, `None` e `2.0` onde o JavaScript escreve `true`, `undefined` e `2`. Por isso, `console.log(...)` vira uma chamada a `_js_print(...)`, e duas funções auxiliares são escritas no início do arquivo gerado:

```python
def _js_str(valor):
    if valor is None:
        return "undefined"
    if isinstance(valor, bool):
        return "true" if valor else "false"
    if isinstance(valor, float) and valor.is_integer() and abs(valor) < 1e21:
        return str(int(valor))
    return str(valor)


def _js_print(*valores):
    print(" ".join(_js_str(valor) for valor in valores))
```

| Valor | `print` | `_js_print` e `console.log` |
|---|---|---|
| `None` (variável sem valor ou função sem retorno) | `None` | `undefined` |
| `True` / `False` | `True` / `False` | `true` / `false` |
| `4 / 2` | `2.0` | `2` |
| `7 / 2` | `3.5` | `3.5` |
| `console.log("a", 1)` | `a 1` | `a 1` |

As funções só são escritas quando o programa usa `console.log` em algum lugar, inclusive dentro de funções, `if` e laços. Um programa sem impressão gera apenas o seu próprio código.

### 3.5 `for` Vira `while`

O `for` do Python percorre uma sequência (`for i in range(3)`), enquanto o `for` do JavaScript é um laço com inicialização, condição e incremento livres. A tradução que funciona para qualquer `for` é a forma equivalente com `while`: a inicialização sai antes do laço, a condição vira a condição do `while` e o incremento vai para o fim do corpo. Sem condição, o laço vira `while True:`.

Essa tradução é segura porque `break` e `continue` estão fora do escopo. Com `continue`, o incremento no fim do corpo seria pulado e o laço nunca terminaria.

### 3.6 `else if` Vira `elif`

Na AST, um `else if` é um `if` dentro do `else` do `if` anterior. Traduzido ao pé da letra, cada `else if` aumentaria um nível de indentação. O gerador reconhece o caso em que o `else` contém **apenas** outro `if` (ignorando blocos e comandos vazios em volta) e escreve `elif`, mantendo a cadeia no mesmo nível, como no código original. Se o `else` tiver outros comandos além do `if`, ele continua sendo um `else:` com o `if` dentro.

### 3.7 Nomes Reservados do Python

Alguns nomes válidos em JavaScript quebrariam o programa gerado. A análise semântica rejeita declarar variável, função ou parâmetro com esses nomes, com a mensagem `nome 'x' e reservado em Python`:

*   **Palavras reservadas do Python** que são identificadores comuns no Mini-JS: `False`, `None`, `True`, `and`, `as`, `assert`, `async`, `def`, `del`, `elif`, `except`, `global`, `is`, `lambda`, `nonlocal`, `not`, `or`, `pass` e `raise`. As que também são reservadas no JavaScript (`class`, `import` etc.) já são rejeitadas pelo léxico.
*   **Nomes usados pelo código gerado:** `_js_print` e `_js_str`, além de `print`, `abs`, `bool`, `float`, `int`, `isinstance` e `str`, que as funções auxiliares usam. Uma variável chamada `str`, por exemplo, esconderia a função do Python e quebraria o `_js_print`.

---

## 4. Limitações Conhecidas

Os casos abaixo passam pela análise semântica, mas o Python gerado se comporta de forma diferente do JavaScript. Tratá-los exigiria gerar código extra para imitar o JavaScript em cada operação, o que contraria o critério de tradução direta do projeto.

| Caso | Exemplo | JavaScript | Python |
|---|---|---|---|
| Divisão por zero | `1 / 0` | `Infinity` | `ZeroDivisionError` |
| Variável sem valor em uma conta | `let x; let y = x + 1;` | `NaN` | `TypeError` |
| Tipos que a semântica não conhece | `soma(1, 2) + "!"` | `"3!"` | `TypeError` |
| Inteiros acima de 2^53 | `console.log(fatorial(25))` | `1.5511210043330986e+25` | `15511210043330985984000000` |
| Números muito pequenos | `console.log(1 / 100000)` | `0.00001` | `1e-05` |
| Literal com mais de 15 dígitos | `0.1234567890123456` | `0.1234567890123456` | `0.123456789012346` |
| Zero negativo | `console.log(-0)` | `-0` | `0` |

*   **Tipos desconhecidos:** as regras de tipo só geram erro quando os dois tipos envolvidos são conhecidos (veja a [Análise Semântica](semantica.md)). O retorno de funções, os parâmetros e as variáveis reatribuídas com outro tipo ficam como desconhecidos, e uma combinação inválida entre eles só aparece ao executar o Python.
*   **Números:** o JavaScript guarda todo número como real de 64 bits, enquanto o Python tem inteiros de tamanho ilimitado e outra regra para escolher a notação científica. Os resultados só divergem em valores muito grandes ou muito pequenos. Os literais são escritos com até 15 dígitos significativos.

---

## 5. Exemplo Completo

Entrada (`tests/python/programa_fatorial.js`):

```javascript
// Fatorial recursivo de 0 a 10
function fatorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

for (let i = 0; i <= 10; i = i + 1) {
    console.log("fatorial de", i, "=", fatorial(i));
}
```

Python gerado com `./minijs --python tests/python/programa_fatorial.js`:

```python
def _js_str(valor):
    if valor is None:
        return "undefined"
    if isinstance(valor, bool):
        return "true" if valor else "false"
    if isinstance(valor, float) and valor.is_integer() and abs(valor) < 1e21:
        return str(int(valor))
    return str(valor)


def _js_print(*valores):
    print(" ".join(_js_str(valor) for valor in valores))


def fatorial(n):
    if n <= 1:
        return 1
    return n * fatorial(n - 1)
i = 0
while i <= 10:
    _js_print("fatorial de", i, "=", fatorial(i))
    i = i + 1
```

Saída, idêntica no Node e no Python:

```text
fatorial de 0 = 1
fatorial de 1 = 1
fatorial de 2 = 2
fatorial de 3 = 6
fatorial de 4 = 24
fatorial de 5 = 120
fatorial de 6 = 720
fatorial de 7 = 5040
fatorial de 8 = 40320
fatorial de 9 = 362880
fatorial de 10 = 3628800
```

---

## 6. Testes da Geração

A pasta `tests/python/` tem, para cada programa, três arquivos: o `.js` de entrada, o `.py.esperado` com o Python que deve ser gerado e o `.saida` com o que o programa deve imprimir. Os scripts `run_tests.sh` e `run_tests.ps1` fazem três checagens em cada programa:

1. a saída de `./minijs --python` é igual ao `.py.esperado`;
2. o arquivo gravado com `-o` é igual ao `.py.esperado`;
3. a execução do Python gerado imprime exatamente o `.saida`.

As saídas dos programas completos (`programa_*.js`) foram conferidas rodando o `.js` original no Node. Os testes unitários (`tests/unit_tests.c`) também cobrem a geração de cada construção isoladamente.
