#ifndef CREATE_INDEX_H
#define CREATE_INDEX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "cabecalho.h"
#include "serial.h"
#include "manipul_arq.h"
#include "../arvoreB/arvoreB.h"

void create_index(const char* ArquivoBin, const char* ArquivoIndiceBin);

#endif