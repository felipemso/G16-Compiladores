# Mini-JS Compiler

Compilador educacional para um subconjunto estruturado de JavaScript (**Mini-JS**). O projeto está focado no desenvolvimento dos analisadores léxico e sintático (Flex e Bison), com suporte a declarações de variáveis, blocos `{ ... }`, gramática completa de expressões com precedência de operadores e suíte de testes automatizados.

> ℹ️ **Documentação Completa:** As informações detalhadas sobre a arquitetura do compilador, decisões de projeto e especificações de escopo estão centralizadas na branch **`docs`**.

---

## 🛠️ Como Compilar

Certifique-se de ter o `gcc`, `flex`, `bison` e `make` instalados no ambiente. No terminal, execute:

```bash
make
```

*(Alternativa sem Make: `bison -d parser/parser.y && flex lexer/lexer.l && gcc -o minijs parser.tab.c lex.yy.c src/main.c`)*

---

## ▶️ Como Executar

### 1. Analisando um arquivo `.js`:
```bash
./minijs caminho/para/arquivo.js
```

### 2. Avaliando código via linha de comando (`-e`):
```bash
./minijs -e "let x = (10 + 20) * 3; console.log(x);"
```

### 3. Modo interativo (`stdin`):
```bash
./minijs
```
*(Digite as instruções e finalize com `Ctrl+D` no Linux/MSYS ou `Ctrl+Z` + `Enter` no Windows cmd)*

---

## 🧪 Como Rodar os Testes

Para executar toda a suíte de testes automatizados (testes em memória e validação de arquivos válidos e inválidos):

```bash
make test
```

---

## 📋 O que esperar de saída

Para um código sintaticamente válido, o compilador reconhece as estruturas e finaliza com código de saída `0`:

**Entrada (`teste.js`):**
```javascript
let total = (10 + 20) * 3;
console.log(total);
```

**Saída esperada:**
```text
AST: Declaracao de variavel 'total' reconhecida.
AST: Comando de impressao reconhecido.

Compilacao bem-sucedida! Nao ha erros de sintaxe.
```

Caso o código contenha erros de sintaxe, o compilador reporta as falhas no `stderr` e retorna código de saída `1`.
