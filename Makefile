MAIN = projetoTrab.c

FORNECIDAS = funcoes_fornecidas/fornecidas.c

LOCAL1 = funcionalidades

LOCAL2 = funcionalidades_aux

FUNCIONALIDADES = $(LOCAL1)/create_table.c $(LOCAL1)/delete.c $(LOCAL1)/select_all.c $(LOCAL1)/update.c $(LOCAL2)/busca.c $(LOCAL2)/manipul_arq.c 

FLAGS = -std=c99

# Diretórios para procurar arquivos de cabeçalho
INCLUDES = -I. -Ifuncionalidades -Ifuncoes_fornecidas -Ifuncionalidades_aux

all:
	gcc $(MAIN) $(FORNECIDAS) $(FUNCIONALIDADES) -o programaTrab $(FLAGS) $(INCLUDES)

run:
	./programaTrab

clean:
	rm -f programaTrab *.o *.bin