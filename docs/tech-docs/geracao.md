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
