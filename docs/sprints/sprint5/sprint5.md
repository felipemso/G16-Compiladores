# Documento de Planejamento - Sprint 5

## Visão Geral da Sprint
**Objetivo Principal:** Planejar a arquitetura da Árvore Sintática Abstrata (AST), suas interfaces e sua integração com o Analisador Sintático, além de definir a organização da tabela de símbolos e das verificações semânticas.
**Período:** 18/10/2026 a 28/10/2026

## Resultado Esperado
Ao final da sprint, deverão estar definidos a representação da AST, suas interfaces e o fluxo de integração com o parser e a análise semântica. A declaração `let x = 1 + 2;`, por exemplo, deverá ter sua representação planejada como uma declaração cujo inicializador é uma operação de soma entre dois literais. A organização da tabela de símbolos deverá prever o registro de `x` e sua resolução no escopo correspondente.

## Escopo da Sprint
*   Modelagem das categorias, atributos, filhos e listas de nós da AST.
*   Planejamento das interfaces de construção, impressão e liberação e da responsabilidade pela memória.
*   Mapeamento das produções do Bison para a construção da árvore e disponibilização de sua raiz.
*   Planejamento do registro de símbolos, gerenciamento de escopos e verificações semânticas.
*   Divisão das tarefas de implementação e validação da etapa seguinte.

## Exclusões e Continuidade
*   A emissão de Python e os recursos retirados na revisão de escopo não fazem parte desta sprint.
*   A implementação da AST, da tabela de símbolos e das verificações semânticas pertence à Sprint 6; esta sprint concentrou-se no entendimento e na estruturação dessas atividades.

## Resultados da Sprint
A sprint foi dedicada ao entendimento dos conceitos e à estruturação do trabalho necessário para a AST e a tabela de símbolos. A equipe organizou as frentes e as tarefas para preparar a implementação na Sprint 6.

## Organização e Responsáveis
O trabalho foi dividido entre Felipe Melo de Sousa e Augusto Garcia Medeiros, na frente da AST, e os demais integrantes — Carlos Eduardo Deusdara Motta, Pedro Gonçalves Rocha e José Eduardo Vieira do Prado — na frente da tabela de símbolos.

## Problemas Encontrados e Soluções
A maior dificuldade foi conduzir o projeto de forma autônoma e aplicar, na prática, os conceitos envolvidos. A sprint concentrou-se no entendimento desses conceitos e na estruturação das atividades, com a divisão do trabalho entre as frentes de AST e tabela de símbolos para preparar a etapa de implementação.

---

## 1. Modelagem da Árvore Sintática Abstrata
Definir uma representação do programa que possa ser utilizada pela análise semântica e pela futura geração de Python.

*   **Categorias de Nós:** Planejar nós para programa, blocos, declarações, atribuições, condicionais, laços, funções, parâmetros, retornos, chamadas, impressão, expressões e literais.
*   **Atributos dos Nós:** Definir o armazenamento de categoria, linha de origem, operador, declarador, valor e tipo inferido, incluindo a possibilidade de tipo desconhecido.
*   **Relacionamento entre Nós:** Planejar a organização de filhos e o encadeamento de comandos, parâmetros e argumentos.
*   **Operadores e Tipos:** Definir representações para os operadores da linguagem de entrada e para números, strings e booleanos.

## 2. Interfaces e Gerenciamento de Memória
Planejar as operações necessárias para construir, inspecionar e liberar a árvore em C.

*   **Construção de Nós:** Definir interfaces de criação para cada categoria de nó.
*   **Manipulação de Listas:** Planejar a inclusão de elementos nas sequências de comandos, parâmetros e argumentos.
*   **Inspeção da AST:** Definir uma operação de impressão que permita verificar a estrutura produzida pelo parser.
*   **Liberação de Recursos:** Planejar a liberação dos nós e textos armazenados, estabelecendo a responsabilidade pela memória transferida do lexer para a árvore.

## 3. Integração com o Parser e Fluxo de Análise
Definir como as produções gramaticais passarão a produzir uma representação persistente do programa.

*   **Valores Semânticos do Bison:** Planejar a inclusão de referências a nós da AST na comunicação entre as produções.
*   **Ações Gramaticais:** Mapear as regras do parser para a criação e composição de nós, evoluindo as ações de reconhecimento para construção da árvore.
*   **Raiz do Programa:** Definir como a árvore completa será disponibilizada ao programa principal.
*   **Responsabilidades das Etapas:** Organizar o fluxo entre análise léxica, análise sintática com construção de AST, análise semântica e futura geração de Python.

## 4. Planejamento da Tabela de Símbolos e das Validações
Preparar a implementação das regras consolidadas na revisão de escopo.

*   **Registro de Símbolos:** Definir as informações necessárias para representar variáveis, constantes, parâmetros e funções.
*   **Operações de Escopo:** Planejar abertura, encerramento, inserção e consulta de símbolos em blocos, funções e laços `for`.
*   **Regras de Resolução:** Planejar as verificações de identificadores, redeclaração, sombreamento e restrições de atribuição a globais.
*   **Tipos e Funções:** Organizar as verificações de operações, comparações, ordem de declaração, recursão e quantidade de argumentos.
*   **Divisão das Tarefas:** Separar as atividades de estrutura da AST, integração com o parser, tabela de símbolos, validações semânticas e testes para a implementação seguinte.

---
