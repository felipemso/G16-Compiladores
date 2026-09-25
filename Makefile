# Nome dos executaveis
TARGET = minijs
TEST_UNIT = test_unit

# Pastas do projeto
SRC_DIR = src
LEXER_DIR = lexer
PARSER_DIR = parser
TESTS_DIR = tests

# Arquivos C gerados automaticamente pelo Flex e Bison
LEX_C = lex.yy.c
YACC_C = parser.tab.c
YACC_H = parser.tab.h

# Regra principal: compila tudo
all: $(TARGET)

# Como gerar o executavel final (junta o main.c com os arquivos gerados)
$(TARGET): $(YACC_C) $(LEX_C) $(SRC_DIR)/main.c
	gcc -o $(TARGET) $(YACC_C) $(LEX_C) $(SRC_DIR)/main.c

# Como gerar o executavel de testes unitarios em memoria (diferencial)
$(TEST_UNIT): $(YACC_C) $(LEX_C) $(TESTS_DIR)/unit_tests.c
	gcc -o $(TEST_UNIT) $(YACC_C) $(LEX_C) $(TESTS_DIR)/unit_tests.c

# Como gerar os arquivos do Bison
$(YACC_C): $(PARSER_DIR)/parser.y
	bison -d $(PARSER_DIR)/parser.y

# Como gerar o arquivo do Flex
$(LEX_C): $(LEXER_DIR)/lexer.l
	flex $(LEXER_DIR)/lexer.l

# Regra de execucao dos testes automatizados
test: $(TARGET) $(TEST_UNIT)
ifeq ($(OS),Windows_NT)
	powershell -ExecutionPolicy Bypass -File tests/run_tests.ps1
else
	bash tests/run_tests.sh
endif

# Regra para limpar os temporarios gerados
clean:
	rm -f $(TARGET) $(TEST_UNIT) $(LEX_C) $(YACC_C) $(YACC_H) $(TARGET).exe $(TEST_UNIT).exe