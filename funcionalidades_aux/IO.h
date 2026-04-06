#ifndef PRINT_H
#define PRINT_H

#include "estruturas.h"
#include <stdio.h>

typedef void (*AcaoPosBusca)(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras);

void mostrarRegistro(Registro* registro_lido);
void lerLinhaBusca(FILE* file, AcaoPosBusca callback, void* dados_extras);

#endif