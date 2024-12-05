# Variáveis
CC = gcc
BISON = bison
FLEX = flex
CFLAGS = -Wall -Wextra -lm
BISON_FLAGS = -d -Wcounterexamples
FLEX_FLAGS = 
EXEC = dcmat

# Arquivos fontes
LEXER_SRC = lexer.l
PARSER_SRC = parser.y
AST_SRC = ast.c
SYMBOL_SRC = symbol_table.c
FUNCTIONS_SCR = functions.c
MAIN_SRC = main.c
MATRIX_SRC = matrix.c
EVAL_SRC = eval.c

# Arquivos gerados
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h
LEXER_C = lex.yy.c
OBJS = $(PARSER_C:.c=.o) $(LEXER_C:.c=.o) $(SYMBOL_SRC:.c=.o) $(AST_SRC:.c=.o) $(EVAL_SRC:.c=.o) $(FUNCTIONS_SCR:.c=.o) $(MATRIX_SRC:.c=.o) $(MAIN_SRC:.c=.o)

# Regra principal
all: $(EXEC)

# Compilação do executável
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC) $(CFLAGS)

# Compilação do Bison
$(PARSER_C) $(PARSER_H): $(PARSER_SRC)
	$(BISON) $(BISON_FLAGS) $(PARSER_SRC)

# Compilação do Flex
$(LEXER_C): $(LEXER_SRC) $(PARSER_H)
	$(FLEX) $(FLEX_FLAGS) $(LEXER_SRC)

# Compilação dos arquivos .c em .o
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Limpeza
clean:
	rm -f $(EXEC) $(OBJS) $(PARSER_C) $(PARSER_H) $(LEXER_C)

# Limpeza completa (inclui arquivos gerados pelo Bison e Flex)
distclean: clean
	rm -f $(LEXER_C) $(PARSER_H)

# Rodar o programa
run: $(EXEC)
	./$(EXEC)

# valgrind: $(EXEC)
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./$(EXEC)