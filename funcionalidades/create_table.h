#ifndef CREATE_TABLE_H
#define CREATE_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "estruturas.h" 
#include "fornecidas.h"
#include "cabecalho.h"
#include "serial.h"
#include "manipul_arq.h"

void create_table(const char *nomeArquivoCSV, const char *nomeArquivoBin);

#endif