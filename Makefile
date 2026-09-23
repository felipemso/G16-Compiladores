# Nome do executável final
TARGET = minijs

# Pastas do projeto
SRC_DIR = src
LEXER_DIR = lexer
PARSER_DIR = parser

# Arquivos C gerados automaticamente pelo Flex e Bison
LEX_C = lex.yy.c
YACC_C = parser.tab.c
YACC_H = parser.tab.h

# Regra principal: compila tudo
all: $(TARGET)

# Como gerar o executável final (junta o main.c com os arquivos gerados)
$(TARGET): $(YACC_C) $(LEX_C) $(SRC_DIR)/main.c
	gcc -o $(TARGET) $(YACC_C) $(LEX_C) $(SRC_DIR)/main.c

# Como gerar os arquivos do Bison
$(YACC_C): $(PARSER_DIR)/parser.y
	bison -d $(PARSER_DIR)/parser.y

# Como gerar o arquivo do Flex
$(LEX_C): $(LEXER_DIR)/lexer.l
	flex $(LEXER_DIR)/lexer.l

# Regra para limpar a os temporários gerados
clean:
	rm -f $(TARGET) $(LEX_C) $(YACC_C) $(YACC_H)