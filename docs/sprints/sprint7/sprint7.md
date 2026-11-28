# Documento de Planejamento - Sprint 7

## Visão Geral da Sprint
**Objetivo Principal:** Gerar o código Python a partir da AST validada, provar com testes de ponta a ponta que o programa gerado se comporta como o JavaScript original e concluir a documentação para a entrega final.
**Período:** 11/11/2026 a 27/11/2026

## Resultado Esperado
Ao final da sprint, o compilador deverá gerar um programa Python equivalente para todo programa aceito pelas análises léxica, sintática e semântica. Por exemplo, `let x = 1 + 2; console.log(x);` deverá gerar `x = 1 + 2` e `_js_print(x)`, e a execução do Python deverá imprimir `3`, exatamente como o Node. Um programa com erro não deverá gerar nenhum arquivo.

## Escopo da Sprint
*   Estrutura do gerador, controle de indentação e opções `--python` e `-o`.
*   Tradução de expressões, declarações, blocos, `if`/`else`, `while`, `for`, funções, chamadas e `console.log`.
*   Regra semântica que rejeita nomes válidos em JavaScript, mas reservados no Python.
*   Ajustes pendentes da sprint anterior na impressão da AST, nas opções de linha de comando e nos testes semânticos.
*   Testes automatizados da geração, comparando o Python gerado e a saída da sua execução.
*   Programas completos de ponta a ponta, conferidos com o Node.
*   Documentação da geração, registro da sprint, retrospectiva e README final.

## Exclusões e Continuidade
*   Os recursos retirados na revisão de escopo continuam sendo rejeitados pelo compilador, sem tradução.
*   Código intermediário e otimização não fazem parte do projeto: o Python é gerado diretamente da AST.
*   Esta é a última sprint do projeto. As diferenças de comportamento que não são detectadas pela análise semântica, como a divisão por zero, ficam registradas como limitações conhecidas na página [Geração de Python](../../tech-docs/geracao.md).

## Resultados da Sprint
Todas as tarefas da sprint (T28–T38) foram concluídas.

*   **Gerador de Python (T28–T32):** o módulo `src/gerador.c` traduz todas as construções do Mini-JS. A opção `--python` imprime o código gerado e a opção `-o` o grava em um arquivo.
*   **Semântica (T33 e T36):** nomes reservados do Python passaram a ser rejeitados, a opção `--ast` passou a mostrar o tipo inferido e opções desconhecidas passaram a gerar erro.
*   **Testes (T34 e T35):** a pasta `tests/python/` reúne 16 programas, cada um com o Python esperado e a saída esperada. Sete deles são programas completos, como fatorial, Fibonacci e MDC, com a saída conferida no Node. Ao final da sprint, o `make test` executa 292 testes unitários e 133 testes de arquivo.
*   **Documentação (T37 e T38):** nova página de geração de Python, atualização do Escopo e Arquitetura, registro desta sprint e README final.

## Organização e Responsáveis
O trabalho foi dividido em três frentes. Augusto Garcia Medeiros e Felipe Melo de Sousa ficaram com o gerador de Python; Carlos Eduardo Deusdara Motta e Pedro Gonçalves Rocha ficaram com as regras semânticas, os ajustes pendentes e os testes da geração. A documentação final (T37 e T38) foi feita por Augusto Garcia Medeiros.

---

## 1. Estrutura do Gerador
Criar a base sobre a qual todas as construções são traduzidas.

**Responsável:** Augusto (@augustogmedeiros)

*   **Módulo do Gerador:** Criar `src/gerador.h` e `src/gerador.c`, com a função `gera_python`, e incluí-los no `Makefile`.
*   **Indentação:** Controlar o nível de indentação (4 espaços por nível) e escrever `pass` quando um corpo não gerar nenhuma linha.
*   **Opções de Linha de Comando:** Adicionar `--python`, que imprime o código gerado, e `-o saida.py`, que o grava em um arquivo, com mensagem de erro para `-o` sem nome ou para arquivo que não pode ser criado.
*   **Geração Condicionada:** Gerar o Python somente quando não houver erro léxico, sintático ou semântico.

## 2. Tradução das Construções
Escrever o Python equivalente a cada tipo de nó da AST.

**Responsáveis:** Augusto (@augustogmedeiros) e Felipe (@felipemso)

*   **Expressões (T29):** Traduzir literais, operadores aritméticos, relacionais e lógicos (`&&`, `||` e `!` viram `and`, `or` e `not`), com parênteses colocados de acordo com a precedência do Python.
*   **Declarações, Blocos e Condicionais (T30):** Traduzir declarações e atribuições, blocos soltos e comandos vazios, `if`/`else` e cadeias de `else if` como `elif`.
*   **Laços (T31):** Traduzir `while` e converter o `for` em um `while` equivalente, com a inicialização antes do laço e o incremento no fim do corpo.
*   **Funções e Saída (T32):** Traduzir funções, `return` e chamadas, e converter o `console.log` em uma função auxiliar que imprime os valores como o JavaScript.

## 3. Regras Semânticas e Ajustes Pendentes
Rejeitar programas que gerariam um Python inválido e concluir os ajustes da sprint anterior.

**Responsáveis:** Carlos Eduardo (@cadumotta) e Pedro (@pedrogrocha13)

*   **Nomes Reservados do Python (T33):** Rejeitar variáveis, funções e parâmetros com nomes como `None`, `def`, `pass` e `lambda`, além dos nomes usados pelas funções auxiliares do código gerado.
*   **Tipo Inferido na AST (T36):** Mostrar o tipo inferido das expressões na opção `--ast`, imprimindo a árvore depois da análise semântica.
*   **Opções Desconhecidas (T36):** Rejeitar opções de linha de comando desconhecidas com mensagem de erro, em vez de tratá-las como nome de arquivo.
*   **Erros em Cascata (T36):** Evitar um segundo erro depois de uma operação aritmética inválida com string.

## 4. Testes da Geração
Garantir que o Python gerado está correto e que sua execução imprime o mesmo que o JavaScript.

**Responsáveis:** Pedro (@pedrogrocha13) e Carlos Eduardo (@cadumotta)

*   **Estrutura dos Testes (T34):** Criar a pasta `tests/python/`, com um `.js`, um `.py.esperado` e um `.saida` por programa, e integrá-la aos scripts `run_tests.sh` e `run_tests.ps1`.
*   **Três Checagens:** Comparar a saída de `--python` e o arquivo gravado com `-o` com o `.py.esperado`, e executar o Python gerado para comparar o que ele imprime com o `.saida`.
*   **Programas de Ponta a Ponta (T35):** Escrever programas completos que combinam várias construções e conferir a saída de cada um rodando o `.js` original no Node.

## 5. Documentação
Registrar o funcionamento final do compilador.

**Responsável:** Augusto (@augustogmedeiros)

*   **Geração de Python (T37):** Documentar como o gerador percorre a AST, a tradução de cada construção, as decisões de projeto, as limitações conhecidas e um exemplo completo, além de atualizar o Escopo e Arquitetura.
*   **Sprint e Retrospectiva (T38):** Registrar esta sprint, os problemas encontrados e a retrospectiva final do projeto.
*   **README (T38):** Documentar as opções `--python` e `-o`, um exemplo de saída em Python e o requisito do Python para os testes de geração.

## 6. Organização do Trabalho
A sprint manteve as regras da sprint anterior, com um cuidado extra para o arquivo compartilhado pela dupla da geração.

*   **Uma Branch por Issue:** Cada tarefa foi desenvolvida em sua própria branch, com Pull Request para `dev`; a documentação foi para a branch `docs`, exceto o README.
*   **Revisão por Outro Integrante:** Nenhum Pull Request foi mergeado pelo próprio autor; a orientação era que a revisão viesse, de preferência, de outra frente.
*   **Arquivo Compartilhado:** Como `src/gerador.c` foi alterado pelas tarefas T28–T32, a orientação era atualizar cada branch com o `dev` antes de abrir o Pull Request e manter os Pull Requests pequenos.
*   **Testes Junto com o Código:** Cada construção traduzida entrou com seus testes unitários e com casos em `tests/python/`.

## 7. Divisão de Tarefas

| Issue | Tarefa | Frente | Responsável |
|---|---|---|---|
| [T28](https://github.com/felipemso/G16-Compiladores/issues/53) | Estrutura do gerador e opções `--python` / `-o` | Geração | Augusto |
| [T29](https://github.com/felipemso/G16-Compiladores/issues/54) | Expressões, literais, declarações e atribuição | Geração | Augusto |
| [T30](https://github.com/felipemso/G16-Compiladores/issues/55) | Blocos, comando vazio e `if`/`else` | Geração | Felipe |
| [T31](https://github.com/felipemso/G16-Compiladores/issues/56) | `while` e `for` | Geração | Felipe |
| [T32](https://github.com/felipemso/G16-Compiladores/issues/57) | Funções, `return`, chamadas e `console.log` | Geração | Augusto |
| [T33](https://github.com/felipemso/G16-Compiladores/issues/58) | Regra semântica de nomes reservados do Python | Semântica | Carlos Eduardo |
| [T34](https://github.com/felipemso/G16-Compiladores/issues/59) | Testes de geração (`tests/python/` e scripts) | Testes | Pedro |
| [T35](https://github.com/felipemso/G16-Compiladores/issues/60) | Programas de ponta a ponta | Testes | Carlos Eduardo |
| [T36](https://github.com/felipemso/G16-Compiladores/issues/61) | Ajustes pendentes da sprint anterior | Semântica | Pedro |
| [T37](https://github.com/felipemso/G16-Compiladores/issues/62) | Documentação da geração de código | Documentação | Augusto |
| [T38](https://github.com/felipemso/G16-Compiladores/issues/63) | Sprint, retrospectiva e README final | Documentação | Augusto |
