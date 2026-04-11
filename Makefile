MAIN = projetoTrab.c

FORNECIDAS = funcoes_fornecidas/fornecidas.c

LOCAL1 = funcionalidades

LOCAL2 = funcionalidades_aux

FUNCIONALIDADES = $(LOCAL1)/create_table.c $(LOCAL1)/delete.c $(LOCAL1)/select_all.c $(LOCAL1)/select_where.c $(LOCAL1)/insert.c $(LOCAL1)/update.c $(LOCAL2)/busca.c $(LOCAL2)/manipul_arq.c $(LOCAL2)/IO.c

FLAGS = -Wall -Wextra -std=c99 -g

# Diretórios para procurar arquivos de cabeçalho
INCLUDES = -I. -Ifuncionalidades -Ifuncoes_fornecidas -Ifuncionalidades_aux

all:
	gcc $(MAIN) $(FORNECIDAS) $(FUNCIONALIDADES) -o programaTrab $(FLAGS) $(INCLUDES)

run:
	./programaTrab

clean:
	rm -f programaTrab *.o *.bin