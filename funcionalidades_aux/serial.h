#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "estruturas.h"

char *strsep(char **stringp, const char *delim);
void gravarRegistroBin(Registro* temporario, FILE* bin, char removido, int proxLista);
void binToStruct(Registro* regAtual, FILE* bin);
void salvaDadosNoRegistro(Registro* temporario, char* linha_entrada);

#endif
