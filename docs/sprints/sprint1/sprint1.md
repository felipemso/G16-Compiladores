# Documento de Planejamento - Sprint 1

## Visão Geral da Sprint
**Objetivo Principal:** Configurar o ambiente de desenvolvimento, definir o escopo da linguagem a ser compilada, produzir a documentação inicial e dar início à implementação do Analisador Léxico.
**Período:** 19/08/2026 a 02/09/2026

## Resultado Esperado
Ao final da sprint, a equipe deverá dispor de uma base organizada para desenvolver o compilador de JavaScript para Python: ambiente de desenvolvimento definido, repositório estruturado, documentação inicial e início do reconhecimento léxico. Uma entrada simples como `let x = 10;` deverá orientar os primeiros casos de reconhecimento de palavras-chave, identificadores, números e delimitadores.

## Escopo da Sprint
*   Configuração do ambiente, organização do repositório e definição do fluxo de trabalho.
*   Definição inicial das linguagens de entrada e saída e da arquitetura do compilador.
*   Documentação de instalação, execução, escopo e categorias de tokens.
*   Estrutura inicial do lexer e primeiros testes de reconhecimento.

## Exclusões e Continuidade
*   A conclusão do scanner e de sua cobertura de testes fica para a Sprint 2.
*   A implementação completa do parser, da AST, da análise semântica e da geração de Python pertence às etapas seguintes.
*   A definição inicial do escopo poderá ser refinada nas próximas sprints; não representa garantia de suporte já implementado.

## Resultados da Sprint
A sprint teve como foco a ambientação da equipe na disciplina e a preparação do projeto. Foram concluídas a organização dos ambientes de desenvolvimento dos membros, o levantamento de requisitos, a definição de um escopo inicial e a criação dos documentos iniciais e da estrutura de documentação com MkDocs.

O início da implementação e dos testes do analisador léxico, previsto no planejamento abaixo, foi transferido para a Sprint 2. Não houve implementação do lexer nesta sprint.

## Organização e Responsáveis
As atividades foram realizadas em conjunto pela equipe, sem divisão individual de responsabilidades por frente nesta sprint.

## Problemas Encontrados e Soluções
A principal dificuldade foi compreender a dinâmica da disciplina, identificar o que deveria ser desenvolvido e conhecer as ferramentas propostas. A equipe iniciou o acompanhamento do guia da disciplina como orientação para organizar o trabalho e compreender as etapas do projeto.

---

## 1. Ambientação e Infraestrutura
Nesta etapa, o objetivo é garantir que todos os membros da equipe tenham as ferramentas necessárias e um ambiente padronizado.

*   **Definição do Stack Tecnológico:** Escolher a linguagem de programação base para o compilador e ferramentas auxiliares.
*   **Controle de Versão:** Criar o repositório no GitHub e definir a estratégia de ramificação.
*   **Configuração de Ambiente:** Configurar IDEs e frameworks de teste.

## 2. Levantamento de Escopo e Requisitos
Esta etapa define *o que* o compilador será capaz de processar.

*   **Especificação da Linguagem Fonte:** Definir claramente a linguagem que o compilador irá ler.
*   **Definição da Linguagem Alvo:** Estabelecer qual será a saída do compilador.
*   **Arquitetura Geral:** Desenhar e estudar a arquitetura em alto nível do compilador (Lexer -> Analisador Sintático -> Analisador semântico -> Gerador de código).

## 3. Produção de Documentação
Documentação essencial para o início do projeto.

*   **Arquivo `README.md`:** Criar o documento principal com instruções de instalação, execução e visão geral do projeto.
*   **Tabela de Tokens:** Documentar todas as expressões regulares e regras léxicas da linguagem fonte (Palavras reservadas, Operadores, Identificadores, Literais numéricos e strings, Símbolos de pontuação).
*   **Documento de Especificação Inicial:** Markdown detalhando as decisões tomadas na fase de escopo.

## 4. Início da Estruturação do Analisador Léxico (Lexer)
A primeira fase prática do compilador. O objetivo é estabelecer a fundação do lexer.

*   **Estrutura de Pastas:** Criar a organização básica de diretórios.
*   **Definição da Estrutura de Token:** 
*   **Implementação Inicial:** Começar a leitura do arquivo fonte e implementar o reconhecimento dos primeiros tokens (ex: ignorar espaços em branco, ler números e algumas palavras-chave).
*   **Testes Iniciais:** Escrever os primeiros testes unitários garantindo que os tokens implementados estão sendo reconhecidos corretamente.

---
