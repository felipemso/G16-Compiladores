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

Analisando um arquivo:
```bash
./minijs caminho/para/arquivo.js
```

Para imprimir a AST:
```bash
./minijs --ast caminho/para/arquivo.js
```

Sem `--ast`, arquivos validos nao produzem saida. Erros sao enviados para stderr.

---

## Como Rodar os Testes

```bash
make test
```

---

## Saída Esperada

**Entrada (`teste.js`):**
```javascript
let total = (10 + 20) * 3;
console.log(total);
```

**Saída com `--ast`:**
```text
NO_PROGRAMA
├── NO_DECL_VAR let total
│   └── NO_BINARIO *
│       ├── NO_BINARIO +
│       │   ├── NO_NUMERO 10
│       │   └── NO_NUMERO 20
│       └── NO_NUMERO 3
└── NO_PRINT
    └── NO_ID total
```

A impressao usa conexoes `├──`, `└──` e `│`, identifica os papeis dos filhos
de `if`, `while` e `for`, omite filhos nulos e preserva a ordem
dos filhos e das listas. Os pares `tests/ast/*.js` e `*.esperado` verificam esse
formato nos runners Bash e PowerShell, alem da execucao silenciosa sem a flag.
