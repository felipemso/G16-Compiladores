# Documento de Planejamento - Sprint 2

## Visão Geral da Sprint
**Objetivo Principal:** Formalizar a especificação léxica da linguagem, concluir a implementação funcional do Analisador Léxico (Scanner) e estabelecer o início da gramática livre de contexto para a fase seguinte.
**Período:** 02/09/2026 a 16/09/2026

---

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

---
