---
hide:
  - navigation
  - toc
---
# Sobre o Projeto

Este projeto tem como foco o desenvolvimento de um compilador para traduzir código-fonte de um subconjunto estruturado e imperativo de **JavaScript (Mini-JS)** diretamente para **Python**. O desenvolvimento é conduzido de forma incremental ao longo de várias sprints, consolidando as principais etapas clássicas da engenharia de compiladores.

## Fluxo de Compilação

O fluxo de compilação especificado para a arquitetura do projeto é composto pelas seguintes tecnologias e etapas:

![Fluxo de Compilação](assets/FluxoCompilador.png)

1. **Analisador Léxico:** Desenvolvido em C utilizando o **Flex**. É responsável por fazer a varredura do código-fonte em JavaScript, descartando espaços em branco e comentários e convertendo o texto em uma sequência de tokens válidos.
2. **Analisador Sintático:** Implementado em C utilizando o **Bison**. Esta etapa recebe os tokens gerados no processo léxico, valida a estrutura das instruções contra a gramática livre de contexto da linguagem e constrói a Árvore Sintática Abstrata (AST).
3. **Gerador de Código Final:** Responsável por percorrer a representação intermediária estruturada na AST e emitir o código correspondente em **Python (`.py`)**, devidamente identado e pronto para execução.
