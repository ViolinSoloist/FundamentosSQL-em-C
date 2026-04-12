#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "estruturas.h"
#include "busca.h"
#include "fornecidas.h"

void lerRegistro(Registro* temp);
void lerLinhaBusca(FILE* file, ArgumentosCallback* args);
void mostrarRegistro(Registro* registro_lido);
void lerLinhaNormal(FILE* file, char* linha);

#endif