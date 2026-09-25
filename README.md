# Mini-JS Compiler

Compilador educacional projetado para analisar um subconjunto estático e estruturado da linguagem JavaScript (**Mini-JS**) e preparar sua representação sintática e semântica para tradução futura para Python.

---

## 🚀 Funcionalidades Implementadas

### 1. Infraestrutura de Execução e CLI
- **Execução direta de arquivos**: Lê e analisa arquivos fonte `.js` passados como argumento via linha de comando (`./minijs arquivo.js`).
- **Avaliação em memória**: Suporte à flag `-e` / `--eval` para analisar trechos de código diretamente via string (`./minijs -e "let x = 10;"`).
- **Modo interativo**: Leitura via `stdin` com finalização por `Ctrl+D` ou `Ctrl+Z`.
- **Suíte de Testes Automatizados**:
  - Testes de ponta a ponta para casos válidos (`tests/validos/`) e casos inválidos com falha esperada (`tests/invalidos/`).
  - Testes unitários em memória utilizando `yy_scan_string` (`tests/unit_tests.c`).
  - Script runner multiplataforma (`tests/run_tests.ps1`) com relatório visual integrado ao `make test`.

### 2. Analisador Léxico (`Flex`)
- **Palavras reservadas**: `let`, `const`, `var`, `function`, `if`, `else`, `while`, `for`, `return`, `console.log`, etc.
- **Valores literais e identificadores**:
  - Números inteiros e decimais (`NUM`)
  - Strings com aspas simples ou duplas (`STRING_VAL`)
  - Booleanos literais `true` e `false` (`BOOLEAN_VAL`)
  - Identificadores alfanuméricos válidos (`ID`)
- **Operadores**:
  - Aritméticos: `+`, `-`, `*`, `/`
  - Relacionais: `==`, `!=`, `<`, `<=`, `>`, `>=`
  - Lógicos: `&&`, `||`
  - Atribuição: `=`
- **Delimitadores e Comentários**:
  - Parênteses `( )`, chaves `{ }`, vírgula `,`, ponto e vírgula `;`
  - Comentários de linha única (`// ...`) e de bloco (`/* ... */`)

### 3. Integração Semântica (`%union` e `yylval`)
- Ponte de dados tipada entre o Flex e o Bison:
  - `valor_num` (`double`) para literais numéricos via `atof`
  - `texto` (`char *`) para identificadores e strings alocados dinamicamente via `strdup`
  - `booleano` (`int`) para valores booleanos (`1` ou `0`)
- Desalocação preventiva de memória (`free`) nas ações que consom strings temporariamente para evitar vazamentos (*memory leaks*).

### 4. Analisador Sintático LALR(1) (`Bison`)
- **Estrutura do Programa**:
  - Regra inicial `programa` derivada de `lista_comandos` flexível (aceita programas vazios ou compostos apenas por comentários).
  - Suporte a blocos aninhados delimitados por chaves `{ ... }`.
  - Suporte a instruções vazias avulsas com ponto e vírgula (`;`).
- **Gramática de Expressões e Precedência de Operadores**:
  - Tabela formal de precedência e associatividade (da menor para a maior):
    1. `%left OR` (`||`)
    2. `%left AND` (`&&`)
    3. `%left EQ NEQ` (`==`, `!=`)
    4. `%left LT LEQ GT GEQ` (`<`, `<=`, `>`, `>=`)
    5. `%left PLUS MINUS` (`+`, `-`)
    6. `%left TIMES DIVIDE` (`*`, `/`)
    7. `%right UMINUS` (menos unário)
  - Agrupamento prioritário por parênteses `(expressao)`.
  - Regra `atomo` para reconhecimento de literais e variáveis.
- **Comandos Reconhecidos**:
  - Declaração de variável com inicialização (`let ID = expressao;`).
  - Comando primitivo de impressão (`console.log(expressao);`).
- **Recuperação de Erros**:
  - Sincronização sintática após ponto e vírgula (`error SEMICOLON`) com `yyerrok`.

---

## 📁 Estrutura do Projeto

```text
cdev/
├── lexer/
│   └── lexer.l              # Especificação léxica do Flex
├── parser/
│   └── parser.y             # Gramática formal e precedência do Bison
├── src/
│   └── main.c               # Ponto de entrada do executável CLI
├── tests/
│   ├── run_tests.ps1        # Script runner de testes automatizados
│   ├── unit_tests.c         # Suíte de testes unitários em memória
│   ├── validos/             # Casos de teste que devem compilar com sucesso
│   └── invalidos/           # Casos de teste com erros sintáticos esperados
├── Makefile                 # Automação de compilação e testes
└── README.md                # Documentação técnica do projeto
```

---

## 🛠️ Como Compilar

### Pré-requisitos
* GCC (MinGW / MSYS2 / Linux)
* Flex (`flex`)
* Bison (`bison`)
* Make (`make`)

### Compilação via Makefile
```bash
make
```

### Compilação Manual
```bash
bison -d parser/parser.y
flex lexer/lexer.l
gcc -Wall -Wextra -o minijs parser.tab.c lex.yy.c src/main.c
gcc -Wall -Wextra -o test_unit parser.tab.c lex.yy.c tests/unit_tests.c
```

---

## 🧪 Como Executar os Testes

Para executar toda a suíte de testes (testes unitários em memória + casos de arquivos válidos e inválidos):

```bash
make test
```

Ou diretamente pelo PowerShell:
```powershell
powershell -ExecutionPolicy Bypass -File tests/run_tests.ps1
```

---

## ▶️ Modos de Uso do Compilador

### 1. Analisando um Arquivo Fonte
```bash
./minijs caminho/para/codigo.js
```

### 2. Avaliando Código Diretamente pela Linha de Comando (`-e`)
```bash
./minijs -e "let total = (10 + 20) * 3; console.log(total);"
```

### 3. Modo Interativo (`stdin`)
```bash
./minijs
```
Digite as instruções e encerre a entrada com `Ctrl+D` (Linux/MSYS) ou `Ctrl+Z` e `Enter` (Windows cmd).

---

## 📝 Exemplo de Código Mini-JS Válido

```javascript
// Operações aritméticas e precedência
let a = 10 + 20 * 3;
let b = (10 + 20) * 3;
let c = -a + 50 / 2;

// Expressões relacionais e lógicas
let cond = a > 10 && b <= 100 || c == 0;

// Blocos aninhados e saída
{
    let mensagem = "resultado: " + "sucesso";
    console.log(b);
}
```

Saída esperada:
```text
AST: Declaracao de variavel 'a' reconhecida.
AST: Declaracao de variavel 'b' reconhecida.
AST: Declaracao de variavel 'c' reconhecida.
AST: Declaracao de variavel 'cond' reconhecida.
AST: Declaracao de variavel 'mensagem' reconhecida.
AST: Comando de impressao reconhecido.

Compilacao bem-sucedida! Nao ha erros de sintaxe.
```
