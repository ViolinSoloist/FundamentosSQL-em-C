#ifndef INSERT_H
#define INSERT_H

#include <stdio.h>
#include <stdlib.h>

#include "estruturas.h"
#include "manipul_arq.h"
#include "fornecidas.h"
#include "busca.h"
#include "serial.h"
#include "cabecalho.h"
#include "terminal.h"
#include "arvoreB.h"

void insert_btree(const char* nomeArquivoBin, const char* nomeArquivoArvoreBin, int numeroLeituras);

#endif