#ifndef BIN_STRUCT_H
#define BIN_STRUCT_H

#include <stdbool.h>
#include <stdio.h>

#include "estruturas.h"

/**
 * @private função auxiliar, le o registro atual do arq bin e copia os dados pra struct Registro
 * recebe ponteiro para @param regAtual e o arquivo @param bin
 */
void binToStruct(Registro* regAtual, FILE* bin);

#endif