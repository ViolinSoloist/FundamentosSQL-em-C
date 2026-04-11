#ifndef PRINT_H
#define PRINT_H

#include "estruturas.h"
#include <stdio.h>

typedef void (*AcaoPosBusca)(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras);

char *strsep(char **stringp, const char *delim);
void salvaDadosNoRegistro(Registro* temporario, char* linha_entrada);
void mostrarRegistro(Registro* registro_lido);
void lerLinhaBusca(FILE* file, AcaoPosBusca callback, void* dados_extras);
void lerLinhaNormal(FILE* file, char* linha);
void gravarRegistroBin(Registro* temporario, FILE* bin, char removido, int proxLista);
void lerRegistro(Registro* temporario);

#endif