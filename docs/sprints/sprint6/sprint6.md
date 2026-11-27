# Documento de Planejamento - Sprint 6

## Visão Geral da Sprint
**Objetivo Principal:** Implementar a Árvore Sintática Abstrata (AST) e a tabela de símbolos, integrando-as ao compilador para representar o programa e validar as regras semânticas da linguagem de entrada, com os ajustes necessários nos analisadores léxico e sintático.
**Período:** 28/10/2026 a 11/11/2026

## Resultado Esperado
Ao final da sprint, o compilador deverá construir a AST das construções mantidas no escopo do projeto, registrar e resolver símbolos e executar as validações semânticas previstas. Por exemplo, `let x = 1; console.log(x);` deverá produzir uma árvore e resolver o uso de `x` à sua declaração; um uso sem declaração acessível deverá ser rejeitado. Chamadas com quantidade incorreta de argumentos e operações proibidas entre tipos conhecidos também deverão gerar diagnósticos.

## Escopo da Sprint
*   Implementação da estrutura da AST, funções de construção, listas, impressão, liberação e integração ao build.
*   Adaptação do parser para construir e disponibilizar a árvore completa do programa.
*   Implementação da tabela de símbolos e gerenciamento dos escopos de blocos, funções e laços.
*   Validação de identificadores, redeclaração, sombreamento e restrições de atribuição a globais.
*   Verificações de operações, comparações, declaração de funções, recursão e quantidade de argumentos.
*   Adequações do lexer e parser ao escopo revisado, incluindo `!`, strings, palavras reservadas e propagação de erros.
*   Testes integrados da AST, da tabela de símbolos e das regras semânticas.

## Exclusões e Continuidade
*   A geração de arquivos Python permanece obrigatória, mas fica para a próxima etapa.
*   Arrays, funções aninhadas, atribuição a globais dentro de funções e sombreamento não serão suportados, respeitada a exceção de parâmetros com nome de global.
*   Incremento `++`, atribuições compostas, `%`, escape de aspas e notação científica continuam fora do escopo.
*   Código intermediário e otimização não fazem parte do projeto.
*   As validações seguem as regras do subconjunto de JavaScript definido pelo projeto, incluindo as restrições de coerção e de ordem de declaração de funções.

## Resultados da Sprint
Considerando concluídas as tarefas T11–T27, conforme informado pela equipe, o trabalho abrangeu a implementação da AST, sua integração com o parser, a tabela de símbolos, as verificações semânticas, os testes e a documentação dessas etapas.

*   **AST e integração com o parser (T11–T16):** Implementação dos construtores, listas, liberação e impressão da árvore; construção de nós para expressões, declarações, blocos, controle de fluxo, funções, chamadas, retornos e impressão; preservação das linhas de origem e testes unitários e de saída. A opção `--ast` permite inspecionar a árvore.
*   **Tabela de símbolos e análise semântica (T17–T22):** Implementação do cadastro e da consulta de símbolos, gerenciamento de escopos e percurso da AST; validações de declarações, atribuições, funções e tipos; integração ao programa principal, diagnósticos e testes automatizados. A opção `--tabela` permite inspecionar a tabela de símbolos.
*   **Documentação (T23–T27):** Atualização do escopo, da arquitetura e do README, documentação da gramática e dos tokens, registro das sprints e descrição da AST, da tabela de símbolos e das regras semânticas.

Este balanço utiliza as descrições e os critérios de aceitação das issues locais como referência e a conclusão informada pela equipe. Não representa uma verificação técnica das implementações nem uma confirmação das datas de conclusão de cada tarefa; as cópias locais das issues mantêm o estado da consulta original.

## Organização e Responsáveis
A implementação foi dividida em duas frentes: Felipe Melo de Sousa e Augusto Garcia Medeiros ficaram responsáveis pela AST; Carlos Eduardo Deusdara Motta, Pedro Gonçalves Rocha e José Eduardo Vieira do Prado ficaram na frente da tabela de símbolos. As issues detalham as tarefas de análise semântica associadas à tabela e também atribuem a José Eduardo as atividades de documentação (T23–T27).

---

## 1. Implementação da Árvore Sintática Abstrata
Materializar a representação do programa planejada na sprint anterior.

*   **Estrutura dos Nós:** Implementar enumerações de categorias, tipos, operadores e declaradores, além da estrutura de nó com linha, valor, tipo inferido, filhos e encadeamento.
*   **Criação e Composição:** Implementar funções de construção de nós para expressões, declarações, blocos, comandos, funções, parâmetros e argumentos.
*   **Listas de Nós:** Implementar a composição de sequências de comandos, parâmetros e argumentos.
*   **Impressão e Liberação:** Implementar operações de inspeção da árvore e liberação dos recursos alocados.
*   **Integração com o Build:** Incluir os arquivos da AST no processo de compilação pelo Makefile.

## 2. Integração da AST com o Analisador Sintático
Fazer o parser construir a árvore completa do programa durante o reconhecimento das produções.

*   **Valores Semânticos:** Adaptar `%union` e os tipos das produções do Bison para transportar referências aos nós da AST.
*   **Ações das Produções:** Construir nós e relacionar seus filhos nas regras de expressões, declarações, controles de fluxo e funções.
*   **Raiz da Árvore:** Disponibilizar a AST do programa para a etapa de análise semântica.
*   **Preservação dos Dados:** Transferir identificadores, literais e informações de linha para a árvore, adequando o gerenciamento de memória.

## 3. Implementação da Tabela de Símbolos e dos Escopos
Registrar declarações e resolver os identificadores utilizados no programa.

*   **Cadastro e Consulta:** Implementar operações de inserção e busca de variáveis, constantes, parâmetros e funções, com os atributos necessários às validações.
*   **Ciclo de Vida dos Escopos:** Abrir e encerrar escopos de blocos, funções e laços `for`, permitindo reutilização de nomes em laços consecutivos independentes.
*   **Redeclaração e Sombreamento:** Aplicar a proibição de redeclaração e sombreamento, respeitando a exceção de parâmetros com nome de variável global.
*   **Regras de Variáveis:** Aplicar escopo de bloco a `var`, da mesma forma que `let`, e rejeitar atribuição a variáveis globais dentro de funções.
*   **Resolução de Identificadores:** Verificar se os nomes utilizados possuem declaração acessível no ponto de uso.

## 4. Validações Semânticas de Tipos e Funções
Percorrer a AST utilizando a tabela de símbolos para verificar as regras da linguagem.

*   **Operações entre Tipos:** Rejeitar soma de string com número ou booleano e uso de strings nos operadores `-`, `*` e `/`.
*   **Comparações:** Rejeitar comparações entre tipos conhecidos diferentes, preservando a distinção entre tipos conhecidos e desconhecidos.
*   **Declarações e Chamadas:** Exigir declaração de função antes do uso, permitindo chamadas recursivas.
*   **Parâmetros e Argumentos:** Verificar se a quantidade de argumentos corresponde à quantidade de parâmetros declarados.
*   **Restrições Estruturais:** Rejeitar funções aninhadas e declarações diretamente em corpos de condicionais e laços sem chaves, conforme o escopo revisado.

## 5. Adequações dos Analisadores e Validação Integrada
Consolidar o lexer e o parser para suportar a nova etapa do compilador e validar o comportamento integrado.

*   **Negação Lógica:** Implementar o reconhecimento de `!`, sua regra gramatical, precedência e representação na AST.
*   **Ajustes da Gramática:** Corrigir conflitos e inconsistências nas regras de argumentos e de `console.log`.
*   **Escopo Léxico:** Revisar tokens e rejeitar claramente palavras reservadas de construções não suportadas.
*   **Strings e Erros Léxicos:** Rejeitar quebras de linha em strings e garantir que erros léxicos informem a linha e façam a compilação falhar.
*   **Testes da AST e da Tabela:** Validar estrutura da árvore, cadastro e resolução de símbolos, abertura e encerramento de escopos e liberação de recursos.
*   **Testes Semânticos:** Cobrir programas válidos e violações das regras de variáveis, tipos e funções, com diagnósticos que indiquem a localização do erro.

---
