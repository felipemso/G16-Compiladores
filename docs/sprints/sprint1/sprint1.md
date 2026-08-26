# Documento de Planejamento - Sprint 1

## Visão Geral da Sprint
**Objetivo Principal:** Configurar o ambiente de desenvolvimento, definir o escopo da linguagem a ser compilada, produzir a documentação inicial e dar início à implementação do Analisador Léxico.
**Período:** 19/08/2026 a 02/09/2026

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
