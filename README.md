# Compilador JavaScript Python

Projeto de Compilador proposto pelo professor Sérgio Freitas na disciplina de Compiladores I. Esse trabalho propõe a tradução de um subconjunto estruturado de JavaScript (Mini-JS) para Python. As informações detalhadas sobre arquitetura, escopo e demais considerações técnicas estão centralizadas na branch `docs`.


## Equipe (Grupo 16)

| Foto | Membro | Matrícula | GitHub |
| :---: | :--- | :---: | :--- |
| <img src="https://github.com/augustogmedeiros.png" width="40" height="40"> | **Augusto Garcia Medeiros** | 232000697 | [@augustogmedeiros](https://github.com/augustogmedeiros) |
| <img src="https://github.com/cadumotta.png" width="40" height="40"> | **Carlos Eduardo Deusdara Motta** | 241025194 | [@cadumotta](https://github.com/cadumotta) |
| <img src="https://github.com/felipemso.png" width="40" height="40"> | **Felipe Melo de Sousa** | 242015370 | [@felipemso](https://github.com/felipemso) |
| <img src="https://github.com/jevprado.png" width="40" height="40"> | **José Eduardo Vieira do Prado** | 221008202 | [@jevprado](https://github.com/jevprado) |
| <img src="https://github.com/pedrogrocha13.png" width="40" height="40"> | **Pedro Gonçalves Rocha** | 241025363 | [@pedrogrocha13](https://github.com/pedrogrocha13) |


## Estrutura do Repositório e Branches

* `dev`: Código-fonte em C (analisadores léxico e sintático, testes e automação).
* `docs`: Documentação completa do projeto em MkDocs.
* `main`: Branch principal com versões estáveis.
* `comp`: Guia organizacional da disciplina.
* `gh-pages`: Publicação do site de documentação.

### Organização de Arquivos (`dev`)

```text
cdev/
├── lexer/
│   └── lexer.l
├── parser/
│   └── parser.y
├── src/
│   └── main.c
├── tests/
│   ├── run_tests.ps1
│   ├── unit_tests.c
│   ├── validos/
│   └── invalidos/
├── Makefile
└── README.md
```

---

## Como Compilar

```bash
make
```

---

## Como Executar

Analisando um arquivo:
```bash
./minijs caminho/para/arquivo.js
```

Avaliando código diretamente:
```bash
./minijs -e "let x = (10 + 20) * 3; console.log(x);"
```

---

## Como Rodar os Testes

```bash
make test
```

---

## Saída Esperada

**Entrada (`teste.js`):**
```javascript
let total = (10 + 20) * 3;
console.log(total);
```

**Saída:**
```text
AST: Declaracao de variavel 'total' reconhecida.
AST: Comando de impressao reconhecido.

Compilacao bem-sucedida! Nao ha erros de sintaxe.
```
