# Documento de Arquitetura e Escopo 

## 1. Visão Geral
O projeto consiste no desenvolvimento de um compilador para traduzir código-fonte de JavaScript (JS) para Python (Py). 

## 2. Escopo do MVP 
Com base nas definições atuais, o escopo inicial focará apenas nas construções básicas da linguagem JavaScript. 

**Recursos Suportados (JavaScript):**
*   Declaração de variáveis (`let`, `const`, `var`).
*   Estruturas de controle de fluxo (`if`, `else`, `switch`, `case`).
*   Estruturas de repetição (`for`, `while`, `do`).
*   Declaração de funções simples e retorno (`function`, `return`).
*   Tipos básicos e operações matemáticas/lógicas.

---

## 3. Arquitetura: Analisador Léxico (Fase Atual)

No momento, o foco arquitetural está restrito à primeira etapa do compilador: o Analisador Léxico.

### 3.1 Tecnologias e Ferramentas
*   **Linguagem de Implementação:** C
*   **Gerador de Analisador Léxico:** Flex 
*   **Arquivo correspondente:** `lexer.l`

### 3.2 Estrutura e Identificação de Tokens
O lexer será responsável por varrer o código-fonte caractere por caractere e convertê-lo em uma sequência de tokens lógicos. As seguintes categorias de tokens serão implementadas:

1.  **Palavras-Chave (Keywords):**
    Identificadores reservados pela linguagem (ex: `let`, `if`, `function`, `return`). 
2.  **Identificadores (Identifiers):**
    Nomes definidos pelo usuário para variáveis, funções, etc. 
3.  **Literais (Literals):**
    Valores numéricos, strings (aspas simples e duplas) e booleanos.
4.  **Operadores (Operators):**
    Símbolos matemáticos (`+`, `-`, etc.), lógicos (`&&`, `||`) e de comparação (`==`, `!=`, `<`, etc.).
5.  **Pontuação (Punctuators):**
    Chaves `{}`, parênteses `()`, colchetes `[]`, ponto e vírgula `;`.

### 3.3 Tratamento de Espaços e Comentários
*   **Espaços em Branco:** Espaços, tabulações e quebras de linha (`\n`, `\t`, ` `) serão consumidos e ignorados pelo lexer.
*   **Comentários:** Comentários de linha (`//`) e de bloco (`/* */`) também serão ignorados, não gerando tokens para o Parser.