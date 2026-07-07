.PHONY: all run clean zip subs memcheck

MAIN = projetoTrab.c

FORNECIDAS = funcoes_fornecidas/fornecidas.c

LOCAL1 = funcionalidades
LOCAL2 = funcionalidades_aux
LOCAL3 = arvoreB
LOCAL4 = juncao

FUNCIONALIDADES = $(LOCAL1)/create_table.c $(LOCAL1)/delete.c $(LOCAL1)/select_all.c $(LOCAL1)/select_where.c $(LOCAL1)/insert.c $(LOCAL1)/update.c $(LOCAL2)/busca.c $(LOCAL2)/cabecalho.c $(LOCAL2)/serial.c $(LOCAL2)/manipul_arq.c $(LOCAL2)/terminal.c $(LOCAL3)/arvoreB.c $(LOCAL3)/create_index.c $(LOCAL3)/insert_btree.c $(LOCAL3)/select_where_btree.c $(LOCAL3)/delete_btree.c $(LOCAL4)/select_where_juncao.c $(LOCAL4)/select_where_juncao_btree.c $(LOCAL4)/order_by.c $(LOCAL4)/select_where_juncao_merge.c 

FLAGS = -Wall -Wextra -std=c99 -g
INCLUDES = -I. -I$(LOCAL1) -Ifuncoes_fornecidas -I$(LOCAL2) -I$(LOCAL3) -I$(LOCAL4)

# adaptação para rodar em Windows e Linux
ifeq ($(OS),Windows_NT)
	RM = del /Q /F
	EXEC = programaTrab.exe
	RUN_CMD = programaTrab.exe
	ZIP_CMD = tar.exe -a -c -f entrega.zip $(MAIN) funcoes_fornecidas $(LOCAL1) $(LOCAL2) $(LOCAL3) $(LOCAL4) Makefile estruturas.h
else
	RM = rm -f
	EXEC = programaTrab
	RUN_CMD = ./programaTrab
	ZIP_CMD = zip -r entrega.zip * -x "IO/*" "README.md" "saidas"
endif

all:
	gcc $(MAIN) $(FORNECIDAS) $(FUNCIONALIDADES) -o $(EXEC) $(FLAGS) $(INCLUDES)

run:
	$(RUN_CMD)

clean:
	$(RM) $(EXEC) *.o *.bin *.zip

zip:
	$(ZIP_CMD)

subs:
	cp arq_entrada/* ./

memcheck:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXEC)