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

* `dev`: Código-fonte em C (analisadores léxico, sintático e semântico, AST, tabela de símbolos, gerador de Python, testes e automação).
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
│   ├── semantico.c / semantico.h  # Análise semântica sobre a AST
│   └── gerador.c / gerador.h  # Geração do código Python a partir da AST
├── tests/
│   ├── validos/               # Programas que devem compilar sem erros
│   ├── invalidos/             # Programas com erro léxico ou sintático
│   ├── invalidos_semanticos/  # Programas com erro semântico
│   ├── ast/                   # Pares .js / .esperado para conferir a saída de --ast
│   ├── python/                # Trios .js / .py.esperado / .saida para conferir a geração de Python
│   ├── unit_tests.c           # Testes unitários do parser, da AST, da tabela, da semântica e da geração
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
./minijs [--ast] [--tabela] [--python] [-o saida.py] caminho/para/arquivo.js
```

No Windows, o executável é `minijs.exe`.

| Comando | O que faz |
|---|---|
| `./minijs arquivo.js` | Executa as análises léxica, sintática e semântica. Se o programa for válido, não imprime nada. |
| `./minijs --ast arquivo.js` | Também imprime a Árvore Sintática Abstrata (AST), com o tipo inferido de cada expressão, depois da análise semântica. |
| `./minijs --tabela arquivo.js` | Também imprime a tabela de símbolos, ao final da análise semântica. |
| `./minijs --ast --tabela arquivo.js` | Imprime a AST e depois a tabela de símbolos. |
| `./minijs --python arquivo.js` | Se o programa for válido, imprime o código Python gerado. |
| `./minijs arquivo.js -o saida.py` | Se o programa for válido, grava o código Python gerado no arquivo `saida.py`. |

* As opções podem vir antes ou depois do nome do arquivo.
* Sem um arquivo, o `minijs` lê o programa da entrada padrão (por exemplo, `echo "let a = 1;" | ./minijs --tabela`).
* A AST e a tabela são impressas na saída padrão (`stdout`). As mensagens de erro vão para a saída de erro (`stderr`).
* A AST é impressa sempre que não há erro léxico ou sintático, e a tabela sempre que a análise semântica é executada, mesmo que ela encontre erros. Isso ajuda a entender a causa de um erro semântico.
* Se houver um erro léxico ou sintático, a análise semântica não é executada, e nem a AST nem a tabela são impressas.
* O Python só é gerado quando não há nenhum erro léxico, sintático ou semântico. Se houver erro, nada é impresso com `--python` e nenhum arquivo é criado com `-o`.
* Uma opção desconhecida, ou `-o` sem o nome do arquivo, gera uma mensagem de erro, e o programa não é compilado.

### Código de Saída

| Código | Situação |
|---|---|
| `0` | Compilação sem erros |
| `1` | Erro léxico, sintático ou semântico, opção inválida, arquivo de entrada que não pôde ser aberto ou arquivo de saída que não pôde ser criado |

Quando há erros no programa, a última mensagem informa a fase e a quantidade de erros, por exemplo: `Compilacao falhou na fase semantica (1 erro(s))`. Erros léxicos e sintáticos são relatados juntos, como fase `lexica/sintatica`.

---

## Como Rodar os Testes

Os testes usam as mesmas ferramentas da compilação: `gcc`, `make`, `flex` e `bison`, disponíveis no `PATH`. O comando é o mesmo nos dois sistemas:

```bash
make test
```

O `make test` compila o `minijs` e o executável de testes unitários (`test_unit`) e depois chama o script de testes adequado ao sistema operacional.

Os testes de geração também executam o Python gerado, por isso é preciso ter o **Python 3** instalado e no `PATH` (`python3` ou `python` no Linux; `python3`, `python` ou `py` no Windows). Sem o Python, o script avisa e pula apenas a execução dos programas gerados.

### Linux

No Ubuntu/Debian, as ferramentas podem ser instaladas com:

```bash
sudo apt install gcc make flex bison python3
```

No Linux, o `make test` executa o script Bash `tests/run_tests.sh`. Depois de compilar com `make` e `make test_unit`, o script também pode ser executado diretamente:

```bash
bash tests/run_tests.sh
```

### Windows

No Windows, o `make test` executa o script PowerShell `tests/run_tests.ps1`, que procura os executáveis `minijs.exe` e `test_unit.exe`. É preciso ter `gcc`, `make`, `flex` e `bison` instalados e no `PATH` (por exemplo, via MSYS2 ou MinGW). Depois de compilar, o script também pode ser executado diretamente:

```powershell
powershell -ExecutionPolicy Bypass -File tests/run_tests.ps1
```

### O que é Testado

Os dois scripts executam as mesmas etapas:

| Etapa | O que verifica |
|---|---|
| Testes unitários (`tests/unit_tests.c`) | Parser, construção da AST, tabela de símbolos, regras semânticas e geração de Python, analisando trechos de código diretamente em memória |
| `tests/validos/` | Programas que devem compilar sem erros (código de saída `0`) |
| `tests/invalidos/` | Programas com erro léxico ou sintático, que devem falhar |
| `tests/invalidos_semanticos/` | Programas com erro semântico, que devem falhar com a mensagem `Erro semantico` (assim, um erro de sintaxe acidental no teste não passa despercebido) |
| `tests/ast/` | Compara a saída de `./minijs --ast` com o arquivo `.esperado` correspondente e confere que, sem a opção, nada é impresso |
| `tests/python/` | Compara a saída de `./minijs --python` e o arquivo gravado com `-o` com o `.py.esperado`, executa o Python gerado e compara o que ele imprime com o `.saida` |

Ao final, o script mostra o total de testes que passaram e falharam e retorna código de saída diferente de zero se algum teste falhar.

Para adicionar um caso de teste, basta criar um arquivo `.js` na pasta correspondente; para a AST, crie também o `.esperado` com a saída esperada, e para a geração de Python, o `.py.esperado` com o código esperado e o `.saida` com o que a execução deve imprimir. Para remover os arquivos gerados na compilação, use `make clean`.

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
│           └── NO_BINARIO * [numero]
│               ├── NO_ID n [desconhecido]
│               └── NO_NUMERO 2 [numero]
├── NO_DECL_VAR let total
│   └── NO_BINARIO * [numero]
│       ├── NO_BINARIO + [numero]
│       │   ├── NO_NUMERO 10 [numero]
│       │   └── NO_NUMERO 20 [numero]
│       └── NO_NUMERO 3 [numero]
└── NO_PRINT
    ├── NO_STRING "total:" [string]
    └── NO_CHAMADA dobro [desconhecido]
        └── NO_ID total [numero]
Nivel  Nome                 Categoria  Tipo          Linha  Parametros
0      dobro                funcao     desconhecido  1      1
1      n                    parametro  desconhecido  1      -
0      total                let        numero        5      -
```

A AST usa as conexões `├──`, `└──` e `│`, mostra o papel dos filhos de `if`, `while` e `for` (`condicao:`, `entao:`, `senao:`, `corpo:` etc.) omite os filhos ausentes e mostra, entre colchetes, o tipo inferido de cada expressão. Na tabela, cada símbolo aparece com o nível do escopo (`0` = global), a categoria, o tipo inferido, a linha da declaração e, para funções, a quantidade de parâmetros.

### Geração de Python

Com `--python`, o mesmo `exemplo.js` gera o código abaixo. As funções `_js_str` e `_js_print` aparecem no início porque o programa usa `console.log`: elas imprimem os valores como o JavaScript (`true`, `undefined`, `2` em vez de `2.0`).

```bash
./minijs --python exemplo.js
```

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


def dobro(n):
    return n * 2
total = (10 + 20) * 3
_js_print("total:", dobro(total))
```

Para gravar o código em um arquivo e executá-lo:

```bash
./minijs exemplo.js -o exemplo.py
python3 exemplo.py
```

```text
total: 180
```

A tradução de cada construção, as decisões de projeto e as limitações conhecidas estão na página de Geração de Python da [documentação do projeto](https://felipemso.github.io/G16-Compiladores/).

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
