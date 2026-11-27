# Documento de Planejamento - Sprint 2

## Visão Geral da Sprint
**Objetivo Principal:** Formalizar a especificação léxica da linguagem, concluir a implementação funcional do Analisador Léxico (Scanner) e estabelecer o início da gramática livre de contexto para a fase seguinte.
**Período:** 02/09/2026 a 16/09/2026

## Resultado Esperado
Ao final da sprint, o scanner deverá converter pequenos programas na linguagem de entrada em sequências de tokens conforme a especificação léxica planejada, descartar espaços e comentários e identificar entradas léxicas inválidas. Por exemplo, `let total = 10 + 2;` deverá produzir os tokens da declaração, do identificador, da atribuição, dos operandos, do operador e do ponto e vírgula. A gramática deverá ter um esboço para orientar a implementação do parser.

## Escopo da Sprint
*   Formalização das categorias de tokens, expressões regulares, autômatos e regras de desambiguação.
*   Implementação do scanner, localização das entradas e tratamento de erros léxicos.
*   Testes por categoria de token, entradas inválidas e programas de exemplo.
*   Esboço da gramática e preparação da integração com o analisador sintático.

## Exclusões e Continuidade
*   A validação completa da estrutura dos programas e a recuperação de erros sintáticos ficam para a Sprint 3.
*   A construção da AST, a resolução de identificadores, a análise de tipos e a geração de Python ficam para etapas posteriores.
*   O reconhecimento de um token não implica suporte à construção correspondente na gramática.
*   Os requisitos léxicos registrados nesta sprint representam o planejamento daquele período; os cortes posteriores de linguagem serão tratados na revisão de escopo da Sprint 4.

---

## Resultados da Sprint
A implementação do analisador léxico foi iniciada e concluída nesta sprint. O trabalho concentrou-se na definição das regras de reconhecimento dos tokens, com caráter predominantemente declarativo.

O esboço da gramática previsto no planejamento não foi iniciado nesta sprint. A implementação ficou restrita ao reconhecimento de tokens.

## Organização e Responsáveis
As atividades foram realizadas em conjunto pela equipe, com Augusto Garcia Medeiros à frente do trabalho no analisador léxico.

## Problemas Encontrados e Soluções
A principal dificuldade foi o primeiro contato com uma ferramenta pouco familiar à equipe e a compreensão da estrutura do analisador léxico. A familiarização com a organização das regras de reconhecimento foi o principal desafio de aprendizagem nesta etapa.

Para superar essa dificuldade, a equipe seguiu o guia da disciplina e utilizou as explicações em sala e os slides como apoio à compreensão da ferramenta e à implementação das regras.

## 1. Formalização da Especificação Léxica
Partindo da tabela de tokens e das decisões de escopo produzidas na Sprint 1, o objetivo é consolidar uma especificação formal e completa.

*   **Expressões Regulares:** Documentar formalmente as expressões regulares que definem cada categoria de token (identificadores, literais numéricos, literais de string, operadores compostos, comentários, etc.).
*   **Autômatos Finitos:** Produzir os diagramas de autômatos finitos determinísticos (AFD) correspondentes às expressões regulares, ilustrando as transições de estados do Scanner.
*   **Regras de Desambiguação:** Documentar as regras de precedência léxica (ex: correspondência mais longa, prioridade de palavras reservadas sobre identificadores).

## 2. Conclusão do Analisador Léxico (Scanner)
Evoluir a implementação inicial da Sprint 1 para um Scanner completo e robusto.

*   **Cobertura Completa de Tokens:** Implementar o reconhecimento de todos os tokens definidos na especificação (incluindo operadores compostos, literais de string com caracteres de escape, e comentários de linha/bloco).
*   **Tratamento de Erros Léxicos:** Implementar detecção e reporte de erros léxicos (caracteres inválidos, strings não terminadas, números mal formados), indicando linha e coluna.
*   **Controle de Posição:** Garantir que cada token gerado carregue informações de localização (linha, coluna) para uso nas fases posteriores de análise.
*   **Ignorar Elementos Não-Significativos:** Confirmar o tratamento correto de espaços em branco, tabulações, quebras de linha e comentários.

## 3. Testes do Scanner
Expandir a cobertura de testes além dos testes iniciais da Sprint 1.

*   **Testes Unitários por Categoria:** Criar testes específicos para cada categoria de token (palavras reservadas, identificadores, operadores, literais, símbolos de pontuação).
*   **Testes de Erros:** Validar que o Scanner reporta corretamente entradas inválidas e continua a análise após o erro (modo de recuperação).
*   **Testes com Arquivos Fonte:** Executar o Scanner sobre pequenos programas-exemplo na linguagem definida, verificando se a sequência de tokens gerada é a esperada.

## 4. Esboço da Gramática Livre de Contexto (GLC)
Preparação para a Sprint 3, que dará início ao Analisador Sintático.

*   **Definição das Produções:** Esboçar as regras de produção da gramática que descreve a sintaxe da linguagem (declarações, expressões, comandos de controle, blocos).
*   **Identificação de Ambiguidades:** Analisar as produções em busca de possíveis ambiguidades ou conflitos que dificultem a construção do Parser.
*   **Escolha da Técnica de Parsing:** Definir e documentar a abordagem de análise sintática a ser utilizada (ex: Recursive Descent, LL(1), ou uso de gerador de parser).

## 5. Problemas Encontrados e Soluções
Registro das dificuldades que surgiram durante a sprint e de como foram resolvidas.

### 5.1 Dependência do Léxico em Relação ao Parser
*   **Problema:** O analisador léxico (`lexer.l`) não compila sozinho: ele inclui o arquivo `parser.tab.h`, onde estão as definições dos tokens, e esse arquivo só existe depois que o Bison processa o `parser.y`. Sem o parser, não era possível compilar nem testar o léxico.
*   **Solução:** Criar um esqueleto do `parser.y` já nesta sprint, declarando todos os tokens com `%token`, e um `Makefile` que executa as etapas na ordem certa (`bison` → `flex` → `gcc`). Assim, o léxico passou a ser compilado junto com um parser mínimo, antes mesmo de a gramática ser escrita.

### 5.2 Léxico Incompleto ao Fim da Sprint
*   **Problema:** A primeira versão do léxico, entregue no fim da sprint (14/09), ainda não reconhecia algumas palavras reservadas do escopo (`var`, `function`, `return`), aceitava apenas números inteiros e não enviava ao parser o valor dos literais e identificadores.
*   **Solução:** O léxico foi completado no início da Sprint 3, com as palavras reservadas que faltavam e os números decimais. O envio dos valores ao parser foi resolvido pela integração com `%union` e `yylval` (tarefa T02 da Sprint 3).

### 5.3 Estrutura de Pastas
*   **Problema:** O léxico e o parser foram criados dentro de `src/` (`src/lexer/` e `src/parser/`), mas a organização do projeto mudou, e o `Makefile` dependia desses caminhos.
*   **Solução:** No início da Sprint 3, os arquivos foram movidos para as pastas `lexer/` e `parser/` na raiz do repositório, e o `Makefile` foi ajustado aos novos caminhos.

---
