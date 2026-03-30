MAIN = projetoTrab.c

FORNECIDAS = funcoes_fornecidas/fornecidas.c

FUNCIONALIDADES = funcionalidades/create_table.c

FLAGS = -Wall -Wextra -std=c99

# Diretórios para procurar arquivos de cabeçalho
INCLUDES = -I. -Ifuncionalidades -Ifuncoes_fornecidas

all:
	gcc $(MAIN) $(FORNECIDAS) $(FUNCIONALIDADES) -o programaTrab $(FLAGS) $(INCLUDES)

run:
	./programaTrab

clean:
	rm -f programaTrab *.o *.bin