#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "estruturas.h"
#include "busca.h"
#include "fornecidas.h"

typedef void (*AcaoPosBusca)(FILE* file, int qtd_encontrados, long* offsets, void* dados_extras);
void lerLinhaBusca(FILE* file, AcaoPosBusca callback, void* dados_extras);
void mostrarRegistro(Registro* registro_lido);
void lerLinhaNormal(FILE* file, char* linha);

#endif