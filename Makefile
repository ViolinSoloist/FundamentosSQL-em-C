.PHONY: all run clean zip

MAIN = projetoTrab.c

FORNECIDAS = funcoes_fornecidas/fornecidas.c

LOCAL1 = funcionalidades
LOCAL2 = funcionalidades_aux

FUNCIONALIDADES = \
    $(LOCAL1)/create_table.c \
    $(LOCAL1)/delete.c \
    $(LOCAL1)/select_all.c \
    $(LOCAL1)/select_where.c \
    $(LOCAL1)/insert.c \
    $(LOCAL1)/update.c \
    $(LOCAL2)/busca.c \
    $(LOCAL2)/cabecalho.c \
    $(LOCAL2)/serial.c \
    $(LOCAL2)/manipul_arq.c \
    $(LOCAL2)/terminal.c

FLAGS = -Wall -Wextra -std=c99 -g
INCLUDES = -I. -I$(LOCAL1) -Ifuncoes_fornecidas -I$(LOCAL2)

all:
	gcc $(MAIN) $(FORNECIDAS) $(FUNCIONALIDADES) -o programaTrab $(FLAGS) $(INCLUDES)

run:
	./programaTrab

clean:
	rm -f programaTrab *.o *.bin *.zip

zip:
	zip -r entrega.zip * -x "casos_IO/*" "README.md"

