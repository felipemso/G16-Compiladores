# Compilador JavaScript Python

Projeto de Compilador proposto pelo professor Sérgio Freitas na disciplina de Compiladores I. Esse trabalho propõe a tradução de um subconjunto estruturado de JavaScript (Mini-JS) para Python. As informações detalhadas sobre arquitetura, escopo e demais considerações técnicas estão centralizadas na branch `docs`.


## Equipe (Grupo 16)

| Foto | Membro | Matrícula | GitHub |
| :---: | :--- | :---: | :--- |
| <img src="https://github.com/augustogmedeiros.png" width="40" height="40"> | **Augusto Garcia Medeiros** | 232000697 | [@augustogmedeiros](https://github.com/augustogmedeiros) |
| <img src="https://github.com/cadumotta.png" width="40" height="40"> | **Carlos Eduardo Deusdara Motta** | 241025194 | [@cadumotta](https://github.com/cadumotta) |
| <img src="https://github.com/felipemso.png" width="40" height="40"> | **Felipe Melo de Sousa** | 242015370 | [@felipemso](https://github.com/felipemso) |
| <img src="https://github.com/jevprado.png" width="40" height="40"> | **José Eduardo Vieira do Prado** | 221008202 | [@jevprado](https://github.com/jevprado) |
| <img src="https://github.com/pedrogrocha13.png" width="40" height="40"> | **Pedro Gonçalves Rocha** | 241025363 | [@pedrogrocha13](https://github.com/pedrogrocha13) |


## Estrutura do Repositório e Branches

* `dev`: Código-fonte em C (analisadores léxico, sintático e semântico, AST, tabela de símbolos, testes e automação).
* `docs`: Documentação completa do projeto em MkDocs.
* `main`: Branch principal com versões estáveis.
* `comp`: Guia organizacional da disciplina.
* `gh-pages`: Publicação do site de documentação.

### Organização de Arquivos (`dev`)

```text
G16-Compiladores/
├── lexer/
│   └── lexer.l                # Analisador léxico (Flex)
├── parser/
│   └── parser.y               # Analisador sintático (Bison) e construção da AST
├── src/
│   ├── main.c                 # Ponto de entrada: executa as fases e trata as opções
│   ├── ast.c / ast.h          # Nós da AST: construtores, impressão e liberação
│   ├── tabela.c / tabela.h    # Tabela de símbolos e pilha de escopos
│   └── semantico.c / semantico.h  # Análise semântica sobre a AST
├── tests/
│   ├── validos/               # Programas que devem compilar sem erros
│   ├── invalidos/             # Programas com erro léxico ou sintático
│   ├── invalidos_semanticos/  # Programas com erro semântico
│   ├── ast/                   # Pares .js / .esperado para conferir a saída de --ast
│   ├── unit_tests.c           # Testes unitários do parser, da AST, da tabela e da semântica
│   ├── run_tests.sh           # Executor dos testes no Linux (Bash)
│   └── run_tests.ps1          # Executor dos testes no Windows (PowerShell)
├── teste.js                   # Programa de exemplo
├── Makefile
└── README.md
```

Os arquivos gerados na compilação (`lex.yy.c`, `parser.tab.c`, `parser.tab.h` e os executáveis `minijs` e `test_unit`) ficam na raiz do projeto e não são versionados.

---

## Como Compilar

```bash
make
```

---

## Como Executar

```bash
./minijs [--ast] [--tabela] caminho/para/arquivo.js
```

No Windows, o executável é `minijs.exe`.

| Comando | O que faz |
|---|---|
| `./minijs arquivo.js` | Executa as análises léxica, sintática e semântica. Se o programa for válido, não imprime nada. |
| `./minijs --ast arquivo.js` | Também imprime a Árvore Sintática Abstrata (AST), depois da análise sintática. |
| `./minijs --tabela arquivo.js` | Também imprime a tabela de símbolos, ao final da análise semântica. |
| `./minijs --ast --tabela arquivo.js` | Imprime a AST e depois a tabela de símbolos. |

* As opções podem vir antes ou depois do nome do arquivo.
* Sem um arquivo, o `minijs` lê o programa da entrada padrão (por exemplo, `echo "let a = 1;" | ./minijs --tabela`).
* A AST e a tabela são impressas na saída padrão (`stdout`). As mensagens de erro vão para a saída de erro (`stderr`).
* A AST é impressa sempre que não há erro léxico ou sintático, e a tabela sempre que a análise semântica é executada, mesmo que ela encontre erros. Isso ajuda a entender a causa de um erro semântico.
* Se houver um erro léxico ou sintático, a análise semântica não é executada, e nem a AST nem a tabela são impressas.

### Código de Saída

| Código | Situação |
|---|---|
| `0` | Compilação sem erros |
| `1` | Erro léxico, sintático ou semântico, ou arquivo que não pôde ser aberto |

Quando há erros no programa, a última mensagem informa a fase e a quantidade de erros, por exemplo: `Compilacao falhou na fase semantica (1 erro(s))`. Erros léxicos e sintáticos são relatados juntos, como fase `lexica/sintatica`.

---

## Como Rodar os Testes

```bash
make test
```

---

## Exemplos de Saída

### Sucesso

**Entrada (`exemplo.js`):**
```javascript
function dobro(n) {
    return n * 2;
}

let total = (10 + 20) * 3;
console.log("total:", dobro(total));
```

Sem opções, um programa válido não produz saída, e o código de saída é `0`:

```bash
./minijs exemplo.js
```

Com `--ast --tabela`, o compilador imprime a AST e, em seguida, a tabela de símbolos:

```bash
./minijs --ast --tabela exemplo.js
```

```text
NO_PROGRAMA
├── NO_FUNCAO dobro
│   ├── NO_PARAMETRO n
│   └── NO_BLOCO
│       └── NO_RETURN
│           └── NO_BINARIO *
│               ├── NO_ID n
│               └── NO_NUMERO 2
├── NO_DECL_VAR let total
│   └── NO_BINARIO *
│       ├── NO_BINARIO +
│       │   ├── NO_NUMERO 10
│       │   └── NO_NUMERO 20
│       └── NO_NUMERO 3
└── NO_PRINT
    ├── NO_STRING "total:"
    └── NO_CHAMADA dobro
        └── NO_ID total
Nivel  Nome                 Categoria  Tipo          Linha  Parametros
0      dobro                funcao     desconhecido  1      1
1      n                    parametro  desconhecido  1      -
0      total                let        numero        5      -
```

A AST usa as conexões `├──`, `└──` e `│`, mostra o papel dos filhos de `if`, `while` e `for` (`condicao:`, `entao:`, `senao:`, `corpo:` etc.) e omite os filhos ausentes. Na tabela, cada símbolo aparece com o nível do escopo (`0` = global), a categoria, o tipo inferido, a linha da declaração e, para funções, a quantidade de parâmetros.

### Erro Sintático

**Entrada:**
```javascript
let a = 10;
let b = a +;
console.log(a)
let c = 3;
```

```text
Erro sintatico na linha 2: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Erro sintatico na linha 4: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Compilacao falhou na fase lexica/sintatica (2 erro(s))
```

O parser se recupera de cada erro descartando a entrada até o próximo `;` e continua a análise, por isso os dois erros são relatados na mesma execução. A falta de `;` na linha 3 é relatada na linha 4, onde o parser encontra o `let` inesperado.

Erros léxicos aparecem da mesma forma e também interrompem a compilação. Por exemplo, para `let preco = 10 @ 2;`:

```text
Erro lexico: Caractere invalido '@' na linha 1
Erro sintatico na linha 1: syntax error
[RECUPERACAO] Erro sintatico descartado ate ';'
Compilacao falhou na fase lexica/sintatica (1 erro(s))
```

### Erro Semântico

**Entrada:**
```javascript
const LIMITE = 100;
let nome = "Ana";

function soma(a, b) {
    return a + b;
}

LIMITE = 200;
let texto = nome + 1;
let r = soma(1);
console.log(idade);
```

```text
Erro semantico na linha 8: atribuicao a constante 'LIMITE' (declarada na linha 1)
Erro semantico na linha 9: operador '+' entre string e numero (string so pode ser concatenada com string)
Erro semantico na linha 10: funcao 'soma' espera 2 argumento(s), mas recebeu 1
Erro semantico na linha 11: identificador 'idade' nao declarado
Compilacao falhou na fase semantica (4 erro(s))
```

A análise semântica só é executada quando não há erros léxicos ou sintáticos. Ela também não para no primeiro erro: todos os erros do programa são relatados. A lista completa de regras semânticas está na [documentação do projeto](https://felipemso.github.io/G16-Compiladores/).
